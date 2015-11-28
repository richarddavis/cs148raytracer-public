#include "common/Rendering/Renderer/Photon/PhotonMappingRenderer.h"
#include "common/Scene/Scene.h"
#include "common/Sampling/ColorSampler.h"
#include "common/Scene/Lights/Light.h"
#include "common/Scene/Geometry/Primitives/Primitive.h"
#include "common/Scene/Geometry/Mesh/MeshObject.h"
#include "common/Rendering/Material/Material.h"
#include "common/Intersection/IntersectionState.h"
#include "common/Scene/SceneObject.h"
#include "common/Scene/Geometry/Mesh/MeshObject.h"
#include "common/Rendering/Material/Material.h"
#include "glm/gtx/component_wise.hpp"
#include <random>

#define VISUALIZE_PHOTON_MAPPING 1

PhotonMappingRenderer::PhotonMappingRenderer(std::shared_ptr<class Scene> scene, std::shared_ptr<class ColorSampler> sampler):
    BackwardRenderer(scene, sampler), 
    diffusePhotonNumber(1000000),
    maxPhotonBounces(1000)
{
    srand(static_cast<unsigned int>(time(NULL)));
}

void PhotonMappingRenderer::InitializeRenderer()
{
    // Generate Photon Maps
    GenericPhotonMapGeneration(diffuseMap, diffusePhotonNumber);
    diffuseMap.optimise();
}

void PhotonMappingRenderer::GenericPhotonMapGeneration(PhotonKdtree& photonMap, int totalPhotons)
{
    float totalLightIntensity = 0.f;
    size_t totalLights = storedScene->GetTotalLights();
    for (size_t i = 0; i < totalLights; ++i) {
        const Light* currentLight = storedScene->GetLightObject(i);
        if (!currentLight) {
            continue;
        }
        totalLightIntensity = glm::length(currentLight->GetLightColor());
    }

    // Shoot photons -- number of photons for light is proportional to the light's intensity relative to the total light intensity of the scene.
    for (size_t i = 0; i < totalLights; ++i) {
        const Light* currentLight = storedScene->GetLightObject(i);
        if (!currentLight) {
            continue;
        }

        const float proportion = glm::length(currentLight->GetLightColor()) / totalLightIntensity;
        const int totalPhotonsForLight = static_cast<const int>(proportion * totalPhotons);
        const glm::vec3 photonIntensity = currentLight->GetLightColor() / static_cast<float>(totalPhotonsForLight);
        for (int j = 0; j < totalPhotonsForLight; ++j) {
            Ray photonRay;
            std::vector<char> path;
            path.push_back('L');
            currentLight->GenerateRandomPhotonRay(photonRay);
            TracePhoton(photonMap, &photonRay, photonIntensity, path, 1.f, maxPhotonBounces);
        }
    }
}

