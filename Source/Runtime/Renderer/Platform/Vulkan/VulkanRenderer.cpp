// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "VulkanRenderer.h"

#include "Core/Containers/Strings/Utf8String.h"

namespace Basalt
{

/**
 * Structure that contains all the data that the Vulkan renderer manages and uses.
 */
struct VulkanRendererData
{
    VkAllocationCallbacks allocator = {};

    VkInstance instance = VK_NULL_HANDLE;

    VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;

    VulkanRenderer::PhysicalDevice physical_device = {};

    VkDevice device = VK_NULL_HANDLE;

    VkQueue graphics_queue = VK_NULL_HANDLE;
    VkQueue present_queue  = VK_NULL_HANDLE;
    VkQueue compute_queue  = VK_NULL_HANDLE;
    VkQueue transfer_queue = VK_NULL_HANDLE;
};

/** Pointer to the Vulkan renderer data. */
static VulkanRendererData* s_vulkan_data = nullptr;

bool VulkanRenderer::Initialize()
{
    BT_LOG_TRACE(Renderer, "Initializing the Vulkan renderer...");

    Checkf(!s_vulkan_data, "The Vulkan renderer was already initialized!");
    s_vulkan_data = btnew VulkanRendererData();
    Check(s_vulkan_data);

    s_vulkan_data->instance = VulkanRenderer::CreateInstance();
    if (s_vulkan_data->instance == VK_NULL_HANDLE)
    {
        // Failed to create the Vulkan instance.
        return false;
    }

    s_vulkan_data->physical_device = VulkanRenderer::SelectPhysicalDevice();
    if (s_vulkan_data->physical_device.handle == VK_NULL_HANDLE)
    {
        // Failed to create the Vulkan physical device.
        return false;
    }

#if BT_VULKAN_VALIDATION
    s_vulkan_data->debug_messenger = VulkanRenderer::CreateDebugMessenger();
    if (s_vulkan_data->debug_messenger == VK_NULL_HANDLE)
    {
        // Failed to create the Vulkan debug messenger.
        return false;
    }
#endif // BT_VULKAN_VALIDATION

    s_vulkan_data->device = VulkanRenderer::CreateLogicalDevice();
    if (s_vulkan_data->device == VK_NULL_HANDLE)
    {
        // Failed to create the Vulkan logical device.
        return false;
    }

    // Query the device queues handles.
    VulkanRenderer::GetDeviceQueues();

    BT_LOG_INFO(Renderer, "The Vulkan renderer was initialized successfully!");
    return true;
}

void VulkanRenderer::Shutdown()
{
    Checkf(s_vulkan_data, "The Vulkan renderer was already shut down!");
    
    // Destroying the Vulkan logical device.
    vkDestroyDevice(s_vulkan_data->device, GetAllocator());
    s_vulkan_data->device = VK_NULL_HANDLE;

#if BT_VULKAN_VALIDATION
    PFN_vkDestroyDebugUtilsMessengerEXT destroy_debug_messenger =
        (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(s_vulkan_data->instance, "vkDestroyDebugUtilsMessengerEXT");
    
    if (destroy_debug_messenger == VK_NULL_HANDLE)
    {
        BT_LOG_ERROR(Renderer, "Failed to obtain the Vulkan debug messenger destroy function!");
    }
    else
    {
        // Destroying the Vulkan debug messenger.
        destroy_debug_messenger(s_vulkan_data->instance, s_vulkan_data->debug_messenger, GetAllocator());
        s_vulkan_data->debug_messenger = VK_NULL_HANDLE;
    }
#endif // BT_VULKAN_VALIDATION

    // Destroying the Vulkan instance.
    vkDestroyInstance(s_vulkan_data->instance, GetAllocator());
    s_vulkan_data->instance = VK_NULL_HANDLE;

    btdelete s_vulkan_data;
    s_vulkan_data = nullptr;
}

void VulkanRenderer::BeginRenderPass(Ref<RenderPass> render_pass)
{
}

void VulkanRenderer::EndRenderPass(Ref<RenderPass> render_pass)
{
}

VkAllocationCallbacks* VulkanRenderer::GetAllocator()
{
    // TODO(traian): Use an actual Vulkan memory allocator.
    return nullptr;
    return &s_vulkan_data->allocator;
}

VkInstance VulkanRenderer::GetInstance()
{
    return s_vulkan_data->instance;
}

VkPhysicalDevice VulkanRenderer::GetPhysicalDevice()
{
    return s_vulkan_data->physical_device.handle;
}

VkDevice VulkanRenderer::GetDevice()
{
    return s_vulkan_data->device;
}

VkQueue VulkanRenderer::GetGraphicsQueue()
{
    return s_vulkan_data->graphics_queue;
}

VkQueue VulkanRenderer::GetPresentQueue()
{
    return s_vulkan_data->present_queue;
}

VkQueue VulkanRenderer::GetComputeQueue()
{
    return s_vulkan_data->compute_queue;
}

VkQueue VulkanRenderer::GetTransferQueue()
{
    return s_vulkan_data->transfer_queue;
}

U32 VulkanRenderer::GetGraphicsQueueFamilyIndex()
{
    return s_vulkan_data->physical_device.queue_family_indices.graphics_index;
}

U32 VulkanRenderer::GetPresentQueueFamilyIndex()
{
    return s_vulkan_data->physical_device.queue_family_indices.present_index;
}

U32 VulkanRenderer::GetComputeQueueFamilyIndex()
{
    return s_vulkan_data->physical_device.queue_family_indices.compute_index;
}

U32 VulkanRenderer::GetTransferQueueFamilyIndex()
{
    return s_vulkan_data->physical_device.queue_family_indices.transfer_index;
}

bool VulkanRenderer::GetInstanceLayers(Array<const char*>& out_layers)
{
    U32 available_layers_count = 0;
    BT_VULKAN_CHECK(vkEnumerateInstanceLayerProperties(&available_layers_count, nullptr));
    Array<VkLayerProperties> available_layers;
    available_layers.SetCountUninitialized(available_layers_count);
    BT_VULKAN_CHECK(vkEnumerateInstanceLayerProperties(&available_layers_count, *available_layers));

#if BT_VULKAN_VALIDATION
    out_layers.Add("VK_LAYER_KHRONOS_validation");
#endif // BT_VULKAN_VALIDATION

    BT_LOG_TRACE(Renderer, "Searching for instance layers:");
    for (const char* layer : out_layers)
    {
        bool found = false;
        for (auto& available_layer : available_layers)
        {
            if (UTF8Calls::Equals(layer, available_layer.layerName))
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            BT_LOG_ERROR(Renderer, "Required instance layer not found: '%s'", layer);
            return false;
        }

        BT_LOG_TRACE(Renderer, "  |- %s : found", layer);
    }

    // All required instance layers are present.
    return true;
}

void VulkanRenderer::GetInstanceExtensions(Array<const char*>& out_extensions)
{
    // Platform-generic surface extensions.
    out_extensions.Add(VK_KHR_SURFACE_EXTENSION_NAME);

#if BASALT_PLATFORM_WINDOWS
    // Creates a surface specific for Win32 applications.
    out_extensions.Add("VK_KHR_win32_surface");
#endif // BASALT_PLATFORM_WINDOWS

#if BT_VULKAN_VALIDATION
    // Enables the debug messenger.
    out_extensions.Add(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    BT_LOG_TRACE(Renderer, "Required instance extensions:");
    for (const char* extension : out_extensions)
    {
        BT_LOG_TRACE(Renderer, "  |- %s", extension);
    }
}

VkInstance VulkanRenderer::CreateInstance()
{
    VkApplicationInfo application_info = {};
    application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    application_info.apiVersion = VK_API_VERSION_1_3;
    application_info.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    application_info.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    application_info.pApplicationName = "BasaltGame";
    application_info.pEngineName = "Basalt";

    VkInstanceCreateInfo instance_info = {};
    instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_info.pApplicationInfo = &application_info;

    Array<const char*> instance_layers;
    VulkanRenderer::GetInstanceLayers(instance_layers);

    instance_info.enabledLayerCount = (U32)instance_layers.Count();
    instance_info.ppEnabledLayerNames = *instance_layers;

    Array<const char*> instance_extensions;
    VulkanRenderer::GetInstanceExtensions(instance_extensions);

    instance_info.enabledExtensionCount = (U32)instance_extensions.Count();
    instance_info.ppEnabledExtensionNames = *instance_extensions;

    // Create the Vulkan instance.
    VkInstance instance = VK_NULL_HANDLE;
    BT_VULKAN_CHECK(vkCreateInstance(&instance_info, GetAllocator(), &instance));

    return instance;
}

#if BT_VULKAN_VALIDATION
VkBool32 VulkanRenderer::DebugMessengerCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data
)
{
    switch (severity)
    {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            // BT_LOG_TRACE(Renderer, "{}", (const char*)callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            // BT_LOG_INFO(Renderer, "{}", (const char*)callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            BT_LOG_WARN(Renderer, "%s", (const char*)callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            BT_LOG_ERROR(Renderer, "%s", (const char*)callback_data->pMessage);
            break;
    }

    return VK_FALSE;
}

VkDebugUtilsMessengerEXT VulkanRenderer::CreateDebugMessenger()
{
    VkDebugUtilsMessengerCreateInfoEXT debugger_create_info = {};
    debugger_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugger_create_info.pfnUserCallback = VulkanRenderer::DebugMessengerCallback;

    debugger_create_info.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT   |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;

    debugger_create_info.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT     |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;

    PFN_vkCreateDebugUtilsMessengerEXT create_debug_messenger = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(s_vulkan_data->instance, "vkCreateDebugUtilsMessengerEXT");
    if (create_debug_messenger == VK_NULL_HANDLE)
    {
        BT_LOG_ERROR(Renderer, "Failed to obtain the Vulkan debug messenger creation function!");
        return VK_NULL_HANDLE;
    }

    VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;
    BT_VULKAN_CHECK(create_debug_messenger(s_vulkan_data->instance, &debugger_create_info, nullptr, &debug_messenger));
    return debug_messenger;
}
#endif // BT_VULKAN_VALIDATION

void VulkanRenderer::FindQueueFamilies(PhysicalDevice& physical_device)
{
    U32 queue_families_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device.handle, &queue_families_count, nullptr);
    Array<VkQueueFamilyProperties> queue_families;
    queue_families.SetCountUninitialized(queue_families_count);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device.handle, &queue_families_count, *queue_families);

