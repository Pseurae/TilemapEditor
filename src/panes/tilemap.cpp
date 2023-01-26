#include "panes/tilemap.h"
#include "core/context.h"
#include "core/tilemap.h"
#include "core/tileset.h"
#include "helpers/fs.h"
#include "helpers/texture.h"

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include <vector>

static bool TilemapEdit(Tilemap *tilemap, Tileset *tileset, bool shouldDrawGrid, int scalei)
{
    bool is_changed = false;

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    int tilesnum = tileset->GetWidth() * tileset->GetHeight();
    Texture *tex = tileset->GetTexture();

    ImTextureID tex_id = (void *)(uintptr_t)tex->GetTextureID();
    ImVec2 scale = ImVec2(scalei, scalei);
    ImVec2 size = ImVec2(tex->GetWidth(), tex->GetHeight());
    ImVec2 tilesize = ImVec2(Tileset::TileSize[0], Tileset::TileSize[1]);

    bool has_hovered = false;
    int hovered_item;

    Brush &brush = Context::GetBrush();

    for (int y = 0; y < 32; y++)
    {
        for (int x = 0; x < 32; x++)
        {
            TileEntry &tileentry = tilemap->GetTiles()[x + y * 32];
            int index = tileentry.index;

            bool xflip = tileentry.xflip;
            bool yflip = tileentry.yflip;

            ImVec2 pos = window->DC.CursorPos + ImVec2(0.5f, 0.5f) + ImVec2(x, y) * tilesize * scale;

            ImVec2 uv0 = ImVec2(index % tileset->GetWidth(), index / tileset->GetWidth()) * tilesize / size;
            ImVec2 uv1 = uv0 + tilesize / size;

            if (xflip)
            {
                float temp = uv0.x;
                uv0.x = uv1.x;
                uv1.x = temp;
            }

            if (yflip)
            {
                float temp = uv0.y;
                uv0.y = uv1.y;
                uv1.y = temp;
            }

            window->DrawList->AddImage(tex_id, pos, pos + tilesize * scale, uv0, uv1);
            if (shouldDrawGrid)
                window->DrawList->AddRect(pos - ImVec2(0.5f, 0.5f), pos + tilesize * scale + ImVec2(0.5f, 0.5f), IM_COL32_BLACK);


            ImRect bb_ = ImRect(pos, pos + tilesize * scale);
            bool hovered, held;
            bool pressed = ImGui::ButtonBehavior(bb_, window->GetIDFromRectangle(bb_), &hovered, &held, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);

            if (hovered)
            {
                has_hovered = true;
                hovered_item = x + y * 32;
            }

            if (held || pressed)
            {
                if (ImGui::GetIO().MouseClicked[1])
                {
                    brush.selected_tile = tileentry.index;
                    brush.xflip = tileentry.xflip;;
                    brush.yflip = tileentry.yflip;
                }
                else 
                {
                    tileentry.index = brush.selected_tile;
                    tileentry.xflip = brush.xflip;
                    tileentry.yflip = brush.yflip;
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

    window->DrawList->AddRect(bb.Min - ImVec2(0.5f, 0.5f), bb.Max + ImVec2(0.5f, 0.5f), IM_COL32_WHITE);

    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, 0))
        return false;

    return is_changed;
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
            TilemapEdit(tilemap, tileset, true, m_Scale);
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

