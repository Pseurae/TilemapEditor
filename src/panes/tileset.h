#pragma once

#include "core/pane.h"

class Tileset;

class TilesetPane : public Pane
{
public:
    explicit TilesetPane() : Pane("Tileset") {}
    bool TileSelector(Tileset *tileset, bool shouldDrawGrid);
    virtual void Draw() override;
};
