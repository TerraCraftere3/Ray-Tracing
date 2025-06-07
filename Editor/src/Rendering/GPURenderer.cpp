#include "GPURenderer.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <Core/Log.h>
#include <glm/gtc/type_ptr.hpp>

namespace Utils {
    void CheckCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				LOG_ERROR("Shader Compile Error ({0}):\n{1}", type, infoLog);
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				LOG_ERROR("Program Link Error:\n{0}", infoLog);
            }
        }
    }

    bool replace(std::string& str, const std::string& from, const std::string& to) {
        size_t start_pos = str.find(from);
        if (start_pos == std::string::npos)
            return false;
        str.replace(start_pos, from.length(), to);
        return true;
    }
}

GLuint UploadMaterials(const std::vector<GPU_Material>& materials) {
    GLuint ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, materials.size() * sizeof(GPU_Material), materials.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo); // binding = 1 in GLSL
    return ssbo;
}

// Upload spheres
GLuint UploadSpheres(const std::vector<GPU_Sphere>& spheres) {
    GLuint ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, spheres.size() * sizeof(GPU_Sphere), spheres.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo); // binding = 0 in GLSL
    return ssbo;
}

// Set camera uniforms
void SetCameraUniforms(GLuint shaderProgram, const glm::vec3& position, const glm::vec3& forward, const glm::vec3& right, const glm::vec3& up, float fovDegrees, float aspectRatio, int frameIndex) {
    glUseProgram(shaderProgram);
    glUniform3fv(glGetUniformLocation(shaderProgram, "cameraPosition"), 1, &position[0]);
    glUniform3fv(glGetUniformLocation(shaderProgram, "cameraForward"), 1, &forward[0]);
    glUniform3fv(glGetUniformLocation(shaderProgram, "cameraRight"), 1, &right[0]);
    glUniform3fv(glGetUniformLocation(shaderProgram, "cameraUp"), 1, &up[0]);
    glUniform1f(glGetUniformLocation(shaderProgram, "cameraFov"), fovDegrees);
    glUniform1f(glGetUniformLocation(shaderProgram, "aspectRatio"), aspectRatio);
}

GPURenderer::GPURenderer()
{
    InitQuad();
    InitShader();

    std::vector<GPU_Material> materials = {
        { glm::vec3(0.8, 0.3, 0.3), glm::vec3(0.0), 0.2f },
        { glm::vec3(0.9), glm::vec3(5.0), 0.0f }
    };

    std::vector<GPU_Sphere> spheres = {
        { glm::vec3(0, 0, -5), 1.0f, 0 },
        { glm::vec3(0, -1001, -5), 1000.0f, 0 },
        { glm::vec3(0, 1, -5), 0.5f, 1 }
    };

    GLuint matSSBO = UploadMaterials(materials);
    GLuint sphereSSBO = UploadSpheres(spheres);
}

GPURenderer::~GPURenderer()
{
    glDeleteVertexArrays(1, &m_QuadVAO);
    glDeleteBuffers(1, &m_QuadVBO);
    glDeleteProgram(m_ShaderProgram);
    glDeleteTextures(1, &m_Texture);
    glDeleteFramebuffers(1, &m_Framebuffer);
}

void GPURenderer::Render(const Camera& camera, const Scene& scene)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
    glUseProgram(m_ShaderProgram);

    glUniform1i(glGetUniformLocation(m_ShaderProgram, "frameIndex"), m_FrameIndex);
	glUniform2i(glGetUniformLocation(m_ShaderProgram, "resolution"), m_Width, m_Height);
	glUniform1i(glGetUniformLocation(m_ShaderProgram, "sphereCount"), scene.Spheres.size());
	glUniform1i(glGetUniformLocation(m_ShaderProgram, "materialCount"), scene.Materials.size());
	glUniformMatrix4fv(glGetUniformLocation(m_ShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(camera.GetProjection()));
    glUniformMatrix4fv(glGetUniformLocation(m_ShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(camera.GetView()));
    glUniformMatrix4fv(glGetUniformLocation(m_ShaderProgram, "projectionInverse"), 1, GL_FALSE, glm::value_ptr(camera.GetInverseProjection()));
    glUniformMatrix4fv(glGetUniformLocation(m_ShaderProgram, "viewInverse"), 1, GL_FALSE, glm::value_ptr(camera.GetInverseView()));

    glClearColor(0.7f, 0.3f, 0.2f, 1.0f); // use something visible
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(m_QuadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GPURenderer::OnResize(uint32_t width, uint32_t height)
{
	m_Width = width;
	m_Height = height;
	if (m_Framebuffer) {
		glDeleteFramebuffers(1, &m_Framebuffer);
		glDeleteTextures(1, &m_Texture);
	}
	glGenFramebuffers(1, &m_Framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
	glGenTextures(1, &m_Texture);
	glBindTexture(GL_TEXTURE_2D, m_Texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Texture, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		LOG_CRITICAL("Framebuffer is not complete! Check your OpenGL setup.");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int GPURenderer::GetFinalImageID() const
{
    return m_Texture;
}

bool GPURenderer::isValidImage() const
{
	return m_Texture != 0;
}

void GPURenderer::InitQuad()
{
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
    };

    glGenVertexArrays(1, &m_QuadVAO);
    glGenBuffers(1, &m_QuadVBO);
    glBindVertexArray(m_QuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1); // texcoord
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

std::string GPURenderer::LoadShaderWithIncludes(const std::string& filePath, const std::string& from)
{
    std::ifstream file(filePath);
    if (!file) {
		LOG_ERROR("Failed to open shader file: {}", filePath);
        return "";
    }

    std::stringstream source;
    std::string line;
    while (std::getline(file, line)) {
        if (line.starts_with("#include")) {
            std::string includePath = line.substr(line.find_first_of("\"") + 1);
            includePath = includePath.substr(0, includePath.find_last_of("\""));

            std::string fullPath = (std::filesystem::path(filePath).parent_path() / includePath).string();
			Utils::replace(fullPath, "\\", "/"); // Normalize path for Windows
            source << LoadShaderWithIncludes(fullPath, filePath);
        }
        else {
            source << line << '\n';
        }
    }

    if(from == "")
	    LOG_INFO("Loaded shader: {}", filePath);
    else
        LOG_INFO("Loaded shader: {} -> {}", from, filePath);

    return source.str();
}

void GPURenderer::InitShader()
{
    std::string fragSource = LoadShaderWithIncludes("shader/main.glsl");

    const char* vertexSource = R"(
        #version 450 core
        layout(location = 0) in vec2 aPos;
        layout(location = 1) in vec2 aTexCoord;
        out vec2 vUV;
        void main() {
            vUV = aTexCoord;
            gl_Position = vec4(aPos, 0.0, 1.0);
        }
    )";

    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexSource, nullptr);
    glCompileShader(vertex);
    Utils::CheckCompileErrors(vertex, "VERTEX");

    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fSrc = fragSource.c_str();
    glShaderSource(fragment, 1, &fSrc, nullptr);
    glCompileShader(fragment);
    Utils::CheckCompileErrors(fragment, "FRAGMENT");

    m_ShaderProgram = glCreateProgram();
    glAttachShader(m_ShaderProgram, vertex);
    glAttachShader(m_ShaderProgram, fragment);
    glLinkProgram(m_ShaderProgram);
    Utils::CheckCompileErrors(m_ShaderProgram, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

