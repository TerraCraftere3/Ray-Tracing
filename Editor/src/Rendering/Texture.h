#pragma once
#include <cstdint>
#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Texture {
public:
	Texture(std::string path);
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
	int m_Width, m_Height, m_Channels;
	unsigned char* textureData = nullptr;
	std::string m_Path;
	uint32_t m_TextureID = 0;
};