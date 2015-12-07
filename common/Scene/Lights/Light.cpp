#include "common/Scene/Lights/Light.h"

glm::vec3 Light::GetLightColor() const
{
    return lightColor;
}

void Light::SetLightColor(glm::vec3 input)
{
    lightColor = input;
}

void Light::ContinueSampleRay(Ray& output, glm::vec3 origin, float oldT) const
{
    //origin += normal * LARGE_EPSILON;
    origin += output.GetRayDirection() * LARGE_EPSILON;
    output.SetRayPosition(origin);
    //const glm::vec3 lightPosition = glm::vec3(GetPosition());
    //const glm::vec3 rayDirection = glm::normalize(lightPosition - origin);
    //const float distanceToOrigin = glm::distance(origin, lightPosition);
    //output.SetRayPosition(origin);
    //output.SetRayDirection(rayDirection);
    // To compute the ray intersection point, you need to have the intersection t.
    // So the new ray's maximum T is the old ray's maximum T subtracted by t.
    output.SetMaxT(output.GetMaxT() - oldT);
}