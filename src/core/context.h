#pragma once

#include <string>
#include <list>
#include <util.h>
#include <functional>
#include "core/tilemap.h"
#include "helpers/fs.h"

class Tilemap;
class Tileset;
class Texture;

struct Brush
{
    int selected_tile;
    bool xflip, yflip;
};

using MenuBar = std::list<std::pair<std::string, std::function<void()>>>;

class Context
{
public:
    static bool OpenTilemap(const fs::path &path, TilemapFormat format);
    static bool OpenTileset(const fs::path &path);

    static void SaveTilemap();
    static void SaveAsTilemap(const fs::path &path);

    static Tilemap *GetTilemap() { return m_Tilemap; }
    static Tileset *GetTileset() { return m_Tileset; }
    static Brush &GetBrush() { return m_Brush; }

    const static fs::path &GetTilemapPath() { return m_TilemapPath; }
    const static fs::path &GetTilesetPath() { return m_TilesetPath; }

    const static MenuBar &GetMenuBar() { return m_MenuBar; }
    static void ProcessPopups();
    static void AppendPopup(const char *path) { m_PopupsToOpen.push_back(path); }

private:
    static Tilemap *m_Tilemap;
    static Tileset *m_Tileset;
    static fs::path m_TilesetPath;
    static fs::path m_TilemapPath;

    static std::list<const char *> m_PopupsToOpen;

    static Brush m_Brush;
    static MenuBar m_MenuBar;
};

