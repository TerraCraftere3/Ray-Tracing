#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <glm/glm.hpp>

class Window {
public:
	Window(std::string title, int width=800, int height=600);

	void shutdown();
	void beginFrame();
	void endFrame();

	bool shouldClose();
	glm::vec2 getSize();
	std::string getTitle();
	GLFWwindow* getNativeWindow();
private:
	GLFWwindow* m_Window = nullptr;
};