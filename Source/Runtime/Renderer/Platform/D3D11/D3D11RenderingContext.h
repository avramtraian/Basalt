// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Renderer/RenderingContext.h"

#include "D3D11.h"

namespace Basalt
{

class D3D11RenderingContext : public RenderingContext
{
public:
    D3D11RenderingContext(const RenderingContextDescription& description);
    virtual ~D3D11RenderingContext() override;

private:
    void CreateSwapchain(Window* window);

private:
    IDXGISwapChain* m_swapchain = nullptr;
};

} // namespace Basalt
