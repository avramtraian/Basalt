// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/Core.h"

namespace Basalt
{

// Forward declarations.
class Window;

struct SwapchainDescription
{
    Window* window = nullptr;
};

class Swapchain
{
public:
    static Unique<Swapchain> Create(const SwapchainDescription& description);

public:
    virtual ~Swapchain() = default;
};

} // namespace Basalt
