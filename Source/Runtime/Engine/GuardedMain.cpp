// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "Core/Core.h"
#include "Engine.h"
#include "ExitCodes.h"
#include "GameEngine.h"

#include "Core/Filesystem/FileManager.h"
#include "Core/Memory/Memory.h"
#include "Core/Logging/Logger.h"

namespace Basalt
{

BASALT_API I32 GuardedMain(const char* command_line, Engine*(*instantiate_engine)(void))
{
    // Core systems initialization.

    if (!CommandLine::Set(command_line))
    {
        return BT_EXIT_COMMAND_LINE_OVERFLOW;
    }

    MemoryDescription memory_description;

    if (!Memory::Initialize(memory_description))
    {
        return BT_EXIT_SYSTEM_INITIALIZATION_FAILED;
    }

    CommandLine::Parse();

    FileManagerDescription file_manager_description = {};

    if (!FileManager::Initialize(file_manager_description))
    {
        return BT_EXIT_SYSTEM_INITIALIZATION_FAILED;
    }

    LoggerDescription logger_description;

    if (!Logger::Initialize(logger_description))
    {
        return BT_EXIT_SYSTEM_INITIALIZATION_FAILED;
    }

    // Engine initialization.
    
    EngineDescription engine_description;
#if BASALT_BUILD_GAME
    instantiate_engine = []() -> Engine* { return btnew GameEngine(); };
#endif // BASALT_BUILD_GAME
    engine_description.instantiate_engine = instantiate_engine;

    if (!Engine::Initialize(engine_description))
    {
        return BT_EXIT_SYSTEM_INITIALIZATION_FAILED;
    }

    if (!g_engine->PostInitialize())
    {
        return BT_EXIT_SYSTEM_INITIALIZATION_FAILED;
    }
    
    // Engine ticking.

    while (g_engine->IsRunning())
    {
        g_engine->Tick();
    }

    // Engine shut down.

    g_engine->PreShutdown();
    Engine::Shutdown();

    // Core systems shut down.

    Logger::Shutdown();
    FileManager::Shutdown();
    Memory::Shutdown();

    return 0;
}

} // namespace Basalt
