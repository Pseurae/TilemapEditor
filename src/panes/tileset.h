#pragma once

#include "core/pane.h"

class TilesetPane : public Pane
{
public:
    explicit TilesetPane() : Pane("Tileset") {}
    virtual void Draw() override;
};
