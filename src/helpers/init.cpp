#include "helpers/init.h"
#include "helpers/events.h"
#include "helpers/fs.h"
#include "helpers/hotkeys.h"
#include "core/context.h"
#include "core/tilemap.h"

#include <GLFW/glfw3.h>
#include <imgui.h>

static const fs::DialogFilter s_TilemapFilters = { { "Tilemaps", "bin" } };
static const fs::DialogFilter s_TilesetFilters = { { "Tilesets", "png" } };
static fs::path s_CachedPath;

void OpenFile()
{
    fs::OpenFileDialog(fs::DialogMode::Open, s_TilemapFilters, [](fs::path path) {
        EventManager::publish<RequestOpenTilemap>(path);
    });
}

void RegisterEvents()
{
    EventManager::subscribe<RequestOpenTilemap>([](const fs::path &path) {
        s_CachedPath = path;
        EventManager::publish<RequestOpenPopup>("OpenTilemapOptions");
    });

    EventManager::subscribe<RequestOpenTilemapWindow>([]() {
        if (Context::GetTilemap() && Context::GetTilemap()->IsLoaded())
            EventManager::publish<RequestOpenPopup>("UnsavedChanges");
        else
            OpenFile();
    });

    EventManager::subscribe<RequestOpenTileset>([](const fs::path &path) {
        Context::OpenTileset(path);
    });

    EventManager::subscribe<RequestOpenTilesetWindow>([]() {
        fs::OpenFileDialog(fs::DialogMode::Open, s_TilesetFilters, [](fs::path path) {
            Context::OpenTileset(path);
        });
    });

    EventManager::subscribe<RequestSaveTilemap>([](bool set_path) {
        if (set_path)
            fs::OpenFileDialog(fs::DialogMode::Save, s_TilemapFilters, [](fs::path path) {
                Context::SaveAsTilemap(path);
            });
        else 
            Context::SaveTilemap();
    });

    EventManager::subscribe<RequestOpenPopup>([](const char *name) {
        Context::AppendPopup(name);
    });
}

void RegisterHotKeys()
{
    HotKeyManager::AddHotKey(GLFW_MOD_CONTROL, GLFW_KEY_O, []() {
        EventManager::publish<RequestOpenTilemapWindow>();
    });

    HotKeyManager::AddHotKey(GLFW_MOD_CONTROL, GLFW_KEY_S, []() {
        EventManager::publish<RequestSaveTilemap>(false);
    });

    HotKeyManager::AddHotKey(GLFW_MOD_CONTROL | GLFW_MOD_SHIFT, GLFW_KEY_S, []() {
        EventManager::publish<RequestSaveTilemap>(true);
    });

    HotKeyManager::AddHotKey(GLFW_MOD_CONTROL, GLFW_KEY_Q, []() {
        EventManager::publish<RequestProgramQuit>(true);
    });
}

static constexpr ImGuiWindowFlags s_PopupFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize;

void DrawPopups()
{
    if (ImGui::BeginPopupModal("UnsavedChanges", NULL, s_PopupFlags))
    {
        ImGui::Text("This file has unsaved changes!");
        ImGui::Text("Do you want to continue?");

        if (ImGui::Button("Yes"))
        {
            ImGui::CloseCurrentPopup();
            OpenFile();
        }

        ImGui::SameLine();
        if (ImGui::Button("No"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal("QuitMessage", NULL, s_PopupFlags))
    {
        ImGui::Text("Do you want to quit?");
        if (ImGui::Button("Yes"))
        {
            EventManager::publish<RequestProgramQuit>(false);
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if (ImGui::Button("No"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal("OpenTilemapOptions", NULL, s_PopupFlags))
    {
        const char *items[] = { "4bpp", "8bpp" };
        static int current_item = 0;

        if (ImGui::BeginCombo("Tilemap Type", items[current_item]))
        {
            for (int i = 0; i < IM_ARRAYSIZE(items); i++)
            {
                bool is_selected = current_item == i;

                if (ImGui::Selectable(items[i], is_selected))
                    current_item = i;

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (ImGui::Button("Ok"))
        {
            Context::OpenTilemap(s_CachedPath, static_cast<TilemapFormat>(current_item));
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}
