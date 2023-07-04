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
		case RendererAPI::Vulkan: return Ref<VulkanShader>::Create(description).As<Shader>();
	}

	Checkf(false, "Invalid RendererAPI!");
	return nullptr;
}

template<>
BASALT_API U64 Hasher::Compute<ShaderStage>(const ShaderStage& value)
{
	return Hasher::Compute<U8>((U8)value);
}

} // namespace Basalt
