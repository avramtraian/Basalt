// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/Core.h"
#include "RendererAPI.h"

namespace Basalt
{

/**
 * Enumeration of all framebuffer attachment types.
 */
enum class EFramebufferAttachmentFormat
{
    None = 0,

    // Color formats.
    R8G8B8A8,
    B8G8R8A8,

    // Depth-stencil formats.
    DEPTH16, DEPTH32, DEPTH24STENCIL8,

    MaxEnumValue
};

/**
 * Structure that describes a framebuffer attachment.
 */
struct BASALT_S_API FramebufferAttachment
{
    /** The type of the framebuffer attachment. */
    EFramebufferAttachmentFormat format = EFramebufferAttachmentFormat::None;
};

/**
 * Structure that describes a framebuffer.
 */
struct BASALT_S_API FramebufferDescription
{
    /** The width of the framebuffer. */
    U32 width = 1;
    
    /** The height of the framebuffer. */
    U32 height = 1;
    
    /** The list of the attachments the framebuffer contains. */
    Array<FramebufferAttachment> attachments;
};

class BASALT_API Framebuffer : public RefCounted
{
public:
    static Ref<Framebuffer> Create(const FramebufferDescription& description);

public:
    virtual ~Framebuffer() = default;

    virtual U32 GetAttachmentsCount() const = 0;
    virtual RendererID GetAttachment(U32 attachment_index) const = 0;
    virtual RendererID GetAttachmentView(U32 attachment_index) const = 0;
    virtual EFramebufferAttachmentFormat GetAttachmentFormat(U32 attachment_index) const = 0;
};

namespace Utils
{

FORCEINLINE bool IsDepthFormat(EFramebufferAttachmentFormat format)
{
    switch (format)
    {
        case EFramebufferAttachmentFormat::DEPTH16:         return true;
        case EFramebufferAttachmentFormat::DEPTH32:         return true;
        case EFramebufferAttachmentFormat::DEPTH24STENCIL8: return true;
    }

    return false;
}

FORCEINLINE bool IsStencilFormat(EFramebufferAttachmentFormat format)
{
    switch (format)
    {
        case EFramebufferAttachmentFormat::DEPTH24STENCIL8: return true;
    }

    return false;
}

} // namespace Basalt::Utils

} // namespace Basalt
