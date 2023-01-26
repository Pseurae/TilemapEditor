#pragma once

#include "core/pane.h"

#include <string>

class TilemapPane : public Pane
{
public:
    explicit TilemapPane() : Pane("Tilemap"), m_Scale(2) {};
    virtual void Draw() override;
private:
    int m_Scale;
};
