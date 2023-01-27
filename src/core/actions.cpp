#include "core/actions.h"
#include "core/context.h"
#include "core/tilemap.h"

PlaceTileAction::PlaceTileAction(int idx, TileEntry *entry, Brush *brush)
{
    this->idx = idx;
    this->pidx = entry->index; this->nidx = brush->selected_tile;
    this->px = entry->xflip; this->nx = brush->xflip;
    this->py = entry->yflip; this->ny = brush->yflip;
}

void PlaceTileAction::undo()
{
    TileEntry &tileentry = Context::GetTilemap()->GetTiles()[this->idx];
    tileentry.index = this->pidx;
    tileentry.xflip = this->px;
    tileentry.yflip = this->py;
}

void PlaceTileAction::redo()
{
    TileEntry &tileentry = Context::GetTilemap()->GetTiles()[this->idx];
    tileentry.index = this->nidx;
    tileentry.xflip = this->nx;
    tileentry.yflip = this->ny;
}
