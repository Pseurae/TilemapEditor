#include "core/tileset.h"
#include "helpers/fs.h"
#include "helpers/logger.h"
#include "helpers/texture.h"
#include <stb_image.h>

Tileset::Tileset(const fs::path &path) : m_TexOwner(true)
{
    m_IsLoaded = false;

    int image_width = 0;
    int image_height = 0;
    uint8_t *data = stbi_load(path.c_str(), &image_width, &image_height, NULL, 4);

    if (image_width % 8 > 0 || image_height % 8 > 0)
    {
        TS_LOG_ERROR("Dimensions of image file '%s' are invalid.", path.c_str());
        return;
    }
    
    m_Texture = new Texture(data, image_width, image_height);
    m_Width = image_width / 8;
    m_Height = image_height / 8;
    m_IsLoaded = true;
    stbi_image_free(data);
}

Tileset::Tileset(Texture *texture) : m_TexOwner(false), m_IsLoaded(true)
{
    m_IsLoaded = false;
    if (texture->GetWidth() % 8 > 0 || texture->GetHeight() % 8 > 0)
    {
        TS_LOG_ERROR("Dimensions of provided texture file are invalid.");
        return;
    }

    m_Texture = texture;
    m_Width = texture->GetWidth() / 8;
    m_Height = texture->GetHeight() / 8;
    m_IsLoaded = true;
}

Tileset::~Tileset()
{
    if (m_TexOwner)
        delete m_Texture;
}
