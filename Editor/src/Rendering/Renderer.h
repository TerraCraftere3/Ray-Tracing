#pragma once
#include <cstdint>
#include <memory>

#include "Camera.h"
#include "Scene.h"

#include "Platform/ImGui.h"

class Renderer
{
public:
    struct Settings
    {
        bool Accumulate = true;
        bool MultiThreaded = true;
    };

    virtual ~Renderer() = default;

    virtual void Render(const Camera& camera, const Scene& scene) = 0;
    virtual void OnResize(uint32_t width, uint32_t height) = 0;

    virtual unsigned int GetFinalImageID() const = 0;
    virtual bool isValidImage() const = 0;

    virtual Settings& GetSettings() { return m_Settings; }
    virtual void ResetFrameIndex() { m_FrameIndex = 1; }
    virtual bool isGPU() const = 0;

    virtual ImVec2 GetViewportBottomLeft() const = 0;
    virtual ImVec2 GetViewportTopRight() const = 0;

protected:
    Settings m_Settings;
    uint32_t m_FrameIndex = 1;
};
