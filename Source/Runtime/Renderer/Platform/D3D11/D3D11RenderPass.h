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

    FORCEINLINE RenderPassDescription& GetDescription() { return m_description; }
    FORCEINLINE const RenderPassDescription& GetDescription() const { return m_description; }

private:
    RenderPassDescription m_description;
};

} // namespace Basalt
