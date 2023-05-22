// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "Engine.h"

namespace Basalt
{

Engine* GEngine = nullptr;

bool Engine::Initialize(const EngineDescription& description)
{
    GEngine = description.instantiateEngine();
    return true;
}

void Engine::Shutdown()
{
    delete GEngine;
    GEngine = nullptr;
}

bool Engine::IsRunning() const
{
    return true;
}

void Engine::Tick()
{

}

} // namespace Basalt
