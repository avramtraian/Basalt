// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/Core.h"
#include "Renderer/Framebuffer.h"

namespace Basalt
{

enum class ERenderPassLoadOperation
{
    None = 0,
    Discard,
    Preserve,
    Clear,
    MaxEnumValue
};

enum class ERenderPassStoreOperation
{
    None = 0,
    Discard,
    Preserve,
    MaxEnumValue
};

struct BASALT_S_API RenderPassAttachment
{
    ERenderPassLoadOperation load_operation = ERenderPassLoadOperation::Clear;
    ERenderPassStoreOperation store_operation = ERenderPassStoreOperation::Preserve;

    union
    {
        // Color attachments.
        float clear_value[4] = { 0.0F, 0.0F, 0.0F, 0.0F };

        // Depth-stencil attachments.
        struct
        {
            float depth_value;
            U8 stencil_value;
        };
    };
};

struct BASALT_S_API RenderPassDescription
{
    Ref<Framebuffer> target_framebuffer;
    Array<RenderPassAttachment> attachment_descriptions;
};

class BASALT_API RenderPass : public RefCounted
{
public:
    static Ref<RenderPass> Create(const RenderPassDescription& description);

public:
    virtual ~RenderPass() = default;
};

} // namespace Basalt
