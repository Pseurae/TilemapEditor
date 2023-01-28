#include "core/window.h"
#include "core/context.h"
#include "helpers/events.h"
#include "helpers/fs.h"
#include "helpers/hotkeys.h"
#include "helpers/logger.h"
#include "imgui_internal.h"
#include <cstdio>
#include <functional>
#include <string_view>
#include <iostream>
#include <format>

#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <cstddef>
#include <cstdlib>
#include <string>

#include "core/pane.h"
#include "panes/tilemap.h"
#include "panes/tileset.h"
#include "helpers/registry.h"

Window::Window(const std::string &title, int width, int height) : m_Title(title), m_Width(width), m_Height(height)
{
    Logger::Init();

    this->InitGLFW();
    this->InitImGui();

    TS_LOG_INFO("Created window ('%s' %i %i)", title.c_str(), width, height);

    this->m_Panes.push_back(new TilesetPane());
    this->m_Panes.push_back(new TilemapPane());

    RegisterEvents();
    RegisterHotKeys();
    Context::DeserializeRecentPaths();
}

Window::~Window()
{ 
    this->ExitImGui();
    this->ExitGLFW();
}

void Window::InitGLFW()
{
    if (!glfwInit())
    {
        TS_LOG_FATAL("Failed to initialize GLFW.");
        std::abort();
    }

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

#if defined(OS_MACOS)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

    m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), NULL, NULL);
    if (m_Window == nullptr)
    {
        TS_LOG_FATAL("Failed to create GLFW Window.");
        std::abort();
    }

    glfwMakeContextCurrent(m_Window);
    glfwSwapInterval(1);

    glfwSetWindowUserPointer(m_Window, this);

    glfwSetKeyCallback(m_Window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
        Window *win = static_cast<Window *>(glfwGetWindowUserPointer(window));

        if (action == GLFW_PRESS || action == GLFW_REPEAT) 
        {
            HotKeyManager::AddKey(key);
#if defined(OS_MACOS)
            int temp = mods & (GLFW_MOD_SUPER | GLFW_MOD_CONTROL);
            if (temp == GLFW_MOD_SUPER)
            {
                mods &= ~GLFW_MOD_SUPER;
                mods |= GLFW_MOD_CONTROL;
            }
            else if (temp == GLFW_MOD_CONTROL)
            {
                mods &= ~GLFW_MOD_CONTROL;
                mods |= GLFW_MOD_SUPER;
            }
#endif
            HotKeyManager::SetMods(mods);
            win->PreFrame();
            win->Frame();
            win->PostFrame();
        }
    });

    glfwSetDropCallback(m_Window, [](GLFWwindow *window, int path_count, const char *paths[]) {
        if (path_count <= 0)
            return;

        std::string path = std::string(paths[0], strlen(paths[0]));
        std::string extension = path.substr(path.find_last_of(".") + 1);
        if (extension == "bin")
            EventManager::publish<RequestOpenTilemap>(path);
        else if (extension == "png")
            EventManager::publish<RequestOpenTileset>(path);
    });

    glfwSetWindowCloseCallback(m_Window, [](GLFWwindow *window) {
        glfwSetWindowShouldClose(window, GLFW_FALSE);
        EventManager::publish<RequestOpenPopup>("QuitMessage");
    });

    int win_w, win_h, fb_w, fb_h;
    glfwGetWindowSize(m_Window, &win_w, &win_h);
    glfwGetFramebufferSize(m_Window, &fb_w, &fb_h);

    m_DPIScaling = std::max((float)fb_w / win_w, (float)fb_h / win_h);

    glfwShowWindow(m_Window);

    EventManager::subscribe<RequestProgramQuit>([this](bool confirm) {
        if (confirm)
            EventManager::publish<RequestOpenPopup>("QuitMessage");
        else
            glfwSetWindowShouldClose(this->m_Window, GLFW_TRUE);
    });

    TS_LOG_INFO("GLFW initialized.");
}

void Window::InitImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGuiIO &io = ImGui::GetIO();
    ImFontConfig font_cfg;
    font_cfg.FontDataOwnedByAtlas = false;
    io.Fonts->AddFontFromFileTTF("res/JetBrainsMono-Regular.ttf", 14.0f * m_DPIScaling);
    io.FontGlobalScale /= m_DPIScaling;

    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
#if defined(OS_MACOS)
    ImGui_ImplOpenGL3_Init("#version 150");
#else
    ImGui_ImplOpenGL3_Init("#version 130");
#endif

    TS_LOG_INFO("ImGui initialized.");
}

void Window::ExitGLFW()
{
    glfwDestroyWindow(this->m_Window);
    glfwTerminate();
}

void Window::ExitImGui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::Shutdown();
    TS_LOG_INFO("ImGui shutdown.");
}

void Window::Run()
{
    while (!glfwWindowShouldClose(m_Window))
    {
        if (glfwGetWindowAttrib(m_Window, GLFW_ICONIFIED) || 
            !glfwGetWindowAttrib(m_Window, GLFW_VISIBLE))
        {
            glfwWaitEvents();
        }
        else
        {
            glfwPollEvents();
        }

        this->PreFrame();
        this->Frame();
        this->PostFrame();
    }
}

void Window::PreFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

static constexpr ImGuiWindowFlags s_WindowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse;

void Window::Frame()
{
    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);

    ImGui::Begin("Window", NULL, s_WindowFlags | ImGuiWindowFlags_MenuBar);

    ImGui::BeginMenuBar();
    for (auto &[name, callback] : Context::GetMenuBar())
    {
        if (ImGui::BeginMenu(name.c_str()))
        {
            callback();
            ImGui::EndMenu();
        }
    }
    ImGui::EndMenuBar();

    for (Pane *pane : m_Panes)
    {
        pane->Draw();
    }

    if (!ImGui::GetTopMostPopupModal())
        HotKeyManager::ProcessHotKeys();

    HotKeyManager::ClearKeys();

    Context::ProcessPopups();
    DrawPopups();

    ImGui::End();
}

void Window::PostFrame()
{
    int display_w, display_h;

    ImGui::Render();
    glfwGetFramebufferSize(m_Window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(m_Window);
}
