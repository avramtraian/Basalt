// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "VulkanShader.h"

#include "VulkanRenderer.h"

namespace Basalt
{

VulkanShader::VulkanShader(const ShaderDescription& description)
{
    for (U8 shader_stage = 0; shader_stage < (U8)EShaderStage::MaxEnumValue; ++shader_stage)
    {
        Buffer bytecode = description.bytecodes[shader_stage];

        if (bytecode.size > 0)
        {
            m_shader_modules[shader_stage] = CreateShaderModule((EShaderStage)shader_stage, bytecode);
        }
        else
        {
            m_shader_modules[shader_stage] = VK_NULL_HANDLE;
        }
    }
}

VulkanShader::~VulkanShader()
{
    for (VkShaderModule shader_module : m_shader_modules)
    {
        if (shader_module != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(VulkanRenderer::GetDevice(), shader_module, VulkanRenderer::GetAllocator());
        }
    }
}

VkShaderModule VulkanShader::CreateShaderModule(EShaderStage stage, Buffer bytecode)
{
    VkShaderModuleCreateInfo shader_module_create_info = {};
    shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_module_create_info.codeSize = bytecode.size;
    shader_module_create_info.pCode = bytecode.As<U32>();

    VkShaderModule shader_module = VK_NULL_HANDLE;
    BT_VULKAN_CHECK(vkCreateShaderModule(VulkanRenderer::GetDevice(), &shader_module_create_info, VulkanRenderer::GetAllocator(), &shader_module));
    return shader_module;
}

} // namespace Basalt
