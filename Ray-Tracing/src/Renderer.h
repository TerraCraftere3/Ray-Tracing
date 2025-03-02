#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_stdlib.h"
#include <Shader.h>

struct RendererOpenGLData {
	RendererOpenGLData()
		: VBO(0), VAO(0), EBO(0), shader(nullptr) {
		// Initialize vertices to zero
		for (int i = 0; i < 6 * 4; i++) {
			vertices[i] = 0.0f;
		}
	}


	float vertices[6*4];
	void setVertices(const float newVertices[6*4]) {
		for (int i = 0; i < 6*4; i++) {
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

	Shader* shader;
	void setShaderProgram(Shader newShaderProgram) {
		shader = &newShaderProgram;
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
	static void toggleImGui();
private:
	GLFWwindow* m_Window;
	RendererOpenGLData m_OpenGL;
	RendererImGuiData m_ImGui;
	std::string m_ShaderFragmentPath;
	std::string m_ShaderVertexPath;
	Shader m_Shader;
	static bool m_ImGuiEnabled;
};