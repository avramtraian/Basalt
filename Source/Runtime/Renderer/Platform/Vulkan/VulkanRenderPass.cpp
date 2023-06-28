// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "VulkanRenderPass.h"

#include "VulkanFramebuffer.h"
#include "VulkanRenderer.h"

namespace Basalt
{

namespace Utils
{

FORCEINLINE static VkAttachmentLoadOp LoadOperationToVulkan(ERenderPassLoadOperation load_operation)
{
    switch (load_operation)
    {
        case ERenderPassLoadOperation::Discard:     return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        case ERenderPassLoadOperation::Preserve:    return VK_ATTACHMENT_LOAD_OP_LOAD;
        case ERenderPassLoadOperation::Clear:       return VK_ATTACHMENT_LOAD_OP_CLEAR;
    }

    Checkf(false, "Invalid ERenderPassLoadOperation!");
    return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
}

FORCEINLINE static VkAttachmentStoreOp StoreOperationToVulkan(ERenderPassStoreOperation store_operation)
{
    switch (store_operation)
    {
        case ERenderPassStoreOperation::Discard:     return VK_ATTACHMENT_STORE_OP_DONT_CARE;
        case ERenderPassStoreOperation::Preserve:    return VK_ATTACHMENT_STORE_OP_STORE;
    }

    Checkf(false, "Invalid ERenderPassLoadOperation!");
    return VK_ATTACHMENT_STORE_OP_DONT_CARE;
}

} // namespace Basalt::Utils

VulkanRenderPass::VulkanRenderPass(const RenderPassDescription& description)
    : m_description(description)
{
    CreateRenderPass();
    description.target_framebuffer.As<VulkanFramebuffer>()->Initialize(Ref<VulkanRenderPass>(this));
}

VulkanRenderPass::~VulkanRenderPass()
{
    m_description.target_framebuffer.Release();
    vkDestroyRenderPass(VulkanRenderer::GetDevice(), m_render_pass, VulkanRenderer::GetAllocator());
}

void VulkanRenderPass::CreateRenderPass()
{
    VulkanFramebuffer* vulkan_framebuffer = m_description.target_framebuffer.As<VulkanFramebuffer>().Get();

    Array<VkAttachmentDescription> attachments;
    attachments.SetCapacity(m_description.attachment_descriptions.Count());

    Array<VkAttachmentReference> color_attachment_references;
    Optional<VkAttachmentReference> depth_stencil_attachment_reference = {};

    for (U32 index = 0; index < attachments.Count(); ++index)
    {
        RenderPassAttachment render_pass_attachment = m_description.attachment_descriptions[index];
        VkAttachmentDescription& attachment_description = attachments.AddZeroed();
        const FramebufferAttachment& framebuffer_attachment = vulkan_framebuffer->m_description.attachments[index];

        attachment_description.format = Utils::ImageFormatToVulkan(framebuffer_attachment.format);
        attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
        attachment_description.loadOp = Utils::LoadOperationToVulkan(render_pass_attachment.load_operation);
        attachment_description.storeOp = Utils::StoreOperationToVulkan(render_pass_attachment.store_operation);

        if (Utils::IsStencilFormat(framebuffer_attachment.format))
        {
            attachment_description.stencilLoadOp = attachment_description.loadOp;
            attachment_description.stencilStoreOp = attachment_description.storeOp;
        }
        else
        {
            attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        }

        attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        if (Utils::IsDepthFormat(framebuffer_attachment.format))
        {
            if (Utils::IsStencilFormat(framebuffer_attachment.format))
            {
                attachment_description.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            }
            else
            {
                attachment_description.finalLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
            }

            depth_stencil_attachment_reference = {};
            depth_stencil_attachment_reference->attachment = index;
            depth_stencil_attachment_reference->layout = attachment_description.finalLayout;
        }
        else
        {
            attachment_description.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkAttachmentReference& color_attachment_reference = color_attachment_references.AddZeroed();
            color_attachment_reference.attachment = index;
            color_attachment_reference.layout = attachment_description.finalLayout;
        }
    }

    VkSubpassDescription subpass_description = {};
    subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass_description.colorAttachmentCount = (U32)color_attachment_references.Count();
    subpass_description.pColorAttachments = *color_attachment_references;
    subpass_description.pDepthStencilAttachment = depth_stencil_attachment_reference.GetPtr();

    VkRenderPassCreateInfo render_pass_create_info = {};
    render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_create_info.attachmentCount = (U32)attachments.Count();
    render_pass_create_info.pAttachments = *attachments;
    render_pass_create_info.subpassCount = 1;
    render_pass_create_info.pSubpasses = &subpass_description;

    BT_VULKAN_CHECK(vkCreateRenderPass(VulkanRenderer::GetDevice(), &render_pass_create_info, VulkanRenderer::GetAllocator(), &m_render_pass));
}

} // namespace Basalt
