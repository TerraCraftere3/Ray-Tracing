#pragma once
#include <GLFW/glfw3.h>
#include "imgui.h"

class Renderer {
public:
	Renderer(GLFWwindow* window);
	void renderImGui();
	void renderOpenGL();
private:
	GLFWwindow* m_Window;
};