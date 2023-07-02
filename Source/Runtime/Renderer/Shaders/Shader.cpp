// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "Shader.h"

#include "Renderer/Platform/Vulkan/VulkanShader.h"
#include "Renderer/Renderer.h"

namespace Basalt
{

Ref<Shader> Shader::Create(const ShaderDescription& description)
{
	switch (Renderer::GetRendererAPI())
	{
		case ERendererAPI::Vulkan: return Ref<VulkanShader>::Create(description).As<Shader>();
	}

	Checkf(false, "Invalid ERendererAPI!");
	return nullptr;
}

} // namespace Basalt
