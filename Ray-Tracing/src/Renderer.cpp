#include "Renderer.h"
#include <Core.h>

Renderer::Renderer(GLFWwindow* window)
{
	m_Window = window;
}

void Renderer::prepareImGui()
{
	LOG_INFO("Finished preparing ImGui");
}

void Renderer::renderImGui()
{
	ImGui::Begin("Debug");
	ImGui::Text("Test");
	ImGui::End();
}

void Renderer::prepareOpenGL()
{
	float vertices[6*4] = {
	 1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 0.0f, // top right
	 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom left
	-1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f  // top left 
	};

	unsigned int indices[] = {
	0, 1, 3,
	1, 2, 3
	};

	unsigned int EBO;
	glGenBuffers(1, &EBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);


	Shader shader("shader.vert", "shader.frag");

	m_OpenGL.setVertices(vertices);
	m_OpenGL.setVBO(VBO);
	m_OpenGL.setVAO(VAO);
	m_OpenGL.setEBO(EBO);
	m_OpenGL.setShaderProgram(shader);
	m_OpenGL.shader->use();
	LOG_INFO("Finished preparing OpenGL");
}

void Renderer::renderOpenGL()
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindVertexArray(m_OpenGL.VAO); 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_OpenGL.EBO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
