// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "RenderPass.h"

#include "Renderer.h"
#include "Platform/D3D11/D3D11RenderPass.h"

namespace Basalt
{

Ref<RenderPass> RenderPass::Create(const RenderPassDescription& description)
{
	switch (Renderer::GetRendererAPI())
	{
		case ERendererAPI::D3D11: return Ref<D3D11RenderPass>::Create(description).As<RenderPass>();
	}

	Checkf(false, "Invalid ERendererAPI!");
	return nullptr;
}

} // namespace Basalt
