// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Renderer/RenderingContext.h"

#include "D3D11Swapchain.h"

#include <d3d11.h>

namespace Basalt
{

class D3D11RenderingContext : public RenderingContext
{
public:
    D3D11RenderingContext(const RenderingContextDescription& description);
    virtual ~D3D11RenderingContext() override;

private:
    Unique<D3D11Swapchain> m_swapchain;
};

} // namespace Basalt
