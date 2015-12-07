#pragma once

#include "common/Scene/SceneObject.h"
#include "common/Scene/Geometry/Ray/Ray.h"

class Light : public SceneObject
{
public:
    virtual void ComputeSampleRays(std::vector<Ray>& output, glm::vec3 origin, glm::vec3 normal) const = 0;
    virtual float ComputeLightAttenuation(glm::vec3 origin) const = 0;

    virtual glm::vec3 GetLightColor() const;
    void SetLightColor(glm::vec3 input);
    void ContinueSampleRay(Ray& output, glm::vec3 origin, float oldT) const;

    // Photon Mapping Utility Functions
    virtual void GenerateRandomPhotonRay(Ray& ray) const = 0;

protected:
    glm::vec3 lightColor;
};