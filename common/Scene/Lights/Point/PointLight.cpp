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
    
    float rx = -1 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(2.f)));
    float ry = -1 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(2.f)));
    float rz = -1 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(2.f)));

    while ((pow(rx, 2) + pow(ry, 2) + pow(rz, 2)) > 1) {
        rx = -1 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(2.f)));
        ry = -1 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(2.f)));
        rz = -1 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(2.f)));
    }
    
    const glm::vec3 dirVector = glm::vec3(rx, ry, rz);
    ray.SetRayDirection(glm::normalize(dirVector));
    const glm::vec3 lightPosition = glm::vec3(GetPosition());
    ray.SetRayPosition(lightPosition);
}