    for (U32 queue_family_index = 0; queue_family_index < queue_families.Count(); ++queue_family_index)
    {
        const VkQueueFamilyProperties& properties = queue_families[queue_family_index];
        if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            physical_device.queue_family_indices.graphics_index = queue_family_index;
            // NOTE(traian): The official documentation doesn't state that queues that support graphics operation also support
            //               presenting, but that's almost always the case on "normal" hardware.
            physical_device.queue_family_indices.present_index = queue_family_index;
        }
        if (properties.queueFlags & VK_QUEUE_COMPUTE_BIT)
        {
            physical_device.queue_family_indices.compute_index = queue_family_index;
        }
        if (properties.queueFlags & VK_QUEUE_TRANSFER_BIT)
        {
            physical_device.queue_family_indices.transfer_index = queue_family_index;
        }
    }
}

bool VulkanRenderer::ValidatePhysicalDevice(PhysicalDevice& physical_device)
{
    // Obtain the queue family indices for the physical device to validate.
    // The physical device structure stores the queue family indices, and they will be
    // filled during this function execution.
    FindQueueFamilies(physical_device);
    
    if (physical_device.queue_family_indices.graphics_index == PhysicalDevice::QueueFamilyIndices::InvalidQueueFamilyIndex)
    {
        // If the physical device doesn't have a graphics queue family, the GPU is not usable.
        return false;
    }
    if (physical_device.queue_family_indices.present_index == PhysicalDevice::QueueFamilyIndices::InvalidQueueFamilyIndex)
    {
        // If the physical device doesn't have a present queue family, the GPU is not usable.
        return false;
    }
    if (physical_device.queue_family_indices.compute_index == PhysicalDevice::QueueFamilyIndices::InvalidQueueFamilyIndex)
    {
        // If the physical device doesn't have a compute queue family, the GPU is not usable.
        return false;
    }
    if (physical_device.queue_family_indices.transfer_index == PhysicalDevice::QueueFamilyIndices::InvalidQueueFamilyIndex)
    {
        // If the physical device doesn't have a transfer queue family, the GPU is not usable.
        return false;
    }

    // The provided physical device meets all the requirements and represents a possible candidate.
    return true;
}

