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
    // To compute the ray intersection point, you need to have the intersection t.
    // So the new ray's maximum T is the old ray's maximum T subtracted by t.
    output.SetMaxT(oldT - glm::length(origin - output.GetRayPosition(0)));
    origin += output.GetRayDirection() * LARGE_EPSILON;
    output.SetRayPosition(origin);
}