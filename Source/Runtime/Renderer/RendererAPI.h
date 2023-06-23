// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/Core.h"

namespace Basalt
{

using RendererID = void*;

enum class ERendererAPI : U8
{
    None = 0,
    D3D11,
    D3D12,
    Vulkan,
    Metal,

    MaxEnumValue
};

FORCEINLINE StringView RendererAPIToString(ERendererAPI renderer_api)
{
    switch (renderer_api)
    {
        case ERendererAPI::None:    return "None"sv;
        case ERendererAPI::D3D11:   return "D3D11"sv;
        case ERendererAPI::D3D12:   return "D3D12"sv;
        case ERendererAPI::Vulkan:  return "Vulkan"sv;
        case ERendererAPI::Metal:   return "Metal"sv;
    }

    Check(false); // Invalid ERendererAPI.
    return "Unknown"sv;
}

} // namespace Basalt
