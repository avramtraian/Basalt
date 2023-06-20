// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/Core.h"

namespace Basalt
{

class RendererInterface
{
public:
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
};

} // namespace Basalt
