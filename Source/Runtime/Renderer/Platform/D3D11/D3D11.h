// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Renderer/Image.h"

#include <d3d11.h>

#define INVALID_D3D11_CALL false

#define BT_D3D11_CHECK(EXPRESSION)                  \
    if (!SUCCEEDED(EXPRESSION))                     \
    {                                               \
        Checkf(INVALID_D3D11_CALL, #EXPRESSION);    \
    }

namespace Basalt
{
namespace Utils
{

FORCEINLINE DXGI_FORMAT ImageFormatToDirectX(EImageFormat format)
{
	switch (format)
	{
        // Color formats.
		case EImageFormat::R32G32B32A32:    return DXGI_FORMAT_R32G32B32A32_FLOAT;

        // Depth-stencil formats.
        case EImageFormat::DEPTH16:         return DXGI_FORMAT_D16_UNORM;
        case EImageFormat::DEPTH32:         return DXGI_FORMAT_D32_FLOAT;
        case EImageFormat::DEPTH24STENCIL8: return DXGI_FORMAT_D24_UNORM_S8_UINT;
	}

	Checkf(false, "Invalid EImageFormat!");
	return DXGI_FORMAT_UNKNOWN;
}

} // namespace Basalt::Utils
} // namespace Basalt
