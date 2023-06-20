// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/Core.h"
#include "RendererAPI.h"
#include "RendererInterface.h"

namespace Basalt
{

struct RendererDescription
{
    ERendererAPI renderer_api = ERendererAPI::None;
};

class Renderer
{
public:
    static bool Initialize(const RendererDescription& description);
    static void Shutdown();

    static bool IsInitialized();

private:
    static RendererInterface* s_interface;
};

} // namespace Basalt
