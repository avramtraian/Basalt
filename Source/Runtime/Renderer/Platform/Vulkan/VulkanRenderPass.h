// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "Renderer/RenderPass.h"
#include "Vulkan.h"

namespace Basalt
{

class VulkanRenderPass : public RenderPass
{
public:
    VulkanRenderPass(const RenderPassDescription& description);
    virtual ~VulkanRenderPass() override;

private:
    void CreateRenderPass();

private:
    RenderPassDescription m_description;
    VkRenderPass m_render_pass = VK_NULL_HANDLE;

    friend class VulkanFramebuffer;
};

} // namespace Basalt
