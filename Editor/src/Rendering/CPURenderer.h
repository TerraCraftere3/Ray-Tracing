#pragma once
#include "Renderer.h"
#include <memory>
#include <vector>
#include <glm/glm.hpp>

#include "Platform/Image.h"
#include "Ray.h"

class CPURenderer : public Renderer
{
public:
    CPURenderer();
    ~CPURenderer();

    void Render(const Camera& camera, const Scene& scene) override;
    void OnResize(uint32_t width, uint32_t height) override;

    unsigned int GetFinalImageID() const override;
    bool isValidImage() const override;
    bool isGPU() const override { return false; }; 
    
    ImVec2 GetViewportBottomLeft() const override { return ImVec2(0, 0); };
    ImVec2 GetViewportTopRight() const override { return ImVec2(1, 1); };

private:
    struct HitPayload
    {
        float HitDistance;
        int ObjectIndex = -1;
        glm::vec3 WorldPosition;
        glm::vec3 WorldNormal;
        glm::vec2 UV;
        int MeshIndex = -1;
        int TriangleIndex = -1;
        float BaryU = 0.0f, BaryV = 0.0f;
    };

    glm::vec4 PerPixel(uint32_t x, uint32_t y);

    HitPayload TraceRay(const Ray& ray);
    HitPayload ClosestHit(const Ray& ray, float t, int ObjectIndex);
    HitPayload ClosestHit(const Ray& ray, float t, int meshIndex, int triIndex, float u, float v);
    HitPayload Miss(const Ray& ray);
    bool IntersectTriangle(const Ray& ray, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& t, float& u, float& v);

private:
    std::shared_ptr<Image> m_FinalImage;

    std::vector<uint32_t> m_ImageHorizontalIter, m_ImageVerticalIter;

    const Scene* m_ActiveScene = nullptr;
    const Camera* m_ActiveCamera = nullptr;

    uint32_t* m_ImageData = nullptr;
    glm::vec4* m_AccumulationData = nullptr;
};
