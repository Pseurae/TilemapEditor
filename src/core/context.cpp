#include "core/context.h"
#include "core/tilemap.h"
#include "core/tileset.h"
#include "helpers/events.h"
#include "helpers/fs.h"
#include "helpers/texture.h"
#include "helpers/logger.h"
#include "imgui.h"
#include "imgui_internal.h"
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <ios>
#include <ostream>
#include <string>
#include <sys/types.h>
#include <vector>
#include "core/actions.h"

std::unique_ptr<Tilemap> Context::m_Tilemap = nullptr;
std::unique_ptr<Tileset> Context::m_Tileset = nullptr;
fs::path Context::m_TilemapPath;
fs::path Context::m_TilesetPath;
std::list<const char *> Context::m_PopupsToOpen;
Brush Context::m_Brush;
bool Context::m_ShowTilemapGrid;
bool Context::m_ShowTilesetGrid;
ActionList Context::m_UndoStack;
ActionList Context::m_RedoStack;
std::list<fs::path> Context::m_RecentPaths;
bool Context::m_DirtyFlag;
TilemapFormat Context::m_LoadedFormat;

bool Context::NewTilemap(int width, int height)
{
    m_Tilemap = std::make_unique<Tilemap>(width, height);
    return true;
}

bool Context::OpenTilemap(const fs::path &path, TilemapFormat format)
{
    Tilemap *temp = new Tilemap(path, format);

    if (!temp->IsLoaded())
        return false;

    bool hasLoadedTilemap = (m_Tilemap && m_Tilemap->IsLoaded());

    m_Tilemap = std::unique_ptr<Tilemap>(temp);
    m_TilemapPath = path;
    TS_LOG_INFO("Loaded tilemap from %s", path.c_str());

    auto iter = std::find_if(m_RecentPaths.begin(), m_RecentPaths.end(), [&](auto &item) {
        return item == path;
    });

    if (iter != m_RecentPaths.end()) {
        m_RecentPaths.erase(iter);
    }

    m_RecentPaths.push_front(path);

    while (m_RecentPaths.size() > 10)
        m_RecentPaths.pop_back();

    SerializeRecentPaths();

    if (!(hasLoadedTilemap && m_Tileset && m_Tileset->IsLoaded()))
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

    m_Tileset = std::unique_ptr<Tileset>(temp);
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

    if (!m_DirtyFlag)
        return;

    m_DirtyFlag = false;

    if (m_Tilemap->Save(m_TilemapPath))
        TS_LOG_INFO("Saved tilemap to %s", m_TilemapPath.c_str());
    else
        TS_LOG_INFO("Could not save tilemap to %s", m_TilemapPath.c_str());
}

void Context::SaveAsTilemap(const fs::path &path)
{
    if (!(m_Tilemap && m_Tilemap->IsLoaded()))
        return;

    if (!m_DirtyFlag)
        return;

    m_DirtyFlag = false;

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

void Context::Undo()
{
    std::shared_ptr<Action> action = m_UndoStack.front();
    action->undo();
    m_UndoStack.pop_front();
    m_RedoStack.push_front(action);
    m_DirtyFlag = true;
}

void Context::Redo()
{
    std::shared_ptr<Action> action = m_RedoStack.front();
    action->redo();
    m_RedoStack.pop_front();
    m_UndoStack.push_front(action);
    m_DirtyFlag = true;
}

void Context::LogAction(Action *action)
{
    std::shared_ptr<Action> ptr(action);
    m_UndoStack.push_front(ptr);
    m_RedoStack.clear();

    while (m_RedoStack.size() > 30)
        m_RedoStack.pop_back();
}

static const fs::path s_RecentPathsFile = "recentfiles.txt";

void Context::SerializeRecentPaths()
{
    std::ofstream stream(s_RecentPathsFile);
    stream << m_RecentPaths.size() << std::endl;
    for (auto &path : m_RecentPaths)
    {
        stream << path << std::endl;
    }
}

void Context::DeserializeRecentPaths()
{
    if (!fs::Exists(s_RecentPathsFile))
        return;

    std::string line;
    fs::path path;
    std::ifstream stream(s_RecentPathsFile);

    std::getline(stream, line);
    int num = std::stoi(line);

    m_RecentPaths.clear();

    for (int i = 0; i < num; i++)
    {
        stream >> path;
        if (fs::Exists(path))
            m_RecentPaths.push_back(path);
    }
}

MenuBar Context::m_MenuBar = {
    {"File", []() {
        if (ImGui::MenuItem("New", "Ctrl + N"))
            EventManager::publish<RequestNewTilemapWindow>();

        if (ImGui::MenuItem("Open", "Ctrl + O"))
            EventManager::publish<RequestOpenTilemapWindow>();

        if (ImGui::BeginMenu("Open Recent", !m_RecentPaths.empty()))
        {
            for (auto &path : m_RecentPaths)
            {
                if (ImGui::MenuItem(path.c_str()))
                    EventManager::publish<RequestOpenRecentTilemap>(path);
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Clear Recents"))
                m_RecentPaths.clear();
            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Save", "Ctrl + S"))
            EventManager::publish<RequestSaveTilemap>(Context::GetTilemapPath().empty());

        if (ImGui::MenuItem("Save As", "Ctrl + Shift + S"))
            EventManager::publish<RequestSaveTilemap>(true);

        if (ImGui::MenuItem("Quit", "Ctrl + Q"))
            EventManager::publish<RequestProgramQuit>(true);
    }},
    {"Edit", []() {
        if (ImGui::MenuItem("Undo", "Ctrl + Z", false, Context::CanUndo()))
            EventManager::publish<RequestUndo>();

        if (ImGui::MenuItem("Redo", "Ctrl + Y", false, Context::CanRedo()))
            EventManager::publish<RequestRedo>();
    }},
    {"View", []() {
        if (ImGui::BeginMenu("Grids"))
        {
            ImGui::MenuItem("Tilemap", NULL, &Context::ShouldShowTMGrid());
            ImGui::MenuItem("Tileset", NULL, &Context::ShouldShowTSGrid());
            ImGui::EndMenu();
        }
    }},
    {"Tileset", []() {
        if (ImGui::MenuItem("Import Tileset", "Ctrl + Shift + O"))
            EventManager::publish<RequestOpenTilesetWindow>();
    }}
};

