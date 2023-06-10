// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "Engine.h"

#include "Application/Window.h"

namespace Basalt
{

Engine* g_engine = nullptr;

bool Engine::Initialize(const EngineDescription& description)
{
    if (g_engine)
    {
        BT_LOG_ERROR(Engine, "The engine was already initialized! Aborting.");
        return false;
    }

    g_engine = description.instantiate_engine();
    if (!g_engine)
    {
        BT_LOG_ERROR(Engine, "Failed to instantiate the global engine! Aborting.");
        return false;
    }

    WindowDescription window_description = {};
    window_description.mode = EWindowMode::Windowed;
    window_description.title = "Basalt Editor - Win64 - Untitled*"sv;

    g_engine->m_primary_window = Window::Create();
    Check(g_engine->m_primary_window);
    if (!g_engine->m_primary_window->Initialize(window_description))
    {
        BT_LOG_ERROR(Engine, "Failed to initialize the primary window! Aborting.");
        return false;
    }

    g_engine->m_is_running = true;

    BT_LOG_INFO(Engine, "Engine initialization was successful!");
    return true;
}

void Engine::Shutdown()
{
    delete g_engine;
    g_engine = nullptr;
}

bool Engine::IsRunning()
{
    return m_is_running;
}

void Engine::Tick()
{
    m_primary_window->ProcessMessages();

    if (m_primary_window->IsPendingClose())
    {
        // By releasing the instance, the window will be destroyed.
        m_primary_window.Release();

        m_is_running = false;
    }
}

Window* Engine::GetWindowByHandle(WindowHandle window_handle)
{
    return (m_primary_window->GetNativeHandle() == window_handle) ? m_primary_window.Get() : nullptr;
}

} // namespace Basalt
