#include "core/tilemap.h"
#include "helpers/fs.h"
#include "helpers/logger.h"
#include <cstdio>
#include <fstream>

enum Mask : short
{
    Index = 0x3FF,
    FlipX = 0x400,
    FlipY = 0x800
};

Tilemap::Tilemap(int width, int height) : m_Width(width), m_Height(height)
{
    m_Tiles.resize(m_Width * m_Height, { 0, 0, false, false });
}

Tilemap::Tilemap(const fs::path &path, TilemapFormat format)
{
    uintmax_t size = fs::GetFileSize(path) / 2;
    if (size == 0)
    {
        TS_LOG_ERROR("Invalid size for tilemap '%s'", path.c_str());
        return;
    }

    m_Tiles.resize(size);
    std::ifstream fs(path, std::ios::binary);
    
    uint16_t entry;
    uint8_t palette;

    for (int i = 0; i < size; i++)
    {
        fs.read(reinterpret_cast<char *>(&entry), 2);
        if (format == TilemapFormat::BPP8)
            palette = (entry >> 12) & 0xF;
        else
            palette = 0;

        m_Tiles[i] = {
            static_cast<uint16_t>(entry & Mask::Index),
            palette,
            (entry & Mask::FlipX) == Mask::FlipX,
            (entry & Mask::FlipY) == Mask::FlipY
        }; 
    }
    m_IsLoaded = true;
}

bool Tilemap::Save(const fs::path &path)
{
    if (!m_IsLoaded)
        return false;

    std::ofstream fs(path, std::ios::binary | std::ios::trunc);

    for (TileEntry &entry : m_Tiles)
    {
        uint16_t data = (
            entry.index & Mask::Index |
            (entry.xflip ? Mask::FlipX : 0) |
            (entry.yflip ? Mask::FlipY : 0)
        );
        fs.write(reinterpret_cast<char *>(&data), 2);
    }
    return true;
}
