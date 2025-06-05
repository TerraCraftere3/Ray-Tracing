#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Platform/stb_image.h"
#include "Core/Log.h"

using namespace glm;

Texture::Texture(std::string path)
{
	LoadTexture(path.c_str());
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
    return 0;
}

bool Texture::IsValid() const
{
    return textureData;
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
    //stbi_set_flip_vertically_on_load(true);

    textureData = stbi_load(filename, &m_Width, &m_Height, &m_Channels, 0);
    if (!textureData) {
        LOG_ERROR("Failed to load texture: {}", filename);
        return false;
    }

    LOG_INFO("Loaded texture: {} ({}x{}, {} channels)", filename, m_Width, m_Height, m_Channels);
    return true;
}
