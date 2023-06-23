// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "D3D11Renderer.h"

#include "D3D11Framebuffer.h"
#include "D3D11RenderPass.h"

namespace Basalt
{

/**
 * Structure that contains all the data that the D3D11 renderer manages and uses.
 */
struct D3D11RendererData
{
    IDXGIFactory* dxgi_factory = nullptr;

    IDXGIAdapter* adapter = nullptr;

    ID3D11Device* device = nullptr;

    ID3D11DeviceContext* device_context = nullptr;
};

/** Pointer to the D3D11 renderer data. */
static D3D11RendererData* s_d3d11_data = nullptr;

bool D3D11Renderer::Initialize()
{
    BT_LOG_TRACE(Renderer, "Initializing the D3D11 renderer...");

    // The renderer was already initialized.
    Check(!s_d3d11_data);

    s_d3d11_data = btnew D3D11RendererData();
    Check(s_d3d11_data);

    BT_D3D11_CHECK(CreateDXGIFactory(IID_PPV_ARGS(&s_d3d11_data->dxgi_factory)));

    s_d3d11_data->adapter = D3D11Renderer::PickBestAdapter();
    DXGI_ADAPTER_DESC adapter_desc = {};
    BT_D3D11_CHECK(s_d3d11_data->adapter->GetDesc(&adapter_desc));

    BT_LOG_INFO(Renderer, "Selected GPU: '%ws'", adapter_desc.Description);

    UINT device_flags = 0;
#if !BASALT_BUILD_SHIPPING
    device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // !BASALT_BUILD_SHIPPING
    
    D3D_FEATURE_LEVEL feature_levels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    D3D_FEATURE_LEVEL feature_level;

    BT_D3D11_CHECK(D3D11CreateDevice(
        s_d3d11_data->adapter, D3D_DRIVER_TYPE_UNKNOWN, NULL,
        device_flags, feature_levels, BT_ARRAY_COUNT(feature_levels),
        D3D11_SDK_VERSION, &s_d3d11_data->device, &feature_level, &s_d3d11_data->device_context));

    BT_LOG_INFO(Renderer, "The D3D11 renderer was initialized successfully!");
    return true;
}

void D3D11Renderer::Shutdown()
{
    // The renderer was already shut down or was never initialized.
    Check(s_d3d11_data);

    s_d3d11_data->device->Release();
    s_d3d11_data->device_context->Release();

    s_d3d11_data->adapter->Release();
    s_d3d11_data->dxgi_factory->Release();

    btdelete s_d3d11_data;
    s_d3d11_data = nullptr;
}

void D3D11Renderer::BeginRenderPass(Ref<RenderPass> render_pass)
{
    auto d3d11_render_pass = render_pass.As<D3D11RenderPass>();
    RenderPassDescription& render_pass_description = d3d11_render_pass->GetDescription();

    U32 framebuffer_attachments_count = render_pass_description.output_framebuffer->GetAttachmentsCount();

    Array<ID3D11RenderTargetView*> color_render_target_views;
    color_render_target_views.SetCapacity(framebuffer_attachments_count);
    ID3D11DepthStencilView* depth_stencil_view = nullptr;

    for (U32 attachment_index = 0; attachment_index < framebuffer_attachments_count; ++attachment_index)
    {
        EFramebufferAttachmentFormat attachment_format = render_pass_description.output_framebuffer->GetAttachmentFormat(attachment_index);
        RendererID attachment_view = render_pass_description.output_framebuffer->GetAttachmentView(attachment_index);
        RenderPassAttachmentDescription attachment_description = render_pass_description.attachment_descriptions[attachment_index];

        if (!Utils::IsDepthFormat(attachment_format))
        {
            color_render_target_views.Add((ID3D11RenderTargetView*)attachment_view);

            if (attachment_description.load_operation == ERenderPassLoadOperation::Clear)
            {
                s_d3d11_data->device_context->ClearRenderTargetView((ID3D11RenderTargetView*)attachment_view, attachment_description.clear_value);
            }
        }
        else
        {
            Checkf(!depth_stencil_view, "The output framebuffer has two depth attachments!");
            depth_stencil_view = (ID3D11DepthStencilView*)attachment_view;

            if (attachment_description.load_operation == ERenderPassLoadOperation::Clear)
            {
                UINT clear_flags = D3D11_CLEAR_DEPTH | (Utils::IsStencilFormat(attachment_format) ? D3D11_CLEAR_STENCIL : 0);
                s_d3d11_data->device_context->ClearDepthStencilView(depth_stencil_view, clear_flags, attachment_description.depth_value, attachment_description.stencil_value);
            }
        }
    }

    // Bind the output framebuffer attachments.
    s_d3d11_data->device_context->OMSetRenderTargets((UINT)color_render_target_views.Count(), color_render_target_views.Data(), depth_stencil_view);
}

void D3D11Renderer::EndRenderPass(Ref<RenderPass> render_pass)
{
    // Unbind the output framebuffer attachments.
    s_d3d11_data->device_context->OMSetRenderTargets(0, nullptr, nullptr);
}

ID3D11Device* D3D11Renderer::GetDevice()
{
    return s_d3d11_data->device;
}

ID3D11DeviceContext* D3D11Renderer::GetDeviceContext()
{
    return s_d3d11_data->device_context;
}

IDXGIFactory* D3D11Renderer::GetDXGIFactory()
{
    return s_d3d11_data->dxgi_factory;
}

IDXGIAdapter* D3D11Renderer::GetAdapter()
{
    return s_d3d11_data->adapter;
}

IDXGIAdapter* D3D11Renderer::PickBestAdapter()
{
    IDXGIAdapter* best_adapter = nullptr;

    Array<IDXGIAdapter*> adapters;
    adapters.SetCapacity(2);

    BT_LOG_TRACE(Renderer, "Available GPU(s):");

    UINT iterator = 0;
    while (true)
    {
        IDXGIAdapter* adapter;
        if (s_d3d11_data->dxgi_factory->EnumAdapters(iterator++, &adapter) == DXGI_ERROR_NOT_FOUND)
        {
            break;
        }

        DXGI_ADAPTER_DESC adapter_description = {};
        adapter->GetDesc(&adapter_description);

        BT_LOG_TRACE(Renderer, "  |- %ws", adapter_description.Description);

        adapters.Add(adapter);
    }

    for (IDXGIAdapter* adapter : adapters)
    {
        // NOTE(traian): For now, we pick the first adapter in the enumeration list, but
        //               we should use a better algorithm, based on adapter performance.
        best_adapter = adapter;
        break;
    }

    for (IDXGIAdapter* adapter : adapters)
    {
        if (adapter != best_adapter)
        {
            adapter->Release();
        }
    }

    Checkf(best_adapter, "No GPU available!");
    return best_adapter;
}

} // namespace Basalt
