// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "D3D11RenderingContext.h"

#include "Application/Window.h"
#include "D3D11Renderer.h"

namespace Basalt
{

D3D11RenderingContext::D3D11RenderingContext(const RenderingContextDescription& description)
{
    BT_LOG_TRACE(Renderer, "Creating a rendering context...");
    BT_LOG_TRACE(Renderer, "Creating a swapchain targeting the window '%s'...", description.window->GetTitle().c_str());

    DXGI_SWAP_CHAIN_DESC swapchain_desc = {};
    swapchain_desc.BufferDesc.Width = description.window->GetWidth();
    swapchain_desc.BufferDesc.Height = description.window->GetHeight();
    swapchain_desc.BufferDesc.RefreshRate.Numerator = 144;
    swapchain_desc.BufferDesc.RefreshRate.Denominator = 1;
    swapchain_desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
    swapchain_desc.SampleDesc.Count = 1;
    swapchain_desc.SampleDesc.Quality = 0;
    swapchain_desc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
    swapchain_desc.BufferCount = 3;
    swapchain_desc.OutputWindow = (HWND)description.window->GetNativeHandle();
    swapchain_desc.Windowed = description.window->GetMode() == EWindowMode::Windowed;

    BT_D3D11_CHECK(D3D11Renderer::GetDXGIFactory()->CreateSwapChain(D3D11Renderer::GetDevice(), &swapchain_desc, &m_swapchain));

    BT_LOG_INFO(Renderer, "The swapchain was created successfully!");
    BT_LOG_INFO(Renderer, "The rendering context was created successfully!");
}

D3D11RenderingContext::~D3D11RenderingContext()
{
    m_swapchain->Release();
}

} // namespace Basalt
