#include "core/context.h"
#include "core/tilemap.h"
#include "core/tileset.h"
#include "helpers/events.h"
#include "helpers/fs.h"
#include "helpers/texture.h"
#include "helpers/logger.h"
#include "imgui.h"
#include "imgui_internal.h"
#include <cstdio>
#include <cstring>
#include <sys/types.h>

Tilemap *Context::m_Tilemap = nullptr;
Tileset *Context::m_Tileset = nullptr;
fs::path Context::m_TilemapPath;
fs::path Context::m_TilesetPath;
std::list<const char *> Context::m_PopupsToOpen;
Brush Context::m_Brush;

bool Context::OpenTilemap(const fs::path &path, TilemapFormat format)
{
    Tilemap *temp = new Tilemap(path, format);

    if (!temp->IsLoaded())
        return false;

    if (m_Tilemap)
        delete m_Tilemap;

    m_Tilemap = temp;
    m_TilemapPath = path;
    TS_LOG_INFO("Loaded tilemap from %s", path.c_str());

    if (!(m_Tileset && m_Tileset->IsLoaded()))
    {
        fs::path ts_path = path;
        ts_path.replace_extension(".png");
        if (fs::Exists(ts_path))
        {
            OpenTileset(ts_path);
        }
    }

    return true;
}

bool Context::OpenTileset(const fs::path &path)
{
    Tileset *temp = new Tileset(path);

    if (!temp->IsLoaded())
        return false;

    if (m_Tileset)
        delete m_Tileset;

    m_Tileset = temp;
    m_TilesetPath = path;
    TS_LOG_INFO("Loaded tileset from %s", path.c_str());
    return true;
}

void Context::SaveTilemap()
{
    if (m_TilemapPath.empty())
        return;

    if (!(m_Tilemap && m_Tilemap->IsLoaded()))
        return;

    if (m_Tilemap->Save(m_TilemapPath))
        TS_LOG_INFO("Saved tilemap to %s", m_TilemapPath.c_str());
    else
        TS_LOG_INFO("Could not save tilemap to %s", m_TilemapPath.c_str());
}

void Context::SaveAsTilemap(const fs::path &path)
{
    if (!(m_Tilemap && m_Tilemap->IsLoaded()))
        return;

    if (m_Tilemap->Save(path))
    {
        m_TilemapPath = path;
        TS_LOG_INFO("Saved tilemap to %s", m_TilemapPath.c_str());
    }
    else 
    {
        TS_LOG_INFO("Could not save tilemap to %s", path.c_str());
    }
}

void Context::ProcessPopups()
{
    m_PopupsToOpen.remove_if([](const char *popup) {
        if (!ImGui::IsPopupOpen(popup))
            ImGui::OpenPopup(popup);
        return true;
    });
}

MenuBar Context::m_MenuBar = {
    {"File", []() {
        if (ImGui::MenuItem("Open", "Ctrl + O"))
            EventManager::publish<RequestOpenTilemapWindow>();
        if (ImGui::MenuItem("Save", "Ctrl + S"))
            EventManager::publish<RequestSaveTilemap>(false);
        if (ImGui::MenuItem("Save As", "Ctrl + Shift + S"))
            EventManager::publish<RequestSaveTilemap>(true);
        if (ImGui::MenuItem("Quit", "Ctrl + Q"))
            EventManager::publish<RequestProgramQuit>(true);
    }},
    {"Edit", []() {
        if (ImGui::MenuItem("Undo", "Ctrl + Z"))
        {}
        if (ImGui::MenuItem("Redo", "Ctrl + Shift + Z"))
        {}
    }},
    {"Tileset", []() {
        if (ImGui::MenuItem("Import Tileset", "Ctrl + Shift + O"))
            EventManager::publish<RequestOpenTilesetWindow>();
    }}
};

