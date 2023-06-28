// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Renderer/RenderingContext.h"
#include "Vulkan.h"

namespace Basalt
{

class VulkanContext : public RenderingContext
{
public:
    VulkanContext(const RenderingContextDescription& description);
    virtual ~VulkanContext() override;

    void Invalidate();

private:
    void CreateSurface();
    void CreateSwapchain();
    void GetSwapchainImagesViews();

private:
    struct SwapchainSupport
    {
        VkSurfaceCapabilitiesKHR surface_capabilities = {};
        Array<VkSurfaceFormatKHR> surface_formats;
        Array<VkPresentModeKHR> present_modes;
    };

    void QuerySwapchainSupport(SwapchainSupport& out_support);

private:
    // The window who is associated with this rendering context.
    Window* m_window_context;

    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    VkSurfaceFormatKHR m_swapchain_format;

    Array<VkImage> m_swapchain_images;
    Array<VkImageView> m_swapchain_images_views;
};

} // namespace Basalt
