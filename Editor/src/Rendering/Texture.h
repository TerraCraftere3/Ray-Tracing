#pragma once
#include <cstdint>
#include <string>
#include <glm/glm.hpp>

class Texture {
public:
	Texture(std::string path);
	Texture(const char* path) : Texture(std::string(path)) {}
	uint32_t GetWidth() const;
	uint32_t GetHeight() const;
	uint32_t GetID() const;
	bool IsValid() const;

	glm::vec4 Sample(float u, float v) const;
	glm::vec4 Sample(glm::vec2 uv) const;
private:
	bool LoadTexture(const char* filename);
	int m_Width, m_Height, m_Channels;
	unsigned char* textureData = nullptr;
};