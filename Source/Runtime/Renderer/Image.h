// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/Core.h"

namespace Basalt
{

enum class EImageFormat : U32
{
    None = 0,
    // Color formats.
    R32G32B32A32,
    // Depth formats.
    DEPTH16, DEPTH32, DEPTH24STENCIL8,
    MaxEnumValue
};

namespace Utils
{

FORCEINLINE bool IsDepthFormat(EImageFormat format)
{
    switch (format)
    {
        case EImageFormat::DEPTH16:         return true;
        case EImageFormat::DEPTH32:         return true;
        case EImageFormat::DEPTH24STENCIL8: return true;
    }

    return false;
}

FORCEINLINE bool IsStencilFormat(EImageFormat format)
{
    switch (format)
    {
        case EImageFormat::DEPTH24STENCIL8: return true;
    }

    return false;
}

} // namespace Basalt::Utils

} // namespace Basalt
