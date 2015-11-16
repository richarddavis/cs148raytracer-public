#include "common/Scene/Lights/Point/PointLight.h"
#include <random>


void PointLight::ComputeSampleRays(std::vector<Ray>& output, glm::vec3 origin, glm::vec3 normal) const
{
    origin += normal * LARGE_EPSILON;
    const glm::vec3 lightPosition = glm::vec3(GetPosition());
    const glm::vec3 rayDirection = glm::normalize(lightPosition - origin);
    const float distanceToOrigin = glm::distance(origin, lightPosition);
    output.emplace_back(origin, rayDirection, distanceToOrigin);
}

float PointLight::ComputeLightAttenuation(glm::vec3 origin) const
{
    return 1.f;
}

void PointLight::GenerateRandomPhotonRay(Ray& ray) const
{
    // Assignment 7 TODO: Fill in the random point light samples here.
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-1, 1);
    
    float rx = dis(gen);
    float ry = dis(gen);
    float rz = dis(gen);

    while ((pow(rx, 2) + pow(ry, 2) + pow(rz, 2)) > 1) {
        rx = dis(gen);
        ry = dis(gen);
        rz = dis(gen);
    }
    
    const glm::vec3 dirVector = glm::vec3(rx, ry, rz);
    ray.SetRayDirection(glm::normalize(dirVector));
    const glm::vec3 lightPosition = glm::vec3(GetPosition());
    //0.01909f, 0.0101f, 1.77640f
    //const glm::vec3 lightPosition = glm::vec3(0.f, 3.f, 1.77640f);
    ray.SetRayPosition(lightPosition);
    //std::cout<<glm::to_string(ray.GetPosition())<<std::endl;
}