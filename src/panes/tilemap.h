#pragma once

#include "core/pane.h"

#include <functional>
#include <string>

class Tilemap;
class Tileset;

class TilemapPane : public Pane
{
public:
    explicit TilemapPane() : Pane("Tilemap"), m_Scale(2) {};
    virtual void Draw() override;
    bool TilemapEdit(Tilemap *tilemap, Tileset *tileset, bool shouldDrawGrid);
private:
    int m_Scale;
};
