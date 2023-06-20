// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/Core.h"

namespace Basalt
{

/**
 * Enumeration of all framebuffer attachment types.
 */
enum class EFramebufferAttachmentType
{
    None = 0,

    R8G8B8A8,
    B8G8R8A8,

    MaxEnumValue
};

/**
 * Structure that describes a framebuffer attachment.
 */
struct BASALT_S_API FramebufferAttachment
{
    /** The type of the framebuffer attachment. */
    EFramebufferAttachmentType type = EFramebufferAttachmentType::None;


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
};

} // namespace Basalt
