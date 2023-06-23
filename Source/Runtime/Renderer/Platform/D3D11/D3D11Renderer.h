// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/Core.h"
#include "Renderer/Renderer.h"

#include "D3D11.h"

namespace Basalt
{

class D3D11Renderer : public RendererInterface
{
public:
    virtual bool Initialize() override;
    virtual void Shutdown() override;

public:
    virtual void BeginRenderPass(Ref<RenderPass> render_pass) override;
    virtual void EndRenderPass(Ref<RenderPass> render_pass) override;

public:
    static ID3D11Device* GetDevice();
    static ID3D11DeviceContext* GetDeviceContext();

    static IDXGIFactory* GetDXGIFactory();

    static IDXGIAdapter* GetAdapter();

private:
    static IDXGIAdapter* PickBestAdapter();
};

} // namespace Basalt
