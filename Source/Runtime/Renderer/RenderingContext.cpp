// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "RenderingContext.h"

#include "Platform/D3D11/D3D11RenderingContext.h"

#include "Renderer.h"

namespace Basalt
{

Unique<RenderingContext> RenderingContext::Create(const RenderingContextDescription& description)
{
    switch (Renderer::GetRendererAPI())
    {
        case ERendererAPI::D3D11: return Unique<D3D11RenderingContext>::Create(description).As<RenderingContext>();
    }

    Check(false); // Invalid ERendererAPI.
    return nullptr;
}

} // namespace Basalt
