// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/Core.h"

namespace Basalt
{

/**
 * Abstract interface that all API renderers must provide.
 * This is also the base class for all native renderers.
 * 
 * All documentation for these functions can be found in the `Renderer` class.
 */
class RendererInterface
{
public:
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
};

} // namespace Basalt
