#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"

struct RendererOpenGLData {
	float vertices[3*4];
	void setVertices(const float newVertices[3*4]) {
		for (int i = 0; i < 3*4; i++) {
			vertices[i] = newVertices[i];
		}
	}

	unsigned int VBO;
	void setVBO(const unsigned int newVBO) {
		VBO = newVBO;
	}

	unsigned int VAO;
	void setVAO(const unsigned int newVAO) {
		VAO = newVAO;
	}

	unsigned int EBO;
	void setEBO(const unsigned int newEBO) {
		EBO = newEBO;
	}

	unsigned int shaderProgram;
	void setShaderProgram(const unsigned int newShaderProgram) {
		shaderProgram = newShaderProgram;
	}
};

struct RendererImGuiData {
};

class Renderer {
public:
	Renderer(GLFWwindow* window);
	void prepareImGui();
	void renderImGui();
	void prepareOpenGL();
	void renderOpenGL();
private:
	GLFWwindow* m_Window;
	RendererOpenGLData m_OpenGL;
	RendererImGuiData m_ImGui;
};