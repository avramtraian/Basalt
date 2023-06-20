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

private:
    struct Attachment
    {
        ID3D11Texture2D* attachment = nullptr;
        ID3D11RenderTargetView* attachment_view = nullptr;
    };

    Array<Attachment> m_attachments;
    U32 m_width, m_height;
};

} // namespace Basalt
