// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Renderer/Swapchain.h"

#include <d3d11.h>

namespace Basalt
{

class D3D11Swapchain : public Swapchain
{
public:
    D3D11Swapchain(const SwapchainDescription& description);
    virtual ~D3D11Swapchain() override;

private:
    IDXGISwapChain* m_swapchain = nullptr;
};

} // namespace Basalt
