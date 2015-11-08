#pragma once

#include "common/Rendering/Renderer.h"
#include <kdtree++/kdtree.hpp>

class PhotonMappingRenderer : public Renderer
{
public:
    PhotonMappingRenderer(std::shared_ptr<class Scene> scene, std::shared_ptr<class ColorSampler> sampler);
    virtual void InitializeRenderer() override;
    glm::vec3 ComputeSampleColor(const struct IntersectionState& intersection, const class Ray& fromCameraRay) const override;

private:
};