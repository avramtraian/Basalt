// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "D3D11RenderPass.h"

#include "D3D11Renderer.h"

namespace Basalt
{

D3D11RenderPass::D3D11RenderPass(const RenderPassDescription& description)
    : m_description(description)
{
}

D3D11RenderPass::~D3D11RenderPass()
{
}

} // namespace Basalt
