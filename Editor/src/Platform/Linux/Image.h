#pragma once

#include <cstdint>
#include <vector>
#include <cstring>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Image
{
public:
    Image(uint32_t width, uint32_t height) {
        Create(width, height);
    }

    ~Image()
    {
        if (m_TextureID != 0)
            glDeleteTextures(1, &m_TextureID);
    }

    void Create(uint32_t width, uint32_t height)
    {
        m_Width = width;
        m_Height = height;
        m_Data.resize(width * height * 4); // RGBA8

        if (m_TextureID == 0)
            glGenTextures(1, &m_TextureID);

        glBindTexture(GL_TEXTURE_2D, m_TextureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        glBindTexture(GL_TEXTURE_2D, 0);

        Clear();
    }

    void Clear()
    {
        std::memset(m_Data.data(), 0, m_Data.size());
    }

    uint32_t GetWidth() const { return m_Width; }
    uint32_t GetHeight() const { return m_Height; }

    uint8_t* GetData() { return m_Data.data(); }
    const uint8_t* GetData() const { return m_Data.data(); }

    size_t GetSize() const { return m_Data.size(); }

    GLuint GetTextureID() const { return m_TextureID; }

    void SetData(const uint32_t* data)
    {
        if (!data || m_Width == 0 || m_Height == 0)
            return;

        size_t byteSize = (size_t)m_Width * m_Height * 4;
        if (m_Data.size() != byteSize)
            m_Data.resize(byteSize);

        for (uint32_t y = 0; y < m_Height; ++y) {
            const uint8_t* srcRow = reinterpret_cast<const uint8_t*>(data + (m_Height - 1 - y) * m_Width);
            uint8_t* dstRow = m_Data.data() + y * m_Width * 4;
            std::memcpy(dstRow, srcRow, m_Width * 4);
        }

        Upload();
    }

private:
    void Upload()
    {
        if (m_TextureID == 0 || m_Data.empty())
            return;

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glBindTexture(GL_TEXTURE_2D, m_TextureID);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, GL_RGBA, GL_UNSIGNED_BYTE, m_Data.data());
        glBindTexture(GL_TEXTURE_2D, 0);
    }

private:
    uint32_t m_Width = 0;
    uint32_t m_Height = 0;
    std::vector<uint8_t> m_Data;
    GLuint m_TextureID = 0;
};
