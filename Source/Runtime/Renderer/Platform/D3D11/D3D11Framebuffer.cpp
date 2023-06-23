// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "D3D11Framebuffer.h"

#include "D3D11Renderer.h"

namespace Basalt
{

namespace Utils
{

FORCEINLINE static DXGI_FORMAT FramebufferAttachmentTypeToDXGIFormat(EFramebufferAttachmentFormat format)
{
    switch (format)
    {
        // Color attachments.
        case EFramebufferAttachmentFormat::R8G8B8A8:        return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        case EFramebufferAttachmentFormat::B8G8R8A8:        return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;

        // Depth-stencil attachments.
        case EFramebufferAttachmentFormat::DEPTH16:         return DXGI_FORMAT_D16_UNORM;
        case EFramebufferAttachmentFormat::DEPTH32:         return DXGI_FORMAT_D32_FLOAT;
        case EFramebufferAttachmentFormat::DEPTH24STENCIL8: return DXGI_FORMAT_D24_UNORM_S8_UINT;
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

    for (auto& attachment : description.attachments)
    {
        attachment_description.Format = Utils::FramebufferAttachmentTypeToDXGIFormat(attachment.format);

        Attachment d3d11_attachment;
        d3d11_attachment.format = attachment.format;
        BT_D3D11_CHECK(device->CreateTexture2D(&attachment_description, &attachment_data, &d3d11_attachment.attachment));

        if (!Utils::IsDepthFormat(attachment.format))
        {
            D3D11_RENDER_TARGET_VIEW_DESC view_description = {};
            view_description.Format = attachment_description.Format;
            view_description.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            view_description.Texture2D.MipSlice = 0;

            ID3D11RenderTargetView* view = nullptr;
            BT_D3D11_CHECK(device->CreateRenderTargetView(d3d11_attachment.attachment, &view_description, &view));
            d3d11_attachment.attachment_view = (RendererID)view;
        }
        else
        {
            D3D11_DEPTH_STENCIL_VIEW_DESC view_description = {};
            view_description.Format = attachment_description.Format;
            view_description.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
            view_description.Texture2D.MipSlice = 0;

            ID3D11DepthStencilView* view = nullptr;
            BT_D3D11_CHECK(device->CreateDepthStencilView(d3d11_attachment.attachment, &view_description, &view));
            d3d11_attachment.attachment_view = (RendererID)view;
        }

        m_attachments.Add(d3d11_attachment);
    }
}

D3D11Framebuffer::~D3D11Framebuffer()
{
    for (auto& attachment : m_attachments)
    {
        if (!Utils::IsDepthFormat(attachment.format))
        {
            ID3D11DepthStencilView* view = (ID3D11DepthStencilView*)attachment.attachment_view;
            view->Release();
        }
        else
        {
            ID3D11RenderTargetView* view = (ID3D11RenderTargetView*)attachment.attachment_view;
            view->Release();
        }

        attachment.attachment->Release();
    }
}

RendererID D3D11Framebuffer::GetAttachment(U32 attachment_index) const
{
    Checkf(attachment_index < m_attachments.Count(), "Framebuffer attachment index is out of range: %d out of %d.", attachment_index, GetAttachmentsCount());
    return m_attachments[attachment_index].attachment;
}

RendererID D3D11Framebuffer::GetAttachmentView(U32 attachment_index) const
{
    Checkf(attachment_index < m_attachments.Count(), "Framebuffer attachment index is out of range: %d out of %d.", attachment_index, GetAttachmentsCount());
    return m_attachments[attachment_index].attachment_view;
}

EFramebufferAttachmentFormat D3D11Framebuffer::GetAttachmentFormat(U32 attachment_index) const
{
    Checkf(attachment_index < m_attachments.Count(), "Framebuffer attachment index is out of range: %d out of %d.", attachment_index, GetAttachmentsCount());
    return m_attachments[attachment_index].format;
}

} // namespace Basalt
