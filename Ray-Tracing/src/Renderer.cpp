#include "Renderer.h"

Renderer::Renderer(GLFWwindow* window)
{
	m_Window = window;
}

void Renderer::renderImGui()
{
	ImGui::Begin("Debug");
	ImGui::Text("Test");
	ImGui::End();
}

void Renderer::renderOpenGL()
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}
