#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <Platform/Window.h>

class UIManager
{
public:
	UIManager(Window* window);

	void shutdown();
	void beginFrame();
	void endFrame();
private:
	Window* m_Window;
	ImGuiContext* m_Context = nullptr;
};