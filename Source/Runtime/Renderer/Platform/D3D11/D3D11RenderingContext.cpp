// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "D3D11RenderingContext.h"

#include "D3D11Renderer.h"

namespace Basalt
{

D3D11RenderingContext::D3D11RenderingContext(const RenderingContextDescription& description)
{
    BT_LOG_TRACE(Renderer, "Creating a rendering context...");

    SwapchainDescription swapchain_description = {};
    swapchain_description.window = description.window;

    m_swapchain = Swapchain::Create(swapchain_description).As<D3D11Swapchain>();
    Check(m_swapchain.IsValid());

    BT_LOG_INFO(Renderer, "The rendering context was created successfully!");
}

D3D11RenderingContext::~D3D11RenderingContext()
{
}

} // namespace Basalt
