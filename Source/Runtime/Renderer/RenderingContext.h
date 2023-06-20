// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/Core.h"

namespace Basalt
{

// Forward declarations.
class Window;

struct RenderingContextDescription
{
    Window* window = nullptr;
};

class RenderingContext
{
public:
    static Unique<RenderingContext> Create(const RenderingContextDescription& description);

public:
    virtual ~RenderingContext() = default;
};

} // namespace Basalt
