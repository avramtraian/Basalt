// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "D3D11RenderPass.h"

#include "D3D11Renderer.h"

namespace Basalt
{

D3D11RenderPass::D3D11RenderPass(const RenderPassDescription& description)
    : m_description(description)
{
}

D3D11RenderPass::~D3D11RenderPass()
{
}   

void D3D11RenderPass::Begin()
{
    auto* device_context = D3D11Renderer::GetDeviceContext();

    U32 framebuffer_attachments_count = m_description.output_framebuffer->GetAttachmentsCount();

    Array<ID3D11RenderTargetView*> color_render_target_views;
    color_render_target_views.SetCapacity(framebuffer_attachments_count);
    ID3D11DepthStencilView* depth_stencil_view = nullptr;

    for (U32 attachment_index = 0; attachment_index < framebuffer_attachments_count; ++attachment_index)
    {
        EFramebufferAttachmentFormat attachment_format = m_description.output_framebuffer->GetAttachmentFormat(attachment_index);
        RendererID attachment_view = m_description.output_framebuffer->GetAttachmentView(attachment_index);
        RenderPassAttachmentDescription attachment_description = m_description.attachment_descriptions[attachment_index];

        if (!Utils::IsDepthFormat(attachment_format))
        {
            color_render_target_views.Add((ID3D11RenderTargetView*)attachment_view);

            if (attachment_description.load_operation == ERenderPassLoadOperation::Clear)
            {
                device_context->ClearRenderTargetView((ID3D11RenderTargetView*)attachment_view, attachment_description.clear_value);
            }
        }
        else
        {
            Checkf(!depth_stencil_view, "The output framebuffer has two depth attachments!");
            depth_stencil_view = (ID3D11DepthStencilView*)attachment_view;

            if (attachment_description.load_operation == ERenderPassLoadOperation::Clear)
            {
                UINT clear_flags = D3D11_CLEAR_DEPTH | (Utils::IsStencilFormat(attachment_format) ? D3D11_CLEAR_STENCIL : 0);
                device_context->ClearDepthStencilView(depth_stencil_view, clear_flags, attachment_description.depth_value, attachment_description.stencil_value);
            }
        }
    }

    // Bind the output framebuffer attachments.
    device_context->OMSetRenderTargets((UINT)color_render_target_views.Count(), color_render_target_views.Data(), depth_stencil_view);
}

void D3D11RenderPass::End()
{
    auto* device_context = D3D11Renderer::GetDeviceContext();

    // Unbind the output framebuffer attachments.
    device_context->OMSetRenderTargets(0, nullptr, nullptr);
}

} // namespace Basalt
