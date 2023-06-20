// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "Swapchain.h"

#include "Platform/D3D11/D3D11Swapchain.h"
#include "Renderer.h"

namespace Basalt
{

Unique<Swapchain> Swapchain::Create(const SwapchainDescription& description)
{
    switch (Renderer::GetRendererAPI())
    {
        case ERendererAPI::D3D11: return Unique<D3D11Swapchain>::Create(description).As<Swapchain>();
    }

    Check(false); // Invalid ERendererAPI.
    return nullptr;
}

} // namespace Basalt
