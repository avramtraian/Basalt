// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "VulkanFramebuffer.h"

#include "VulkanRenderer.h"
#include "VulkanRenderPass.h"

namespace Basalt
{

VulkanFramebuffer::VulkanFramebuffer(const FramebufferDescription& description)
    : m_description(description)
{
}

VulkanFramebuffer::~VulkanFramebuffer()
{
    auto device = VulkanRenderer::GetDevice();
    auto allocator = VulkanRenderer::GetAllocator();

    for (Usize index = 0; index < m_framebuffer_images.Count(); ++index)
    {
        vkDestroyImageView(device, m_framebuffer_image_views[index], allocator);
        vkDestroyImage(device, m_framebuffer_images[index], allocator);
    }

    vkDestroyFramebuffer(device, m_framebuffer, allocator);
}

void VulkanFramebuffer::Initialize(Ref<VulkanRenderPass> render_pass)
{
    m_framebuffer_images.SetCapacity(m_description.attachments.Count());
    m_framebuffer_image_views.SetCapacity(m_description.attachments.Count());

    for (FramebufferAttachment attachment : m_description.attachments)
    {
        m_framebuffer_images.Add(CreateImage(attachment));
        m_framebuffer_image_views.Add(CreateImageView(m_framebuffer_images.Back(), attachment.format));
    }

    VkFramebufferCreateInfo framebuffer_create_info = {};
    framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_create_info.renderPass = render_pass.Get()->m_render_pass;
    framebuffer_create_info.attachmentCount = (U32)m_framebuffer_image_views.Count();
    framebuffer_create_info.pAttachments = *m_framebuffer_image_views;
    framebuffer_create_info.width = m_description.width;
    framebuffer_create_info.width = m_description.height;
    framebuffer_create_info.layers = 1;

    BT_VULKAN_CHECK(vkCreateFramebuffer(VulkanRenderer::GetDevice(), &framebuffer_create_info, VulkanRenderer::GetAllocator(), &m_framebuffer));
}

VkImage VulkanFramebuffer::CreateImage(const FramebufferAttachment& attachment)
{
    VkImageCreateInfo image_create_info = {};
    image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_create_info.imageType = VK_IMAGE_TYPE_2D;
    image_create_info.format = Utils::ImageFormatToVulkan(attachment.format);
    image_create_info.extent.width = m_description.width;
    image_create_info.extent.height = m_description.height;
    image_create_info.extent.depth = 1;
    image_create_info.mipLevels = 1;
    image_create_info.arrayLayers = 1;
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;

    if (!Utils::IsDepthFormat(attachment.format))
    {
        image_create_info.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    }
    else
    {
        image_create_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    }

    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_create_info.queueFamilyIndexCount = 1;
    U32 graphics_queue_family_index = VulkanRenderer::GetGraphicsQueueFamilyIndex();
    image_create_info.pQueueFamilyIndices = &graphics_queue_family_index;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VkImage image = VK_NULL_HANDLE;
    BT_VULKAN_CHECK(vkCreateImage(VulkanRenderer::GetDevice(), &image_create_info, VulkanRenderer::GetAllocator(), &image));
    return image;
}

VkImageView VulkanFramebuffer::CreateImageView(VkImage image, EImageFormat format)
{
    VkImageViewCreateInfo image_view_create_info = {};
    image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_create_info.image = image;
    image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    image_view_create_info.format = Utils::ImageFormatToVulkan(format);
    image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_view_create_info.subresourceRange.baseMipLevel = 0;
    image_view_create_info.subresourceRange.levelCount = 1;
    image_view_create_info.subresourceRange.baseArrayLayer = 0;
    image_view_create_info.subresourceRange.layerCount = 1;

    VkImageView image_view = VK_NULL_HANDLE;
    BT_VULKAN_CHECK(vkCreateImageView(VulkanRenderer::GetDevice(), &image_view_create_info, VulkanRenderer::GetAllocator(), &image_view));
    return image_view;
}

} // namespace Basalt
