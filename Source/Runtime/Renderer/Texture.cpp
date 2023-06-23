// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "Texture.h"

#include "Renderer.h"
#include "Platform/D3D11/D3D11Texture.h"

namespace Basalt
{

Ref<Texture2D> Texture2D::Create(const Texture2DDescription& description)
{
	switch (Renderer::GetRendererAPI())
	{
		case ERendererAPI::D3D11: return Ref<D3D11Texture2D>::Create(description).As<Texture2D>();
	}

	Checkf(false, "Invalid ERendererAPI!");
    return nullptr;
}

} // namespace Basalt