VulkanRenderer::PhysicalDevice VulkanRenderer::SelectPhysicalDevice()
{
    U32 physical_devices_count = 0;
    BT_VULKAN_CHECK(vkEnumeratePhysicalDevices(GetInstance(), &physical_devices_count, nullptr));
    Array<VkPhysicalDevice> physical_devices;
    physical_devices.SetCountUninitialized(physical_devices_count);
    BT_VULKAN_CHECK(vkEnumeratePhysicalDevices(GetInstance(), &physical_devices_count, *physical_devices));

    Array<PhysicalDevice> gpus;
    gpus.SetCapacity(physical_devices.Count());

    for (VkPhysicalDevice physical_device : physical_devices)
    {
        VkPhysicalDeviceProperties properties = {};
        vkGetPhysicalDeviceProperties(physical_device, &properties);
        gpus.Add({ physical_device, properties });
    }

    BT_LOG_INFO(Renderer, "Available Vulkan GPUs:");
    for (PhysicalDevice& gpu : gpus)
    {
        BT_LOG_INFO(Renderer, "  |- %s", gpu.properties.deviceName);
    }

    Usize selected_gpu_index = InvalidSize;
    for (Usize index = 0; index < gpus.Count(); ++index)
    {
        if (ValidatePhysicalDevice(gpus[index]))
        {
            if (gpus[index].properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU || selected_gpu_index == InvalidSize)
            {
                selected_gpu_index = index;
            }
        }
    }

    Checkf(selected_gpu_index != InvalidSize, "No GPU supports all required Vulkan features!");
    BT_LOG_INFO(Renderer, "Selected GPU: '%s'", gpus[selected_gpu_index].properties.deviceName);
    return gpus[selected_gpu_index];
}

