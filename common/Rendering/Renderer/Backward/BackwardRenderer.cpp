#include "common/Rendering/Renderer/Backward/BackwardRenderer.h"
#include "common/Scene/Scene.h"
#include "common/Sampling/ColorSampler.h"
#include "common/Scene/Lights/Light.h"
#include "common/Scene/Geometry/Primitives/Primitive.h"
#include "common/Scene/Geometry/Mesh/MeshObject.h"
#include "common/Rendering/Material/Material.h"
#include "common/Intersection/IntersectionState.h"

#include "common/globalParams.h"

BackwardRenderer::BackwardRenderer(std::shared_ptr<Scene> scene, std::shared_ptr<ColorSampler> sampler) :
    Renderer(scene, sampler)
{
}

void BackwardRenderer::InitializeRenderer()
{
}

glm::vec3 BackwardRenderer::ComputeSampleColor(const IntersectionState& intersection, const Ray& fromCameraRay) const
{
    if (!intersection.hasIntersection) {
        return glm::vec3();
    }

    glm::vec3 intersectionPoint = intersection.intersectionRay.GetRayPosition(intersection.intersectionT);
    const MeshObject* parentObject = intersection.intersectedPrimitive->GetParentMeshObject();
    assert(parentObject);

    const Material* objectMaterial = parentObject->GetMaterial();
    assert(objectMaterial);

    // Compute the color at the intersection.
    glm::vec3 sampleColor;
    for (size_t i = 0; i < storedScene->GetTotalLights(); ++i) {
        const Light* light = storedScene->GetLightObject(i);
        assert(light);

        // Sample light using rays, Number of samples and where to sample is determined by the light.
        std::vector<Ray> sampleRays;
        light->ComputeSampleRays(sampleRays, intersectionPoint, intersection.ComputeNormal());
        glm::vec3 localLightColor = light->GetLightColor();

        for (size_t s = 0; s < sampleRays.size(); ++s) {
            // note that max T should be set to be right before the light.
            
#if TRANSPARENT_SHADOWS == 1
            // Need to create a shadow trace function that will handle transparent objects
            IntersectionState state(0, 0);
            state.currentIOR = 1;
            int hitOpaque = 0;
            //float dim = 0.f;
            while (storedScene->Trace(&sampleRays[s], &state)) {
                if (hitOpaque == MAX_TRANSMIT) {
                    break;
                }
                const MeshObject* hitMeshObject = state.intersectedPrimitive->GetParentMeshObject();
                const Material* hitMaterial = hitMeshObject->GetMaterial();
                if(hitMaterial->IsTransmissive()) {
                    // Send out a new ray from the intersection point in the same direction
                    glm::vec3 intPoint = state.intersectionRay.GetRayPosition(state.intersectionT);
                    // To compute the ray intersection point, you need to have the intersection t.
                    // So the new ray's maximum T is the old ray's maximum T subtracted by t.
                    light->ContinueSampleRay(sampleRays[s], intPoint, state.intersectionT);
                    // For attenuation, I'd imagine the value that makes the most sense is (1−t)d
                    // where t is the transmission percentage and d is the diffuse color.
                    // Finally, you multiply the light color by the attenuation value (i.e. when you
                    // call GetLightColor when you ComputeBRDF inside the Backward renderer).
                    float materialTransPerc = hitMaterial->GetTransmittance();
                    //std::cout<<"MaterialTransPerc is: "<<std::endl;
                    //std::cout<<materialTransPerc<<std::endl;
                    //glm::vec3 transAttenuation = glm::vec3(1.f, 1.f, 1.f) - hitMaterial->GetBaseDiffuseReflection();
                    glm::vec3 transAttenuation = hitMaterial->GetBaseDiffuseReflection();
                    //std::cout<<"Hit Material diffuse relfection is: "<<std::endl;
                    //std::cout<<glm::to_string(transAttenuation)<<std::endl;
                    transAttenuation = transAttenuation * materialTransPerc;
                    //std::cout<<"Final attenuation is : "<<std::endl;
                    //std::cout<<glm::to_string(transAttenuation)<<std::endl;
                    //dim += (float)(1/(MAX_TRANSMIT));
                    
                    glm::vec3 col1 = glm::vec3(transAttenuation[0], 0.f, 0.f);
                    glm::vec3 col2 = glm::vec3(0.f, transAttenuation[1], 0.f);
                    glm::vec3 col3 = glm::vec3(0.f, 0.f, transAttenuation[2]);
                    glm::mat3 transMatrix = glm::mat3(col1, col2, col3);
                    localLightColor = transMatrix * light->GetLightColor();
                    //std::cout<<"Original light color is :"<<std::endl;
                    //std::cout<<glm::to_string(light->GetLightColor())<<std::endl;
                    //std::cout<<"FINAL Diffuse color is :"<<std::endl;
                    //std::cout<<glm::to_string(localLightColor)<<std::endl;
                    //std::cout<<"Transformation matrix is :"<<std::endl;
                    //std::cout<<glm::to_string(transMatrix)<<std::endl;
                    
                    hitOpaque++;
                } else {
                    hitOpaque = MAX_TRANSMIT;
                    break;
                }
            }
            //std::cout<<"Broke out of loop."<<std::endl;
            if (hitOpaque == MAX_TRANSMIT) {
                continue;
            }
            //const float lightAttenuation = (light->ComputeLightAttenuation(intersectionPoint) - dim);
            const float lightAttenuation = light->ComputeLightAttenuation(intersectionPoint);
            // Note that the material should compute the parts of the lighting equation too.
            const glm::vec3 brdfResponse = objectMaterial->ComputeBRDF(intersection, localLightColor, sampleRays[s], fromCameraRay, lightAttenuation);
#else
            if(storedScene->Trace(&sampleRays[s], nullptr)){
                continue;
            }
            const float lightAttenuation = light->ComputeLightAttenuation(intersectionPoint);
            // Note that the material should compute the parts of the lighting equation too.
            const glm::vec3 brdfResponse = objectMaterial->ComputeBRDF(intersection, localLightColor, sampleRays[s], fromCameraRay, lightAttenuation);
#endif
            sampleColor += brdfResponse;
        }
    }
    sampleColor += objectMaterial->ComputeNonLightDependentBRDF(this, intersection);
    return sampleColor;
}
