// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Renderer/RenderPass.h"

namespace Basalt
{

class D3D11RenderPass : public RenderPass
{
public:
    D3D11RenderPass(const RenderPassDescription& description);
    virtual ~D3D11RenderPass() override;

    virtual void Begin() override;
    virtual void End() override;

private:
    RenderPassDescription m_description;
};

} // namespace Basalt
