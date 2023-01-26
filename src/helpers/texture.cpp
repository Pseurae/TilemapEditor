#include "texture.h"
#include "helpers/fs.h"
#include "helpers/logger.h"
#include <stb_image.h>

#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>

Texture::Texture(const fs::path &path)
{
    int image_width = 0;
    int image_height = 0;
    uint8_t *data = stbi_load(path.c_str(), &image_width, &image_height, NULL, 4);

    if (data)
    {
        // Create a OpenGL texture identifier
        GLuint image_texture;
        glGenTextures(1, &image_texture);
        glBindTexture(GL_TEXTURE_2D, image_texture);

        // Setup filtering parameters for display
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

        // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        this->m_TextureID = image_texture;
        this->m_Width = image_width;
        this->m_Height = image_height;
        this->m_IsLoaded = true;

        stbi_image_free(data);
    }
    else 
    {
        TS_LOG_ERROR("Could not load texture from: '%s'", path.c_str());
    }
}

Texture::Texture(const uint8_t *data, int image_width, int image_height) : m_Width(image_width), m_Height(image_height)
{
    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    this->m_TextureID = image_texture;
    this->m_IsLoaded = true;
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_TextureID);
}
