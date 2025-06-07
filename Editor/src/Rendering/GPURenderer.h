#pragma once
#include "Renderer.h"
#include <memory>
#include <vector>
#include <glm/glm.hpp>

#include "Platform/Image.h"
#include "Ray.h"

class GPURenderer : public Renderer
{
public:
    GPURenderer();
    ~GPURenderer();

    void Render(const Camera& camera, const Scene& scene) override;
    void OnResize(uint32_t width, uint32_t height) override;

    unsigned int GetFinalImageID() const override;
    bool isValidImage() const override;
    bool isGPU() const override { return false; };
};
