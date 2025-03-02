#include "Renderer.h"
#include <Core.h>
#include <format>
#include <iostream>

bool Renderer::m_ImGuiEnabled = true;

Renderer::Renderer(GLFWwindow* window)
{
	m_Window = window;
}

void Renderer::prepareImGui()
{
	LOG_INFO("Finished preparing ImGui");
}

void renderImGuiTableRow(const char* label, const char* value) {
	ImGui::TableNextRow();
	ImGui::TableNextColumn();
	ImGui::Text(label);
	ImGui::TableNextColumn();
	ImGui::Text(value);
}

void Renderer::renderImGui()
{
	if (m_ImGuiEnabled) {
		ImGui::Begin("Debug");
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Shader: ");
		ImGui::InputText("Fragment Shader Path", &m_ShaderFragmentPath);
		ImGui::InputText("Vertex Shader Path", &m_ShaderVertexPath);
		if (ImGui::Button("Recompile Shader")) {
			Shader newShader(m_ShaderVertexPath.c_str(), m_ShaderFragmentPath.c_str());
			m_OpenGL.shader = &newShader;
			m_OpenGL.shader->use();
			Utils::initTime();
		}
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "System Info: ");
		ImGui::BeginTable("System Info", 2);
		{
			int xsize, ysize;
			glfwGetFramebufferSize(m_Window, &xsize, &ysize);
			std::string scaleStr = std::to_string(xsize) + ", " + std::to_string(ysize);
			renderImGuiTableRow("Window Size", scaleStr.c_str());

			int xpos, ypos;
			glfwGetWindowPos(m_Window, &xpos, &ypos);
			std::string posStr = std::to_string(xpos) + ", " + std::to_string(ypos);
			renderImGuiTableRow("Window Pos", posStr.c_str());

			renderImGuiTableRow("Time", std::to_string(Utils::getTime()).c_str());
			renderImGuiTableRow("Frame", std::to_string(Utils::getFrame()).c_str());
		}

		ImGui::EndTable();
		ImGui::End();
	}
}

void Renderer::prepareOpenGL()
{
	float vertices[6*4] = {
	 1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 0.0f, // top right
	 1.0f, -1.0f, 0.0f, 1.0f,-1.0f, 0.0f, // bottom right
	-1.0f, -1.0f, 0.0f,-1.0f,-1.0f, 0.0f, // bottom left
	-1.0f,  1.0f, 0.0f,-1.0f, 1.0f, 0.0f  // top left 
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

	m_ShaderFragmentPath = (char*)"shader.frag";
	m_ShaderVertexPath = (char*)"shader.vert";
	Shader shader(m_ShaderVertexPath.c_str(), m_ShaderFragmentPath.c_str());

	m_OpenGL.setVertices(vertices);
	m_OpenGL.setVBO(VBO);
	m_OpenGL.setVAO(VAO);
	m_OpenGL.setEBO(EBO);
	m_OpenGL.setShaderProgram(shader);
	m_OpenGL.shader->use();
	m_Shader = shader;
	LOG_INFO("Finished preparing OpenGL");
}

void Renderer::renderOpenGL()
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	int xsize, ysize;
	glfwGetFramebufferSize(m_Window, &xsize, &ysize);
	m_Shader.setFloat2("u_resolution", glm::vec2(xsize, ysize));
	m_Shader.setFloat("u_aspectRatio", xsize / ysize);
	auto time = Utils::getTime();
	m_Shader.setFloat("u_time", time);
	auto frame = Utils::getFrame();
	m_Shader.setFloat("u_frame", frame);

	glBindVertexArray(m_OpenGL.VAO); 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_OpenGL.EBO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Renderer::toggleImGui()
{
	m_ImGuiEnabled = !m_ImGuiEnabled;
}
