#pragma once

#include <memory>
#include <string>
#include <list>
#include <functional>

#include "core/tilemap.h"
#include "core/tileset.h"

#include "helpers/fs.h"
#include "helpers/texture.h"

enum TilemapFormat : char;
struct Action;

struct Brush
{
    int selected_tile, palettenum;
    bool xflip, yflip;
};

using MenuBar = std::list<std::pair<std::string, std::function<void()>>>;
using ActionList = std::list<std::shared_ptr<Action>>;

class Context
{
public:
    static bool NewTilemap(int width, int height);
    static bool OpenTilemap(const fs::path &path, TilemapFormat format);
    static bool OpenTileset(const fs::path &path);

    static void SaveTilemap();
    static void SaveAsTilemap(const fs::path &path);

    static Tilemap *GetTilemap() { return m_Tilemap.get(); }
    static Tileset *GetTileset() { return m_Tileset.get(); }
    static Brush &GetBrush() { return m_Brush; }

    const static fs::path &GetTilemapPath() { return m_TilemapPath; }
    const static fs::path &GetTilesetPath() { return m_TilesetPath; }

    const static MenuBar &GetMenuBar() { return m_MenuBar; }
    static void ProcessPopups();
    static void AppendPopup(const char *path) { m_PopupsToOpen.push_back(path); }

    static bool &ShouldShowTMGrid() { return m_ShowTilemapGrid; }
    static bool &ShouldShowTSGrid() { return m_ShowTilesetGrid; }

    static void Undo();
    static void Redo();

    static bool CanUndo() { return !m_UndoStack.empty(); } 
    static bool CanRedo() { return !m_RedoStack.empty(); }

    static void LogAction(Action *action);
    static void SerializeRecentPaths();
    static void DeserializeRecentPaths();

    static bool &IsDirty() { return m_DirtyFlag; }
    static TilemapFormat &GetFormat() { return m_LoadedFormat; }

private:
    static std::unique_ptr<Tilemap> m_Tilemap;
    static std::unique_ptr<Tileset> m_Tileset;
    static fs::path m_TilesetPath;
    static fs::path m_TilemapPath;

    static std::list<const char *> m_PopupsToOpen;

    static TilemapFormat m_LoadedFormat;
    static Brush m_Brush;
    static MenuBar m_MenuBar;

    static bool m_ShowTilemapGrid;
    static bool m_ShowTilesetGrid;

    static ActionList m_UndoStack;
    static ActionList m_RedoStack;

    static std::list<fs::path> m_RecentPaths;
    static bool m_DirtyFlag;
};

