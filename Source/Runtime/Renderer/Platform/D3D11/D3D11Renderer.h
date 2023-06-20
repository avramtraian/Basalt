// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/Core.h"
#include "Renderer/Renderer.h"

#include <d3d11.h>

#define INVALID_D3D11_CALL false

#define BT_D3D11_CHECK(EXPRESSION)  \
    if (!SUCCEEDED(EXPRESSION))     \
    {                               \
        Checkf(INVALID_D3D11_CALL, #EXPRESSION); \
    }

namespace Basalt
{

class D3D11Renderer : public RendererInterface
{
public:
    virtual bool Initialize() override;
    virtual void Shutdown() override;

public:
    static ID3D11Device* GetDevice();

    static IDXGIFactory* GetDXGIFactory();

    static IDXGIAdapter* GetAdapter();

private:
    static IDXGIAdapter* PickBestAdapter();
};

} // namespace Basalt
