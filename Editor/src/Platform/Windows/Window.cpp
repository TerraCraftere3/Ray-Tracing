#include "Window.h"
#include "Core/Log.h"

#ifdef PLATFORM_WINDOWS
Window::Window(std::string title, int width, int height)
{
	if (!glfwInit())
	{
		LOG_CRITICAL("Failed to initialize GLFW");
		return;
	}

	m_Window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if (!m_Window)
	{
		LOG_CRITICAL("Failed to create GLFW window");
		glfwTerminate();
		return;
	}
	LOG_INFO("Window created successfully: {} ({}x{})", title, width, height);

	glfwMakeContextCurrent(m_Window);
	glfwSwapInterval(1); // Enable VSync
}

void Window::shutdown()
{
	if (m_Window)
	{
		glfwDestroyWindow(m_Window);
		m_Window = nullptr;
	}
	glfwTerminate();
	LOG_INFO("Window shutdown successfully");
}

void Window::beginFrame()
{
	glfwPollEvents();
	if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(m_Window, true);
	}
}

void Window::endFrame()
{
	glfwSwapBuffers(m_Window);
}

bool Window::shouldClose()
{
	return glfwWindowShouldClose(m_Window);
}

glm::vec2 Window::getSize()
{
	return glm::vec2();
}

std::string Window::getTitle()
{
	return std::string();
}

GLFWwindow* Window::getNativeWindow()
{
	return m_Window;
}
#endif
