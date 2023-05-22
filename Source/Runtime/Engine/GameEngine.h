// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Engine.h"

namespace Basalt
{

class GameEngine : public Engine
{
public:
    using Super = Engine;

private:
    virtual bool PostInitialize() override;
    virtual void PreShutdown() override;

    virtual void Tick() override;
};

} // namespace Basalt
