#pragma once

#include "KeyCodes.h"

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include "Core.h"

class Input
{
public:
	Input(GLFWwindow* window);
	static bool IsKeyDown(KeyCode keycode);
	static bool IsMouseButtonDown(MouseButton button);

	static glm::vec2 GetMousePosition();

	static void SetCursorMode(CursorMode mode);
private:
	static GLFWwindow* m_Window;
};
