// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Renderer/Renderer.h"
#include "Vulkan.h"

namespace Basalt
{

class VulkanRenderer : public RendererInterface
{
public:
	virtual bool Initialize() override;
	virtual void Shutdown() override;

public:
	virtual void BeginRenderPass(Ref<RenderPass> render_pass) override;
	virtual void EndRenderPass(Ref<RenderPass> render_pass) override;

public:
	static VkAllocationCallbacks* GetAllocator();
	static VkInstance GetInstance();
	static VkPhysicalDevice GetPhysicalDevice();
	static VkDevice GetDevice();
	
	static VkQueue GetGraphicsQueue();
	static VkQueue GetPresentQueue();
	static VkQueue GetComputeQueue();
	static VkQueue GetTransferQueue();

	static U32 GetGraphicsQueueFamilyIndex();
	static U32 GetPresentQueueFamilyIndex();
	static U32 GetComputeQueueFamilyIndex();
	static U32 GetTransferQueueFamilyIndex();

private:
	static bool GetInstanceLayers(Array<const char*>& out_layers);
	static void GetInstanceExtensions(Array<const char*>& out_extensions);

	static VkInstance CreateInstance();

#if BT_VULKAN_VALIDATION
	static VkBool32 DebugMessengerCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT,
		VkDebugUtilsMessageTypeFlagsEXT,
		const VkDebugUtilsMessengerCallbackDataEXT*,
		void*
	);
#endif // BT_VULKAN_VALIDATION

	static VkDebugUtilsMessengerEXT CreateDebugMessenger();

private:
	struct PhysicalDevice
	{
		VkPhysicalDevice handle = VK_NULL_HANDLE;
		VkPhysicalDeviceProperties properties = {};
		VkPhysicalDeviceFeatures features = {};

		struct QueueFamilyIndices
		{
			static constexpr U32 InvalidQueueFamilyIndex = -1;

			U32 graphics_index	= InvalidQueueFamilyIndex;
			U32 present_index	= InvalidQueueFamilyIndex;
			U32 compute_index	= InvalidQueueFamilyIndex;
			U32 transfer_index	= InvalidQueueFamilyIndex;
		};

		QueueFamilyIndices queue_family_indices;
	};

	static void FindQueueFamilies(PhysicalDevice& physical_device);
	static bool ValidatePhysicalDevice(PhysicalDevice& physical_device);
	static PhysicalDevice SelectPhysicalDevice();

private:
	static bool GetDeviceExtensions(Array<const char*>& out_extensions);
	static VkDevice CreateLogicalDevice();
	static void GetDeviceQueues();

private:
	friend struct VulkanRendererData;
	friend class VulkanContext;
};

} // namespace Basalt
