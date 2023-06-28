// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Renderer/Framebuffer.h"
#include "Vulkan.h"

namespace Basalt
{

class VulkanFramebuffer : public Framebuffer
{
public:
    VulkanFramebuffer(const FramebufferDescription& description);
    virtual ~VulkanFramebuffer() override;

public:
    void Initialize(Ref<class VulkanRenderPass> render_pass);

private:
    VkImage CreateImage(const FramebufferAttachment& attachment);
    VkImageView CreateImageView(VkImage image, EImageFormat format);

private:
    FramebufferDescription m_description;

    Array<VkImage> m_framebuffer_images;
    Array<VkImageView> m_framebuffer_image_views;
    VkFramebuffer m_framebuffer = VK_NULL_HANDLE;

    friend class VulkanRenderPass;
};

} // namespace Basalt
