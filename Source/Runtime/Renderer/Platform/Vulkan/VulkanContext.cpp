// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "VulkanContext.h"

#include "Application/Window.h"
#include "VulkanRenderer.h"

#if BASALT_PLATFORM_WINDOWS
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #include <vulkan/vulkan_win32.h>
#endif // BASALT_PLATFORM_WINDOWS

namespace Basalt
{

VulkanContext::VulkanContext(const RenderingContextDescription& description)
{
    // Create the platform-specific window surface.
    CreateSurface(description.window);

    // Create the swapchain.
    CreateSwapchain(description.window);
}

VulkanContext::~VulkanContext()
{
    vkDestroySwapchainKHR(VulkanRenderer::GetDevice(), m_swapchain, VulkanRenderer::GetAllocator());
    vkDestroySurfaceKHR(VulkanRenderer::GetInstance(), m_surface, VulkanRenderer::GetAllocator());
}

void VulkanContext::CreateSurface(Window* window)
{
#if BASALT_PLATFORM_WINDOWS
    VkWin32SurfaceCreateInfoKHR surface_create_info = {};
    surface_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surface_create_info.hinstance = GetModuleHandle(NULL);
    surface_create_info.hwnd = (HWND)window->GetNativeHandle();

    BT_VULKAN_CHECK(vkCreateWin32SurfaceKHR(VulkanRenderer::GetInstance(), &surface_create_info, VulkanRenderer::GetAllocator(), &m_surface));
#endif // BASALT_PLATFORM_WINDOWS

    // Ensure that the queue family has present capabilities.
    CheckCode(
        VkPhysicalDevice physical_device = VulkanRenderer::GetPhysicalDevice();
        VkBool32 present_support = false;
        BT_VULKAN_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, 0, m_surface, &present_support));
    );
    Checkf(present_support, "The Vulkan queue family selected for presenting doesn't actually have present capabilities!");
}

void VulkanContext::CreateSwapchain(Window* window)
{
    VkDevice device = VulkanRenderer::GetDevice();
    VkPhysicalDevice physical_device = VulkanRenderer::GetPhysicalDevice();

    SwapchainSupport swapchain_support;
    QuerySwapchainSupport(swapchain_support);

    // The first format in the list is usually good enough.
    // This format is selected only if the ideal surface format is not found.
    VkSurfaceFormatKHR surface_format = swapchain_support.surface_formats[0];
    for (VkSurfaceFormatKHR format : swapchain_support.surface_formats)
    {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            // The ideal surface format was found.
            surface_format = format;
            break;
        }
    }

    // This present mode is guaranteed by the specification to exist.
    // This format is selected only if the ideal present mode is not found.
    VkPresentModeKHR present_mode = VK_PRESENT_MODE_MAILBOX_KHR;
    for (VkPresentModeKHR mode : swapchain_support.present_modes)
    {
        if (mode == VK_PRESENT_MODE_FIFO_KHR)
        {
            // The ideal surface present mode was found.
            present_mode = mode;
            break;
        }
    }

    VkExtent2D extent = swapchain_support.surface_capabilities.currentExtent;
    if (extent.width == 0xFFFFFFFF && extent.height == 0xFFFFFFFF)
    {
        extent.width = Math::Clamp(
            window->GetWidth(),
            (U32)swapchain_support.surface_capabilities.minImageExtent.width,
            (U32)swapchain_support.surface_capabilities.maxImageExtent.width
        );

        extent.height = Math::Clamp(
            window->GetHeight(),
            (U32)swapchain_support.surface_capabilities.minImageExtent.height,
            (U32)swapchain_support.surface_capabilities.maxImageExtent.height
        );
    }

    U32 image_count = swapchain_support.surface_capabilities.minImageCount + 1;
    if (swapchain_support.surface_capabilities.maxImageCount > 0 && image_count > swapchain_support.surface_capabilities.maxImageCount)
    {
        image_count = swapchain_support.surface_capabilities.maxImageCount;
    }

    VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE;
    U32 queue_family_indices_count = 1;
    U32 queue_family_indices[2] =
    {
        VulkanRenderer::GetGraphicsQueueFamilyIndex(),
        VulkanRenderer::GetPresentQueueFamilyIndex(),
    };

    if (queue_family_indices[0] != queue_family_indices[1])
    {
        sharing_mode = VK_SHARING_MODE_CONCURRENT;
        queue_family_indices_count = 2;
    }

    VkSwapchainCreateInfoKHR swapchain_create_info = {};
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_create_info.surface = m_surface;
    swapchain_create_info.minImageCount = image_count;
    swapchain_create_info.imageFormat = surface_format.format;
    swapchain_create_info.imageColorSpace = surface_format.colorSpace;
    swapchain_create_info.imageExtent = extent;
    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchain_create_info.imageSharingMode = sharing_mode;
    swapchain_create_info.queueFamilyIndexCount = queue_family_indices_count;
    swapchain_create_info.pQueueFamilyIndices = queue_family_indices;
    swapchain_create_info.preTransform = swapchain_support.surface_capabilities.currentTransform;
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.presentMode = present_mode;
    swapchain_create_info.clipped = VK_FALSE;
    swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;

    BT_VULKAN_CHECK(vkCreateSwapchainKHR(VulkanRenderer::GetDevice(), &swapchain_create_info, VulkanRenderer::GetAllocator(), &m_swapchain));
}

void VulkanContext::QuerySwapchainSupport(SwapchainSupport& out_support)
{
    // Query surface capabilities.
    BT_VULKAN_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VulkanRenderer::GetPhysicalDevice(), m_surface, &out_support.surface_capabilities));

    // Query surface formats.
    U32 surface_formats_count = 0;
    BT_VULKAN_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(VulkanRenderer::GetPhysicalDevice(), m_surface, &surface_formats_count, nullptr));
    out_support.surface_formats.SetCountUninitialized(surface_formats_count);
    BT_VULKAN_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(VulkanRenderer::GetPhysicalDevice(), m_surface, &surface_formats_count, *out_support.surface_formats));

    // Query surface present modes.
    U32 present_modes_count = 0;
    BT_VULKAN_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(VulkanRenderer::GetPhysicalDevice(), m_surface, &present_modes_count, nullptr));
    out_support.present_modes.SetCountUninitialized(present_modes_count);
    BT_VULKAN_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(VulkanRenderer::GetPhysicalDevice(), m_surface, &present_modes_count, *out_support.present_modes));
}

} // namespace Basalt
