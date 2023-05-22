// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "Core/Core.h"
#include "Engine.h"
#include "GameEngine.h"

namespace Basalt
{

BASALT_API I32 GuardedMain(const CommandLineArguments& commandLine, Engine*(*instantiateEngine)(void))
{
    // Core systems initialization.

    MemoryDescription memoryDescription;
    memoryDescription.commandLine = commandLine;

    if (!Memory::Initialize(memoryDescription))
    {
        return EXIT_FAILURE;
    }

    // Engine initialization.
    
    EngineDescription engineDescription;
    engineDescription.commandLine = commandLine;

#if BASALT_BUILD_GAME
    instantiateEngine = []() -> Engine* { return bnew GameEngine(); };
#endif // BASALT_BUILD_GAME
    
    engineDescription.instantiateEngine = instantiateEngine;

    if (!Engine::Initialize(engineDescription))
    {
        return EXIT_FAILURE;
    }

    if (!GEngine->PostInitialize())
    {
        return EXIT_FAILURE;
    }
    
    // Engine ticking.

    while (GEngine->IsRunning())
    {
        GEngine->Tick();
    }

    // Engine shut down.

    GEngine->PreShutdown();
    Engine::Shutdown();

    // Core systems shut down.

    Memory::Shutdown();

    return 0;
}

} // namespace Basalt
