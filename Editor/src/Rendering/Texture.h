#pragma once
#include <cstdint>
#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define ErrorColorRGB glm::vec3(1.0f, 0.0f, 1.0f)
#define ErrorColorRGBA glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)

class Texture {
public:
	Texture(std::string path);
	Texture(glm::vec3 color, const char* path); // Path can be any char string
	Texture(const char* path) : Texture(std::string(path)) {}
	uint32_t GetWidth() const;
	uint32_t GetHeight() const;
	uint32_t GetID() const;
	bool IsValid() const;
	std::string GetPath() const;

	glm::vec4 Sample(float u, float v) const;
	glm::vec4 Sample(glm::vec2 uv) const;
private:
	bool LoadTexture(const char* filename);
	bool LoadColor(glm::vec3 color);
	bool UploadTexture();
	int m_Width, m_Height, m_Channels;
	bool m_TextureNotFound = false;
	unsigned char* textureData = nullptr;
	std::string m_Path;
	uint32_t m_TextureID = 0;
};