// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Application/Window.h"
#include "Core/Core.h"

namespace Basalt
{

/** Forward declarations. */
class Engine;

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
    virtual bool PostInitialize() { return true; }
    virtual void PreShutdown() {}

    /** @return Whether or not to keep ticking the engine. */
    bool IsRunning();

    virtual void Tick();

    virtual Window* GetWindowByHandle(WindowHandle window_handle);

private:
    /** Whether or not the engine is running. While this is true, the engine will keep ticking. */
    bool m_is_running = false;

    /** The window where the game/viewport will be displayed. Has control over the lifetime of the application. */
    Unique<Window> m_primary_window;
};

/**
 * Pointer to the global engine instance.
 * Allows to get access to the engine systems during runtime.
 */
BASALT_API extern Engine* g_engine;

} // namespace Basalt
