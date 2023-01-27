#include "panes/tileset.h"
#include "core/context.h"
#include "core/tileset.h"
#include "helpers/fs.h"
#include "helpers/texture.h"
#include <cassert>
#include <imgui.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

static constexpr int s_TilePerRow = 7;

void TilesetPane::Draw()
{
    if (ImGui::BeginChild("Tileset", ImVec2(270.0f, 0.0f), true))
    {
        Tileset *tileset = Context::GetTileset();

        ImGui::Spacing();

        if (tileset && tileset->IsLoaded())
        {
            Brush &brush = Context::GetBrush();
            ImGui::Text("Tile ID: %i", brush.selected_tile);
            ImGui::Checkbox("Tile X Flip", &brush.xflip);
            ImGui::Checkbox("Tile Y Flip", &brush.yflip);

            ImGui::Spacing();
            int tilecount = tileset->GetWidth() * tileset->GetHeight();
            ImGui::Text("Tile Count: %i (%i + 1)", tilecount, tilecount - 1);

            ImGui::Spacing();

            if (ImGui::BeginChild("###TilesetView", ImVec2(256.0f, 0.0f)))
            {
                this->TileSelector(tileset, Context::ShouldShowTSGrid());
                ImGui::EndChild();
            }
        }
        else
        {
            ImGui::Text("No tileset loaded.");
        }
        ImGui::EndChild();
    }
}

bool TilesetPane::TileSelector(Tileset *tileset, bool shouldDrawGrid)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    int tilesnum = tileset->GetWidth() * tileset->GetHeight();
    Texture *tex = tileset->GetTexture();

    ImTextureID tex_id = (void *)(uintptr_t)tex->GetTextureID();
    ImVec2 scale = ImVec2(4.0f, 4.0f);
    ImVec2 size = ImVec2(tex->GetWidth(), tex->GetHeight());
    ImVec2 tilesize = ImVec2(Tileset::TileSize[0], Tileset::TileSize[1]);

    Brush &brush = Context::GetBrush();

    for (int i = 0; i < tilesnum; i++)
    {
        int x = i % s_TilePerRow, y = i / s_TilePerRow;
        ImVec2 uv0 = ImVec2(i % tileset->GetWidth(), i / tileset->GetWidth()) * tilesize / size;
        ImVec2 pos = window->DC.CursorPos + ImVec2(0.5f, 0.5f) + ImVec2(x, y) * tilesize * scale;
        window->DrawList->AddImage(tex_id, pos, pos + tilesize * scale, uv0, uv0 + tilesize / size);
        if (shouldDrawGrid)
            window->DrawList->AddRect(pos - ImVec2(0.5f, 0.5f), pos + tilesize * scale + ImVec2(0.5f, 0.5f), IM_COL32_BLACK);

        ImRect bb_ = ImRect(pos, pos + tilesize * scale);
        bool hovered, held;
        bool pressed = ImGui::ButtonBehavior(bb_, window->GetIDFromRectangle(bb_), &hovered, &held);
        
        if (held || pressed)
            brush.selected_tile = i;
    }

    ImVec2 widgetsize = ImVec2(s_TilePerRow, tilesnum / s_TilePerRow + 1) * tilesize;
    ImRect bb(window->DC.CursorPos, window->DC.CursorPos + widgetsize * scale + ImVec2(1.0f, 1.0f));

    ImVec2 pos = window->DC.CursorPos + ImVec2(0.5f, 0.5f) + ImVec2(brush.selected_tile % s_TilePerRow, brush.selected_tile / s_TilePerRow) * tilesize * scale;
    window->DrawList->AddRect(pos - ImVec2(0.5f, 0.5f), pos + tilesize * scale + ImVec2(0.5f, 0.5f), IM_COL32(255, 0, 0, 255));

    if (!shouldDrawGrid)
        window->DrawList->AddRect(bb.Min - ImVec2(0.5f, 0.5f), bb.Max + ImVec2(0.5f, 0.5f), IM_COL32_BLACK);

    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, 0))
        return false;

    return true;
}
