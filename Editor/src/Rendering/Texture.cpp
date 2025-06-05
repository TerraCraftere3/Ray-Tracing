#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Platform/stb_image.h"
#include "Core/Log.h"

using namespace glm;

Texture::Texture(std::string path)
{
    m_Path = path;
	LoadTexture(path.c_str());
}

Texture::Texture(glm::vec3 color, const char* path)
{
    m_Path = path;
    LoadColor(color);
}

uint32_t Texture::GetWidth() const
{
    return m_Width;
}

uint32_t Texture::GetHeight() const
{
    return m_Height;
}

uint32_t Texture::GetID() const
{
    return m_TextureID;
}

bool Texture::IsValid() const
{
    return textureData;
}

std::string Texture::GetPath() const
{
    return m_Path;
}

glm::vec4 Texture::Sample(float u, float v) const
{
    return Sample(vec2(u, v));
}

glm::vec4 Texture::Sample(glm::vec2 uv) const
{
    if (!textureData) return glm::vec4(1.0f, 0.0f, 1.0f, 1.0f); // error magenta

    uv = glm::clamp(uv, glm::vec2(0.0f), glm::vec2(1.0f));
    int x = static_cast<int>(uv.x * (m_Width - 1));
    int y = static_cast<int>(uv.y * (m_Height - 1)); // No flip here; image was flipped on load

    int index = (y * m_Width + x) * m_Channels;

    float r = textureData[index + 0] / 255.0f;
    float g = m_Channels > 1 ? textureData[index + 1] / 255.0f : r;
    float b = m_Channels > 2 ? textureData[index + 2] / 255.0f : r;
    float a = m_Channels > 3 ? textureData[index + 3] / 255.0f : 1.0f;

    return glm::vec4(r, g, b, a);
}

bool Texture::LoadTexture(const char* filename)
{
    textureData = stbi_load(filename, &m_Width, &m_Height, &m_Channels, 0);
    if (!textureData && !m_TextureNotFound) {
        LOG_ERROR("Failed to load texture: {}", filename);
        m_TextureNotFound = true;
		LoadColor(ErrorColorRGB); // Load purple
        return false;
    }

    return UploadTexture();
}

bool Texture::LoadColor(glm::vec3 color)
{
    m_Width = 1;
    m_Height = 1;
    m_Channels = 4; // RGBA
    m_TextureID = 0;
    textureData = new unsigned char[4];
    textureData[0] = static_cast<unsigned char>(color.r * 255);
    textureData[1] = static_cast<unsigned char>(color.g * 255);
    textureData[2] = static_cast<unsigned char>(color.b * 255);
    textureData[3] = 255;

    return UploadTexture();
}

bool Texture::UploadTexture()
{
    GLenum format;
    switch (m_Channels) {
    case 1: format = GL_RED; break;
    case 3: format = GL_RGB; break;
    case 4: format = GL_RGBA; break;
    default: format = GL_RGB; break;
    }

    glGenTextures(1, &m_TextureID);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);

    glTexImage2D(GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, textureData);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Set texture parameters (basic)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind

    LOG_INFO("Loaded texture: {} ({}x{}, {} channels) -> OpenGL ID: {}", m_Path, m_Width, m_Height, m_Channels, m_TextureID);
    return true;
}
