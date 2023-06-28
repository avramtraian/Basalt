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
};

} // namespace Basalt
