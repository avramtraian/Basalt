// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Renderer/Texture.h"

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

FORCEINLINE DXGI_FORMAT TextureFormatToDXGIFormat(ETextureFormat format)
{
	switch (format)
	{
		case ETextureFormat::R8G8B8A8: return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
	}

	Checkf(false, "Invalid ETextureFormat!");
	return DXGI_FORMAT_UNKNOWN;
}

} // namespace Basalt::Utils
} // namespace Basalt
