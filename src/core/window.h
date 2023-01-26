#pragma once

#include <list>
#include <mutex>
#include <string>
#include <vector>

class Pane;
class TilemapPane;
class TilesetPane;
struct GLFWwindow;

class Window
{
public:
    Window(const std::string &title, int width, int height);
    ~Window();
    void InitGLFW();
    void InitImGui();
    void ExitGLFW();
    void ExitImGui();
    void Run();
    void PreFrame();
    void Frame();
    void PostFrame();
private:
    GLFWwindow *m_Window;
    const std::string m_Title;
    int m_Width, m_Height;
    float m_DPIScaling;
    std::vector<Pane *> m_Panes;
};
