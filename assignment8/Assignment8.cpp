#include "assignment8/Assignment8.h"
#include "common/core.h"

#include "common/globalParams.h"

std::shared_ptr<Camera> Assignment8::CreateCamera() const
{
    const glm::vec2 resolution = GetImageOutputResolution();
    std::shared_ptr<Camera> camera = std::make_shared<PerspectiveCamera>(resolution.x / resolution.y, 26.6f);
    //Position is given by (left-right, front-back, top-bottom)
    camera->SetPosition(glm::vec3(0.f, -4.5f, 1.93693f));
    camera->Rotate(glm::vec3(1.f, 0.f, 0.f), PI / 2.f);
    camera->Rotate(glm::vec3(1.f, 0.f, 0.f), -13.f * PI / 180.f);
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
    std::vector<std::shared_ptr<MeshObject>> cubeObjects = MeshLoader::LoadMesh("final_scene/cornell_empty.obj", &loadedMaterials);
    std::cout<<cubeObjects.size()<<std::endl;
    for (size_t i = 0; i < cubeObjects.size(); ++i) {
        std::shared_ptr<Material> materialCopy = cubeMaterial->Clone();
        materialCopy->LoadMaterialFromAssimp(loadedMaterials[i]);
#if MATERIAL_HACK == 1
        if (i > 20 || i < 3) {
            materialCopy->SetTransmittance(0.77);
            materialCopy->SetIOR(1.7);
            materialCopy->SetReflectivity(0.07);
        }
#endif
        materialCopy->SetAmbient(glm::vec3(0.f, 0.f, 0.f));
        cubeObjects[i]->SetMaterial(materialCopy);

        std::shared_ptr<SceneObject> cubeSceneObject = std::make_shared<SceneObject>();
        cubeSceneObject->AddMeshObject(cubeObjects[i]);
        cubeSceneObject->Rotate(glm::vec3(1.f, 0.f, 0.f), PI / 2.f);
        cubeSceneObject->CreateAccelerationData(AccelerationTypes::BVH);
    
        cubeSceneObject->ConfigureAccelerationStructure([](AccelerationStructure* genericAccelerator) {
            BVHAcceleration* accelerator = dynamic_cast<BVHAcceleration*>(genericAccelerator);
            accelerator->SetMaximumChildren(2);
            accelerator->SetNodesOnLeaves(2);
        });
    
        cubeSceneObject->ConfigureChildMeshAccelerationStructure([](AccelerationStructure* genericAccelerator) {
            BVHAcceleration* accelerator = dynamic_cast<BVHAcceleration*>(genericAccelerator);
            accelerator->SetMaximumChildren(2);
            accelerator->SetNodesOnLeaves(2);
        });
    
        newScene->AddSceneObject(cubeSceneObject);
    }

    // Lights
    std::shared_ptr<PointLight> pointLight = std::make_shared<PointLight>();
    //pointLight->SetPosition(glm::vec3(0.01909f, 0.0101f, 1.97028f));
    pointLight->SetPosition(glm::vec3(0.02f, 0.1f, 1.9f)); // Slightly change to light positions
    pointLight->SetLightColor(glm::vec3(1.f, 1.f, 1.f));
    
    std::shared_ptr<PointLight> pointLight2 = std::make_shared<PointLight>();
    pointLight2->SetPosition(glm::vec3(-0.2f, -1.6f, 1.f)); // Slightly change to light positions
    pointLight2->SetLightColor(glm::vec3(1.f, 1.f, 1.f));
    
    std::shared_ptr<AreaLight> areaLight = std::make_shared<AreaLight>(glm::vec2(1.f,1.f));
    // Keep the area light size at 1, 1 and the sampler attributes at 1, 1. Then any settings in Blender
    // will apply here.
    areaLight->SetPosition(glm::vec3(0.f, 0.f, 1.95f));
    //areaLight->Rotate(glm::vec3(1.f, 0.f, 0.f), 90 * (PI / 180.f));
    //areaLight->Rotate(glm::vec3(0.f, 0.f, 1.f), 65 * (PI / 180.f));
    areaLight->SetLightColor(glm::vec3(1.f, 1.0f, 1.0f));
    areaLight->SetSamplerAttributes(glm::ivec3(1.f, 1.f, 1.f), AREA_LIGHT_SAMPLES);
    
    //newScene->AddLight(pointLight);
    //newScene->AddLight(pointLight2);
    newScene->AddLight(areaLight);

    // -------------------------------
    // Change the type of acceleration.
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
    // -------------------------------

    return newScene;

}

std::shared_ptr<ColorSampler> Assignment8::CreateSampler() const
{
#if ANTI_ALIAS == 1
    std::shared_ptr<JitterColorSampler> jitter = std::make_shared<JitterColorSampler>();
    
    // The higher the grid size, the better the anti-aliasing but the slower it goes.
    jitter->SetGridSize(glm::ivec3(4, 4, 1));
    
    std::shared_ptr<SimpleAdaptiveSampler> sampler = std::make_shared<SimpleAdaptiveSampler>();
    sampler->SetInternalSampler(jitter);
    
    // Change the '1.f' in '1.f * SMALL_EPSILON' here to be higher and see what your results are.
    // Increasing the multiplier should speed up execution. We were encouraged to try values of
    // 10.f and 100.f.
    sampler->SetEarlyExitParameters(2.f * SMALL_EPSILON, 4);
    
    // Comment out 'return jitter;' to use the adaptive sampler.
    return sampler;
#else
    // This is how the code was given to us:
    std::shared_ptr<JitterColorSampler> jitter = std::make_shared<JitterColorSampler>();
    jitter->SetGridSize(glm::ivec3(1, 1, 1));
    return jitter;
#endif
}

std::shared_ptr<class Renderer> Assignment8::CreateRenderer(std::shared_ptr<Scene> scene, std::shared_ptr<ColorSampler> sampler) const
{
#if PHOTON_MAPPING == 0
    return std::make_shared<BackwardRenderer>(scene, sampler);
#else
    return std::make_shared<PhotonMappingRenderer>(scene, sampler);
#endif
}

int Assignment8::GetSamplesPerPixel() const
{
    // ASSIGNMENT 5 TODO: Change the '1' here to increase the maximum number of samples used per pixel. (Part 1).
    return SAMPLES_PER_PIXEL;
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
//    return glm::vec2(960.f, 720.f);
    return glm::vec2(640.f, 480.f);
}