#pragma once

#include <stdint.h>

struct Action
{
    virtual ~Action() = default;
    virtual void undo() = 0;
    virtual void redo() = 0;
};

struct TileEntry;
struct Brush;

struct PlaceTileAction final : Action
{
    PlaceTileAction(int idx, TileEntry *entry, Brush *brush);
    int idx;

    int pidx, nidx;
    bool px, nx, py, ny;

    virtual void undo() override;
    virtual void redo() override;
};
