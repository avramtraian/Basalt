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

private:
    void CreateSurface(Window* window);
    void CreateSwapchain(Window* window);

private:
    struct SwapchainSupport
    {
        VkSurfaceCapabilitiesKHR surface_capabilities = {};
        Array<VkSurfaceFormatKHR> surface_formats;
        Array<VkPresentModeKHR> present_modes;
    };

    void QuerySwapchainSupport(SwapchainSupport& out_support);

private:
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
};

} // namespace Basalt
