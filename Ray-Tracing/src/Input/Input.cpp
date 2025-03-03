#include "Input.h"
#include <imgui.h>

GLFWwindow* Input::m_Window = nullptr;

Input::Input(GLFWwindow* window)
{
	m_Window = window;
	LOG_INFO("Input initialized!");
}

bool Input::IsKeyDown(KeyCode keycode)
{
	ImGuiIO& io = ImGui::GetIO();
	GLFWwindow* windowHandle = m_Window;
	int state = glfwGetKey(windowHandle, (int)keycode);
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Input::IsMouseButtonDown(MouseButton button)
{
	ImGuiIO& io = ImGui::GetIO();
	GLFWwindow* windowHandle = m_Window;
	int state = glfwGetMouseButton(windowHandle, (int)button);
	return state == GLFW_PRESS;
}

glm::vec2 Input::GetMousePosition()
{
	GLFWwindow* windowHandle = m_Window;

	double x, y;
	glfwGetCursorPos(windowHandle, &x, &y);
	return { (float)x, (float)y };
}

void Input::SetCursorMode(CursorMode mode)
{
	GLFWwindow* windowHandle = m_Window;
	glfwSetInputMode(windowHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL + (int)mode);
}