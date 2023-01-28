#include "panes/tilemap.h"
#include "core/actions.h"
#include "core/context.h"
#include "core/tilemap.h"
#include "core/tileset.h"
#include "helpers/fs.h"
#include "helpers/texture.h"
#include "helpers/logger.h"

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include <vector>

template<class T>
static inline void swap_val(T *v1, T *v2)
{
    T temp = *v1;
    *v1 = *v2;
    *v2 = temp;
}

void TilemapPane::Draw()
{
    ImGui::SameLine();

    ImGui::BeginGroup();
    ImGui::BeginChild("Tilemap", ImVec2(0.0f, 0.0f), true);

    Tilemap *tilemap = Context::GetTilemap();
    Tileset *tileset = Context::GetTileset();

    ImGui::Spacing();

    if (tilemap && tilemap->IsLoaded())
    {
        if (tileset && tileset->IsLoaded())
        {
            ImGui::BeginChild("###TilemapView", ImVec2(0.0f, -ImGui::GetFrameHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);
            this->TilemapEdit(tilemap, tileset, Context::ShouldShowTMGrid());
            ImGui::EndChild();

            const float button_size = ImGui::GetFrameHeight();
            ImGui::InputInt("Zoom", &m_Scale);

            m_Scale = std::clamp(m_Scale, 1, 5);
            ImGui::SameLine();
        }
        else
        {
            ImGui::Text("No tileset loaded.");
        }
    }
    else
    {
        if (tileset && tileset->IsLoaded())
            ImGui::Text("No tilemap loaded.");
        else
            ImGui::Text("No tilemap and tileset loaded.");
    }
    ImGui::EndChild();
    ImGui::EndGroup();
}

bool TilemapPane::TilemapEdit(Tilemap *tilemap, Tileset *tileset, bool shouldDrawGrid)
{
    static bool has_cached_tile = false;
    static TileEntry cached_tile;
    bool is_changed = false;

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    int tilesnum = tileset->GetWidth() * tileset->GetHeight();
    Texture *tex = tileset->GetTexture();

    ImTextureID tex_id = (void *)(uintptr_t)tex->GetTextureID();
    ImVec2 scale = ImVec2(m_Scale, m_Scale);
    ImVec2 size = ImVec2(tex->GetWidth(), tex->GetHeight());
    ImVec2 tilesize = ImVec2(Tileset::TileSize[0], Tileset::TileSize[1]);

    bool has_hovered = false;
    int hovered_item;

    Brush &brush = Context::GetBrush();

    for (int y = 0; y < 32; y++)
    {
        for (int x = 0; x < 32; x++)
        {
            int i = x + y * 32; 
            TileEntry &tileentry = tilemap->GetTiles()[i];
            int index = tileentry.index;

            bool xflip = tileentry.xflip;
            bool yflip = tileentry.yflip;

            ImVec2 pos = window->DC.CursorPos + ImVec2(0.5f, 0.5f) + ImVec2(x, y) * tilesize * scale;

            ImVec2 uv0 = ImVec2(index % tileset->GetWidth(), index / tileset->GetWidth()) * tilesize / size;
            ImVec2 uv1 = uv0 + tilesize / size;

            if (xflip) swap_val(&uv0.x, &uv1.x);
            if (yflip) swap_val(&uv0.y, &uv1.y);

            window->DrawList->AddImage(tex_id, pos, pos + tilesize * scale, uv0, uv1);
            if (shouldDrawGrid)
                window->DrawList->AddRect(pos - ImVec2(0.5f, 0.5f), pos + tilesize * scale + ImVec2(0.5f, 0.5f), IM_COL32_BLACK);

            ImRect bb_ = ImRect(pos, pos + tilesize * scale);
            bool hovered, held;
            bool pressed = ImGui::ButtonBehavior(bb_, window->GetIDFromRectangle(bb_), &hovered, &held, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | ImGuiSelectableFlags_AllowItemOverlap);

            if (hovered)
            {
                has_hovered = true;
                hovered_item = x + y * 32;
            }

            if (pressed)
            {
                if (ImGui::GetIO().MouseClicked[1])
                {
                    brush.selected_tile = tileentry.index;
                    brush.xflip = tileentry.xflip;;
                    brush.yflip = tileentry.yflip;
                }
                else 
                {
                    Context::IsDirty() = true;
                    Context::LogAction(new PlaceTileAction(i, &tileentry, &brush));
                    TS_LOG_INFO("Placed tile at %i (%u -> %u)", i, tileentry.index, brush.selected_tile);
                    tileentry.index = brush.selected_tile;
                    tileentry.xflip = brush.xflip;
                    tileentry.yflip = brush.yflip;
                    is_changed = true;
                }
            }
        }
    }

    ImVec2 widgetsize = ImVec2(32, 32) * tilesize;
    ImRect bb(window->DC.CursorPos, window->DC.CursorPos + widgetsize * scale + ImVec2(1.0f, 1.0f));

    if (has_hovered)
    {
        int x = hovered_item % 32, y = hovered_item / 32;
        ImVec2 pos = window->DC.CursorPos + ImVec2(0.5f, 0.5f) + ImVec2(x, y) * tilesize * scale;
        window->DrawList->AddRect(pos - ImVec2(0.5f, 0.5f), pos + tilesize * scale + ImVec2(0.5f, 0.5f), IM_COL32(255, 0, 0, 255));
    }

    window->DrawList->AddRect(bb.Min - ImVec2(0.5f, 0.5f), bb.Max + ImVec2(0.5f, 0.5f), IM_COL32_BLACK);

    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, 0))
        return false;

    return is_changed;
}
