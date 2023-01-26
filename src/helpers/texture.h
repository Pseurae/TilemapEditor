#pragma once

#include "fs.h"

#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>

class Texture
{
public:
    Texture(const fs::path &path);
    Texture(const uint8_t *data, int image_width, int image_height);
    ~Texture();

    GLuint GetTextureID() { return m_TextureID; }
    int GetWidth() { return m_Width; }
    int GetHeight() { return m_Height; }
    bool IsLoaded() { return m_IsLoaded; }
private:
    bool m_IsLoaded;
    GLuint m_TextureID;
    int m_Width, m_Height;
};
