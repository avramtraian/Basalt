// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "GameEngine.h"

namespace Basalt
{

bool GameEngine::PostInitialize()
{
    return true;
}

void GameEngine::PreShutdown()
{
}

void GameEngine::Tick()
{
    Super::Tick();
}

} // namespace Basalt
