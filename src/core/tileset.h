#pragma once

#include "helpers/fs.h"

class Texture;

class Tileset
{
public:
    static constexpr float TileSize[] = { 8.0f, 8.0f };

    Tileset(const fs::path &path);
    Tileset(Texture *texture);
    ~Tileset();
    Texture *GetTexture() { return m_Texture; }
    int GetWidth() { return m_Width; }
    int GetHeight() { return m_Height; }
    bool IsLoaded() { return m_IsLoaded; }
private:
    bool m_TexOwner;
    bool m_IsLoaded;
    Texture *m_Texture;
    int m_Width;
    int m_Height;
};
