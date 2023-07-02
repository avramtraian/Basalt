// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Engine.h"

namespace Basalt
{

#if BT_BUILD_EDITOR
    /**
     * This function instantiates the global engine and it defined by the client
     * in the editor builds.
     *
     * @return The global engine instance.
     */
    Engine* InstantiateEngine();
#endif // BT_BUILD_EDITOR

/**
 * The application entry point. This is the root of the engine call-stack.
 * It is invoked from the platform specific entry point `WinMain()` (on
 * Windows) or `main()` (on other platforms).
 * 
 * @param command_line          The command line arguments passed when the application was run.
 * @param instantiate_engine    The function that instantiates the engine. It will be
 *                              called at a later time, when the engine instance is created.
 * 
 * @return The application exit code.
 */
BASALT_API I32 GuardedMain(const char* command_line, Engine*(*instantiate_engine)(void));

} // namespace Basalt

#if BT_PLATFORM_WINDOWS
    #include <Windows.h>
#endif // BT_PLATFORM_WINDOWS

#if BT_PLATFORM_WINDOWS && BT_BUILD_SHIPPING

    INT WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR command_line, _In_ int)
    {
        // Invoke the engine platform-agnostic entry point.
#if BT_BUILD_EDITOR
        // NOTE: It is up to the client to ensure that `Basalt::InstantiateEngine()` is defined.
        return (INT)Basalt::GuardedMain(command_line, Basalt::InstantiateEngine);
#endif // BT_BUILD_EDITOR

#if BT_BUILD_GAME
        return (INT)Basalt::GuardedMain(command_line, nullptr);
#endif // BT_BUILD_GAME
    }
#else
    int main(int arg_count, char** arg_values)
    {
#if BT_PLATFORM_WINDOWS
        const char* command_line = GetCommandLineA();
#endif // BT_PLATFORM_WINDOWS

#if BT_BUILD_EDITOR
        // NOTE: It is up to the client to ensure that `Basalt::InstantiateEngine()` is defined.
        return (INT)Basalt::GuardedMain(command_line, Basalt::InstantiateEngine);
#endif // BT_BUILD_EDITOR

#if BT_BUILD_GAME
        return (INT)Basalt::GuardedMain(command_line, nullptr);
#endif // BT_BUILD_GAME
    }
#endif // BT_PLATFORM_WINDOWS && BT_BUILD_SHIPPING
