#pragma once

#include "helpers/fs.h"
#include <stdint.h>
#include <vector>

enum TilemapFormat : char
{
    BPP4,
    BPP8
};

struct TileEntry
{
    uint16_t index;
    uint8_t palette;
    bool xflip, yflip;
};

class Tilemap
{
public:
    Tilemap(int width, int height);
    Tilemap(const fs::path &path, TilemapFormat format);

    bool Save(const fs::path &path);

    int GetWidth() { return m_Width; }
    int GetHeight() { return m_Height; }
    bool IsLoaded() { return m_IsLoaded; }

    std::vector<TileEntry> &GetTiles() { return m_Tiles; }
private:
    bool m_IsLoaded;
    int m_Width;
    int m_Height;
    std::vector<TileEntry> m_Tiles; 
};

