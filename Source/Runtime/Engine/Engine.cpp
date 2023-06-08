// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "Engine.h"

namespace Basalt
{

Engine* g_engine = nullptr;

bool Engine::Initialize(const EngineDescription& description)
{
    g_engine = description.instantiate_engine();
    return true;
}

void Engine::Shutdown()
{
    delete g_engine;
    g_engine = nullptr;
}

bool Engine::IsRunning() const
{
    return true;
}

void Engine::Tick()
{

}

} // namespace Basalt
