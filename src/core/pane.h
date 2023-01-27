#pragma once

#include <string>
#include <imgui.h>

class Pane
{
public:
    static constexpr ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoCollapse;
    explicit Pane(std::string title) : m_Title(title) {}
    virtual ~Pane() = default;
    virtual void Draw() {};
private:
    std::string m_Title;
};
