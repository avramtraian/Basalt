// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/Core.h"
#include "Renderer/Renderer.h"

#include <d3d11.h>

namespace Basalt
{

class D3D11Renderer : public RendererInterface
{
public:
    virtual bool Initialize() override;
    virtual void Shutdown() override;

private:
};

} // namespace Basalt
