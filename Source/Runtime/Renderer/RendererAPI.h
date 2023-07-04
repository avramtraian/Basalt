// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/Core.h"

namespace Basalt
{

using RendererID = void*;

enum class RendererAPI : U8
{
    None = 0,
    D3D11,
    D3D12,
    Vulkan,
    Metal,

    MaxEnumValue
};

FORCEINLINE StringView RendererAPIToString(RendererAPI renderer_api)
{
    switch (renderer_api)
    {
        case RendererAPI::None:    return "None"sv;
        case RendererAPI::D3D11:   return "D3D11"sv;
        case RendererAPI::D3D12:   return "D3D12"sv;
        case RendererAPI::Vulkan:  return "Vulkan"sv;
        case RendererAPI::Metal:   return "Metal"sv;
    }

    Check(false); // Invalid RendererAPI.
    return "Unknown"sv;
}

} // namespace Basalt
