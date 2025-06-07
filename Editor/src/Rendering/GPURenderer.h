#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <filesystem>
#include "Renderer.h"
#include "Camera.h"
#include "Scene.h"

class GPURenderer : public Renderer {
public:
    GPURenderer();
    ~GPURenderer();

    void Render(const Camera& camera, const Scene& scene) override;
    void OnResize(uint32_t width, uint32_t height) override;

    unsigned int GetFinalImageID() const override;
    bool isValidImage() const override;
    bool isGPU() const override { return true; };

    ImVec2 GetViewportBottomLeft() const override { return ImVec2(0, 1); };
    ImVec2 GetViewportTopRight() const override { return ImVec2(1, 0); };

private:
    GLuint m_QuadVAO = 0;
    GLuint m_QuadVBO = 0;
    GLuint m_ShaderProgram = 0;

    GLuint m_Framebuffer = 0;
    GLuint m_Texture = 0;
    uint32_t m_Width = 0, m_Height = 0;

    void InitQuad();
    void InitShader();
    std::string LoadShaderWithIncludes(const std::string& filePath, const std::string& from="");
};
