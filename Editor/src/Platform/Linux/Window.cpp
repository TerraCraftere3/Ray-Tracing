#ifdef PLATFORM_LINUX
#include "Window.h"
#include "Core/Log.h"

Window::Window(std::string title, int width, int height)
{
	if (!glfwInit())
	{
		LOG_CRITICAL("Failed to initialize GLFW");
		return;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	m_Window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if (!m_Window)
	{
		LOG_CRITICAL("Failed to create GLFW window");
		glfwTerminate();
		return;
	}
	LOG_INFO("Window created successfully: {} ({}x{})", title, width, height);

	glfwMakeContextCurrent(m_Window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		LOG_CRITICAL("Failed to initialize GLAD");
	}
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
