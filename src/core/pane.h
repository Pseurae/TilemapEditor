#pragma once

#include <string>
#include <imgui.h>

class Pane
{
public:
    static constexpr ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoCollapse;
    explicit Pane(std::string title) : m_Title(title) {}
    virtual ~Pane() = default;

    virtual void Popup() {};
    virtual void Draw() {};
    std::string &GetName() { return m_Title; }
    const std::string &GetName() const { return m_Title; } 
private:
    std::string m_Title;
};
