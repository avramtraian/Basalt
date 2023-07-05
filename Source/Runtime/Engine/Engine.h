// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Application/Window.h"
#include "Config.h"

namespace Basalt
{

/** Forward declarations. */
class Engine;
class Event;

struct EngineCallbacks
{
    struct
    {
        /** Invoked when a shader is about to be loaded. */
        void(*pre_shader_load)(StringView, StringView) = nullptr;

        /** Invoked after a shader was successfully loaded. */
        void(*post_shader_load)(StringView, StringView) = nullptr;
    }
    renderer;
};

/**
 * Structure that describes the behavior of the engine. It is
 * created in `GuardedMain()`, the root of application call-stack
 * and it is passed to the engine when it is initialized.
 */
struct EngineDescription
{
    /** The callback that will be invoked in order to instantiate the global engine. */
    Engine*(*instantiate_engine)(void) = nullptr;
};

/**
 * The engine class is responsible for management of critical engine systems
 * and game systems, as well as controlling the application lifetime.
 */
class BASALT_API Engine
{
public:
    /**
     * Creates and initializes the engine instance. This function is called
     * in `GuardedMain()`, after the core systems are initialized.
     * Also, the critical engine systems are initialized during this function.
     * 
     * @param description The description of the engine.
     * @return Whether or not the engine initialization was successful.
     */
    static bool Initialize(const EngineDescription& description);

    /**
     * Shuts down and destroys the engine instance. This function is called
     * in `GuardedMain()`, before the core systems are shut down.
     * Also, the critical engine systems are shut down during this function.
     */
    static void Shutdown();

public:
    virtual void GenerateEngineCallbacks() = 0;

    virtual bool PostInitialize() { return true; }
    virtual void PreShutdown() {}

    /** @return Whether or not to keep ticking the engine. */
    bool IsRunning();

    virtual void PreTick();
    virtual void Tick();
    virtual void PostTick();

    virtual Window* GetWindowByHandle(WindowHandle window_handle);

    /** @return The global engine configuration structure. */
    const Config& GetConfig() const { return m_config; }

    /** @return The global engine callbacks structure. */
    const EngineCallbacks& GetCallbacks() const { return m_engine_callbacks; }

protected:
    static void OnWindowEventCallback(WindowHandle window_handle, Event* event);

protected:
    virtual void HandleWindowEventCallback(WindowHandle window_handle, Event* event);

protected:
    /** Whether or not the engine is running. While this is true, the engine will keep ticking. */
    bool m_is_running = false;

    /** The global engine configuration. */
    Config m_config;

    /** The engine callbacks. */
    EngineCallbacks m_engine_callbacks;

    /** The window where the game/viewport will be displayed. Has control over the lifetime of the application. */
    Unique<Window> m_primary_window;
};

/**
 * Pointer to the global engine instance.
 * Allows to get access to the engine systems during runtime.
 */
BASALT_API extern Engine* g_engine;

} // namespace Basalt