bool VulkanRenderer::GetDeviceExtensions(Array<const char*>& out_extensions)
{
    U32 available_extensions_count = 0;
    BT_VULKAN_CHECK(vkEnumerateDeviceExtensionProperties(s_vulkan_data->physical_device.handle, nullptr, &available_extensions_count, nullptr));
    Array<VkExtensionProperties> available_extensions;
    available_extensions.SetCountUninitialized(available_extensions_count);
    BT_VULKAN_CHECK(vkEnumerateDeviceExtensionProperties(s_vulkan_data->physical_device.handle, nullptr, &available_extensions_count, *available_extensions));

    // Extension required to create/manipulate a Vulkan swapchain.
    out_extensions.Add(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    BT_LOG_TRACE(Renderer, "Searching for device extensions:");
    for (const char* extension : out_extensions)
    {
        bool found = false;
        for (auto& available_extension : available_extensions)
        {
            if (UTF8Calls::Equals(extension, available_extension.extensionName))
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            BT_LOG_ERROR(Renderer, "Required device extension not found: '%s'", extension);
            return false;
        }

        BT_LOG_TRACE(Renderer, "  |- %s : found", extension);
    }

    // All required device extensions are present.
    return true;
}

VkDevice VulkanRenderer::CreateLogicalDevice()
{
    // NOTE(traian): The physical device selection process guarantees that all queue families are valid.
    U32 queue_family_indices[] =
    {
        s_vulkan_data->physical_device.queue_family_indices.graphics_index,
        s_vulkan_data->physical_device.queue_family_indices.present_index,
        s_vulkan_data->physical_device.queue_family_indices.compute_index,
        s_vulkan_data->physical_device.queue_family_indices.transfer_index,
    };

    VkDeviceQueueCreateInfo queue_create_infos[3] = {};
    U32 queue_create_infos_count = 0;
    
    for (U32 index = 0; index < BT_ARRAY_COUNT(queue_family_indices); ++index)
    {
        U32 queue_family_index = queue_family_indices[index];

        bool already_exists = false;
        for (U32 jndex = 0; jndex < queue_create_infos_count; ++jndex)
        {
            if (queue_create_infos[jndex].queueFamilyIndex == queue_family_index)
            {
                already_exists = true;
                break;
            }
        }

        if (!already_exists)
        {
            VkDeviceQueueCreateInfo& queue_create_info = queue_create_infos[queue_create_infos_count++];
            queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_info.queueFamilyIndex = queue_family_index;
            queue_create_info.queueCount = 1;
            float queue_priority = 1.0F;
            queue_create_info.pQueuePriorities = &queue_priority;
        }
    }

    VkDeviceCreateInfo device_create_info = {};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    Array<const char*> device_extensions;
    VulkanRenderer::GetDeviceExtensions(device_extensions);

    device_create_info.enabledExtensionCount = (U32)device_extensions.Count();
    device_create_info.ppEnabledExtensionNames = *device_extensions;

    device_create_info.queueCreateInfoCount = (U32)queue_create_infos_count;
    device_create_info.pQueueCreateInfos = queue_create_infos;

    VkPhysicalDeviceFeatures device_features = {};
    device_create_info.pEnabledFeatures = &device_features;

    VkDevice device = VK_NULL_HANDLE;
    BT_VULKAN_CHECK(vkCreateDevice(s_vulkan_data->physical_device.handle, &device_create_info, GetAllocator(), &device));
    return device;
}

void VulkanRenderer::GetDeviceQueues()
{
    auto& queue_family_indices = s_vulkan_data->physical_device.queue_family_indices;
    vkGetDeviceQueue(s_vulkan_data->device, queue_family_indices.graphics_index, 0, &s_vulkan_data->graphics_queue);
    vkGetDeviceQueue(s_vulkan_data->device, queue_family_indices.graphics_index, 0, &s_vulkan_data->present_queue);
    vkGetDeviceQueue(s_vulkan_data->device, queue_family_indices.compute_index, 0, &s_vulkan_data->compute_queue);
    vkGetDeviceQueue(s_vulkan_data->device, queue_family_indices.transfer_index, 0, &s_vulkan_data->transfer_queue);
}

} // namespace Basalt