void PhotonMappingRenderer::TracePhoton(PhotonKdtree& photonMap, Ray* photonRay, glm::vec3 lightIntensity, std::vector<char>& path, float currentIOR, int remainingBounces)
{
    assert(photonRay);
    IntersectionState state(0, 0);
    state.currentIOR = currentIOR;

    // **********************************************************************************************
    // Start RLD code here.
    //
    // Overview of the code
    // There exists a maximum number of bounces that we consider before killing off the photon.
    // By default this is set to 1000.
    // This function has a parameter named remainingBounces. If this number is less than zero,
    // the function should terminate.
    if (remainingBounces < 0) {
        return;
    }
    
    // Part 1. Store the photon when it hits an object in the scene.
    
    // This function returns true if there is an intersection and false otherwise.
    // If the photon does not hit anything simply exit the function.
    bool hit_something = storedScene->Trace(photonRay, &state);
    if (hit_something == false) {
        return;
    }
    
    // If there was an intersection decide (1) whether or not to store the photon and (2) whether or not to
    // scatter the photon. For now we deal with (1).
    //
    // I will only store photons that have bounced around the scene, ignoring those that have come directly
    // from the light. If the path variable only has one element in it, 'L', this means that the photon has
    // come directly from the light. A quick way to test if this is the case is to check the length of the
    // path variable. When the length is 1, don't store the photon.
    // When the length is greater than 1, create and store a new 'Photon' object in the photon map.
    // The 'Photon' struct has three properties: position, intensity, and toLightRay.
    //
    // The 'intensity' varible should be set to 'lightIntensity' and the 'toLightRay' varible should be set
    // to a ray that points in the opposite direction of the photon ray.
    //
    // You can compute the intersection point using the information stored in the 'IntersectionState' object.
    // const glm::vec3 intersectionPoint = state.intersectionRay.GetRayPosition(state.intersectionT);
    
    Photon myPhoton;
    const glm::vec3 intersectionPoint = state.intersectionRay.GetRayPosition(state.intersectionT);
    myPhoton.intensity = lightIntensity;
    Ray toLight;
    toLight.SetRayDirection(-photonRay->GetRayDirection());
    toLight.SetRayPosition(intersectionPoint);
    myPhoton.position = intersectionPoint;
    myPhoton.toLightRay = toLight;
    if (path.size() > 1) {
        photonMap.insert(myPhoton);
    }
    
    // Part 2. Decide whether to scatter or absorb the photon
    const MeshObject* hitMeshObject = state.intersectedPrimitive->GetParentMeshObject();
    const Material* hitMaterial = hitMeshObject->GetMaterial();
    glm::vec3 diffuseColor = hitMaterial->GetBaseDiffuseReflection();
    //std::cout<<glm::to_string(diffuseColor)<<std::endl;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);
    
    float pr = dis(gen);
    if (diffuseColor[0] < pr || diffuseColor[1] < pr || diffuseColor[2] < pr) {
        // Scatter the photon. Otherwise do nothing.
        // To scatter the photon shoot out a ray randomly into the hemisphere above the intersection point.
        // To do this, (1) perform a generic hemisphere sample and (2) transform the ray into world space.
        double u1 = dis(gen);
        double u2 = dis(gen);
        double r = sqrt(u1);
        double theta = 2 * PI * u2;
        double x = r * cos(theta);
        double y = r * sin(theta);
        double z = sqrt(1 - u1);
        glm::vec3 scatterDirection = glm::normalize(glm::vec3(x, y, z));
        
        // Calculate the tangent, bittangent vectors
        glm::vec3 t;
        glm::vec3 n = state.ComputeNormal();
        glm::vec3 unit1 = glm::vec3(1.f, 0.f, 0.f);
        glm::vec3 unit2 = glm::vec3(0.f, 1.f, 0.f);
        if (std::abs(std::abs(glm::dot(n, unit1)) - 1) < 0.1) {
            t = glm::cross(n, unit2);
        } else {
            t = glm::cross(n, unit1);
        }
        glm::vec3 b = glm::cross(n, t);

        glm::vec3 tn = glm::normalize(t);
        glm::vec3 bn = glm::normalize(b);
        glm::vec3 nn = glm::normalize(n);
        
        // Transform the vector using the matrix
        glm::vec3 finalDirection = glm::normalize(glm::mat3(tn, bn, nn) * scatterDirection);
        
        Ray newRay;
        newRay.SetRayDirection(finalDirection);
        newRay.SetRayPosition(intersectionPoint + LARGE_EPSILON * n);
        path.push_back('L');
        remainingBounces -= 1;
        TracePhoton(photonMap, &newRay, lightIntensity, path, currentIOR, remainingBounces);
    }
}

glm::vec3 PhotonMappingRenderer::ComputeSampleColor(const struct IntersectionState& intersection, const class Ray& fromCameraRay) const
{
    glm::vec3 finalRenderColor = BackwardRenderer::ComputeSampleColor(intersection, fromCameraRay);
#if VISUALIZE_PHOTON_MAPPING
    Photon intersectionVirtualPhoton;
    intersectionVirtualPhoton.position = intersection.intersectionRay.GetRayPosition(intersection.intersectionT);
    
    // Find the intersected object
    const MeshObject* parentObject = intersection.intersectedPrimitive->GetParentMeshObject();
    assert(parentObject);
    
    // Get the material of the intersected object
    const Material* objectMaterial = parentObject->GetMaterial();
    assert(objectMaterial);

    std::vector<Photon> foundPhotons;
    diffuseMap.find_within_range(intersectionVirtualPhoton, 0.003f, std::back_inserter(foundPhotons));
    if (!foundPhotons.empty()) {
        // The following line of code shows where each photon intersects geometry in the scene
        // finalRenderColor += glm::vec3(1.f, 0.f, 0.f);
        
        // 1. Iterate over all the photons in the foundPhotons vector.
        for (Photon p : foundPhotons) {
            //const float lightAttenuation = light->ComputeLightAttenuation(intersectionPoint);
            
            // Use the material BRDF to find the response from each photon in the sampling area
            const glm::vec3 brdfResponse = objectMaterial->ComputeBRDF(intersection, p.intensity, p.toLightRay, fromCameraRay, 1.0f);
            //p.position;
            //p.toLightRay;
            glm::vec3 scaledResponse = brdfResponse * (float)(foundPhotons.size() / (3.1415f * pow(0.003f, 2.f)));
            std::cout<<glm::to_string(scaledResponse)<<std::endl;
            finalRenderColor += scaledResponse;
        }
    }
#endif
    std::cout<<"FINAL COMPUTATION!!!"<<std::endl;
    std::cout<<glm::to_string(finalRenderColor)<<std::endl;
    std::cout<<""<<std::endl;
    return finalRenderColor;
}

void PhotonMappingRenderer::SetNumberOfDiffusePhotons(int diffuse)
{
    diffusePhotonNumber = diffuse;
}