// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Renderer/Image.h"

#include <vulkan/vulkan.h>

#if BASALT_BUILD_DEBUG
    #define BT_VULKAN_VALIDATION    1
#endif // BASALT_BUILD_DEBUG

#if BASALT_BUILD_RELEASE
    #define BT_VULKAN_VALIDATION    1
#endif // BASALT_BUILD_DEBUG

#if BASALT_BUILD_SHIPPING
    #define BT_VULKAN_VALIDATION    0
#endif // BASALT_BUILD_DEBUG

#define INVALID_VULKAN_CALL false

#if BT_VULKAN_VALIDATION
    #define BT_VULKAN_CHECK(EXPRESSION)                 \
        if ((EXPRESSION) != VK_SUCCESS)                 \
        {                                               \
            Checkf(INVALID_VULKAN_CALL, #EXPRESSION);   \
        }
#else
    #define BT_VULKAN_CHECK(EXPRESSION) EXPRESSION
#endif // BT_VULKAN_VALIDATION

namespace Basalt
{
namespace Utils
{

FORCEINLINE VkFormat ImageFormatToVulkan(EImageFormat format)
{
    switch (format)
    {
        case EImageFormat::R32G32B32A32: return VK_FORMAT_R32G32B32A32_SFLOAT;
    }

    Checkf(false, "Invalid EImageFormat!");
    return VK_FORMAT_UNDEFINED;
}

} // namespace Basalt::Utils
} // namespace Basalt
