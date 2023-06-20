// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "Framebuffer.h"

#include "Platform/D3D11/D3D11Framebuffer.h"
#include "Renderer.h"

namespace Basalt
{

Ref<Framebuffer> Framebuffer::Create(const FramebufferDescription& description)
{
    switch (Renderer::GetRendererAPI())
    {
        case ERendererAPI::D3D11: return Ref<D3D11Framebuffer>::Create(description).As<Framebuffer>();
    }

    Checkf(false, "Invalid ERendererAPI!");
    return nullptr;
}

} // namespace Basalt
