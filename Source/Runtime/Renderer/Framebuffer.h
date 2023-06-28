// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/Core.h"
#include "Image.h"
#include "RendererAPI.h"

namespace Basalt
{


/**
 * Structure that describes a framebuffer attachment.
 */
struct BASALT_S_API FramebufferAttachment
{
    FramebufferAttachment() = default;
    FramebufferAttachment(EImageFormat attachment_format)
        : format(attachment_format)
    {}

    /** The type of the framebuffer attachment. */
    EImageFormat format = EImageFormat::None;
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
    virtual EImageFormat GetAttachmentFormat(U32 attachment_index) const = 0;
};

namespace Utils
{

} // namespace Basalt::Utils

} // namespace Basalt
