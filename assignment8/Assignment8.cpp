#include "assignment8/Assignment8.h"
#include "common/core.h"

#define GLASS 0
#define NUM_BOUNCES 0

std::shared_ptr<Camera> Assignment8::CreateCamera() const
{
    const glm::vec2 resolution = GetImageOutputResolution();
    std::shared_ptr<Camera> camera = std::make_shared<PerspectiveCamera>(resolution.x / resolution.y, 26.6f);
    camera->SetPosition(glm::vec3(0.f, -4.1469f, 0.73693f));
    camera->Rotate(glm::vec3(1.f, 0.f, 0.f), PI / 2.f);
    return camera;
}

std::shared_ptr<Scene> Assignment8::CreateScene() const
{
    std::shared_ptr<Scene> newScene = std::make_shared<Scene>();

    // Material
    std::shared_ptr<BlinnPhongMaterial> cubeMaterial = std::make_shared<BlinnPhongMaterial>();
    cubeMaterial->SetDiffuse(glm::vec3(1.f, 1.f, 1.f));
    cubeMaterial->SetSpecular(glm::vec3(0.6f, 0.6f, 0.6f), 40.f);

    // Objects
    std::vector<std::shared_ptr<aiMaterial>> loadedMaterials;
    std::vector<std::shared_ptr<MeshObject>> cubeObjects = MeshLoader::LoadMesh("CornellBox/CornellBox-Assignment8.obj", &loadedMaterials);
    for (size_t i = 0; i < cubeObjects.size(); ++i) {
        std::shared_ptr<Material> materialCopy = cubeMaterial->Clone();
        materialCopy->LoadMaterialFromAssimp(loadedMaterials[i]);
        
#if GLASS == 1
        if (i == 5 || i == 6) {
            materialCopy->SetTransmittance(0.77);
            materialCopy->SetIOR(1.52);
            materialCopy->SetReflectivity(0.07);
        }
#endif
        
        cubeObjects[i]->SetMaterial(materialCopy);
    }

    std::shared_ptr<SceneObject> cubeSceneObject = std::make_shared<SceneObject>();
    cubeSceneObject->AddMeshObject(cubeObjects);
    cubeSceneObject->Rotate(glm::vec3(1.f, 0.f, 0.f), PI / 2.f);
    cubeSceneObject->CreateAccelerationData(AccelerationTypes::BVH);
    newScene->AddSceneObject(cubeSceneObject);

    // Lights
    std::shared_ptr<PointLight> pointLight = std::make_shared<PointLight>();
    pointLight->SetPosition(glm::vec3(0.01909f, 0.0101f, 1.97028f));
    pointLight->SetLightColor(glm::vec3(1.f, 1.f, 1.f));
    
#define ACCELERATION_TYPE 1
#if ACCELERATION_TYPE == 0
    newScene->GenerateAccelerationData(AccelerationTypes::NONE);
#elif ACCELERATION_TYPE == 1
    newScene->GenerateAccelerationData(AccelerationTypes::BVH);
#else
    UniformGridAcceleration* accelerator = dynamic_cast<UniformGridAcceleration*>(newScene->GenerateAccelerationData(AccelerationTypes::UNIFORM_GRID));
    assert(accelerator);
    // Change the glm::ivec3(10, 10, 10) here.
    accelerator->SetSuggestedGridSize(glm::ivec3(3, 3, 3));
#endif
    
    newScene->AddLight(pointLight);

    return newScene;

}

std::shared_ptr<ColorSampler> Assignment8::CreateSampler() const
{
    std::shared_ptr<JitterColorSampler> jitter = std::make_shared<JitterColorSampler>();
    //jitter->SetGridSize(glm::ivec3(1, 1, 1));
    jitter->SetGridSize(glm::ivec3(4, 4, 1));
    
    std::shared_ptr<SimpleAdaptiveSampler> sampler = std::make_shared<SimpleAdaptiveSampler>();
    sampler->SetInternalSampler(jitter);
    
    // Change the '1.f' in '1.f * SMALL_EPSILON' here to be higher and see what your results are.
    sampler->SetEarlyExitParameters(SMALL_EPSILON, 16);
    //return sampler;
    
    // Comment out 'return jitter;' to use the adaptive sampler.
    jitter->SetGridSize(glm::ivec3(1, 1, 1));
    return jitter;
}

std::shared_ptr<class Renderer> Assignment8::CreateRenderer(std::shared_ptr<Scene> scene, std::shared_ptr<ColorSampler> sampler) const
{
//    return std::make_shared<BackwardRenderer>(scene, sampler);
    return std::make_shared<PhotonMappingRenderer>(scene, sampler);
}

int Assignment8::GetSamplesPerPixel() const
{
    // ASSIGNMENT 5 TODO: Change the '1' here to increase the maximum number of samples used per pixel. (Part 1).
    return 1;
}

bool Assignment8::NotifyNewPixelSample(glm::vec3 inputSampleColor, int sampleIndex)
{
    return true;
}

int Assignment8::GetMaxReflectionBounces() const
{
    return NUM_BOUNCES;
}

int Assignment8::GetMaxRefractionBounces() const
{
    return NUM_BOUNCES;
}

glm::vec2 Assignment8::GetImageOutputResolution() const
{
    return glm::vec2(640.f, 480.f);
}