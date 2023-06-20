// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "D3D11Framebuffer.h"

#include "D3D11Renderer.h"

namespace Basalt
{

namespace Utils
{

FORCEINLINE static DXGI_FORMAT FramebufferAttachmentTypeToDXGIFormat(EFramebufferAttachmentType type)
{
    switch (type)
    {
        case EFramebufferAttachmentType::R8G8B8A8:  return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        case EFramebufferAttachmentType::B8G8R8A8:  return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
    }

    Checkf(false, "Invalid EFramebufferAttachmentType!");
    return DXGI_FORMAT_UNKNOWN;
}

} // namespace Basalt::Utils

D3D11Framebuffer::D3D11Framebuffer(const FramebufferDescription& description)
    : m_width(description.width)
    , m_height(description.height)
{
    auto* device = D3D11Renderer::GetDevice();
    auto* device_context = D3D11Renderer::GetDeviceContext();

    D3D11_TEXTURE2D_DESC attachment_description = {};
    attachment_description.Width = m_width;
    attachment_description.Height = m_height;
    attachment_description.MipLevels = 1;
    attachment_description.ArraySize = 1;
    attachment_description.SampleDesc.Count = 1;
    attachment_description.SampleDesc.Quality = 0;
    attachment_description.Usage = D3D11_USAGE_DEFAULT;
    attachment_description.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    attachment_description.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA attachment_data = {};
    attachment_data.pSysMem = nullptr;

    D3D11_RENDER_TARGET_VIEW_DESC attachment_view_description = {};
    attachment_view_description.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    attachment_view_description.Texture2D.MipSlice = 0;

    for (auto& attachment : description.attachments)
    {
        attachment_description.Format = Utils::FramebufferAttachmentTypeToDXGIFormat(attachment.type);
        attachment_view_description.Format = attachment_description.Format;
        
        Attachment d3d11_attachment;
        BT_D3D11_CHECK(device->CreateTexture2D(&attachment_description, &attachment_data, &d3d11_attachment.attachment));
        BT_D3D11_CHECK(device->CreateRenderTargetView(d3d11_attachment.attachment, &attachment_view_description, &d3d11_attachment.attachment_view));

        m_attachments.Add(d3d11_attachment);
    }
}

D3D11Framebuffer::~D3D11Framebuffer()
{
    for (auto& attachment : m_attachments)
    {
        attachment.attachment->Release();
        attachment.attachment_view->Release();
    }
}

} // namespace Basalt
