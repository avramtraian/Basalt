// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Renderer/Shader.h"
#include "Vulkan.h"

namespace Basalt
{

class VulkanShader : public Shader
{
public:
    VulkanShader(const ShaderDescription& description);
    virtual ~VulkanShader() override;

private:
    VkShaderModule CreateShaderModule(EShaderStage stage, Buffer bytecode);

private:
    FixedArray<VkShaderModule, (U8)EShaderStage::MaxEnumValue> m_shader_modules = {};
};

} // namespace Basalt
