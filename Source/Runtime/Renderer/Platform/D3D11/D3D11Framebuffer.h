// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Renderer/Framebuffer.h"

#include <d3d11.h>

namespace Basalt
{

class D3D11Framebuffer : public Framebuffer
{
public:
    D3D11Framebuffer(const FramebufferDescription& description);
    virtual ~D3D11Framebuffer() override;

    virtual U32 GetAttachmentsCount() const override { return (U32)m_attachments.Count(); }

    virtual RendererID GetAttachment(U32 attachment_index) const override;
    virtual RendererID GetAttachmentView(U32 attachment_index) const override;
    virtual EFramebufferAttachmentFormat GetAttachmentFormat(U32 attachment_index) const override;

private:
    struct Attachment
    {
        EFramebufferAttachmentFormat format = EFramebufferAttachmentFormat::None;
        ID3D11Texture2D* attachment = nullptr;
        // Depending on the attachment format, might be a render target view or a depth-stencil view.
        RendererID attachment_view = nullptr;
    };

    U32 m_width, m_height;
    Array<Attachment> m_attachments;
};

} // namespace Basalt
