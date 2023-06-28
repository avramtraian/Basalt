// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "Renderer.h"

#include "Platform/D3D11/D3D11Renderer.h"
#include "Platform/Vulkan/VulkanRenderer.h"

#include "RenderingContext.h"

namespace Basalt
{

RendererInterface* Renderer::s_interface = nullptr;

struct RendererData
{
    ERendererAPI renderer_api;

    Unique<RenderingContext> primary_context;

    RenderingContext* active_context;
};

static RendererData* s_renderer_data = nullptr;

bool Renderer::Initialize(const RendererDescription& description)
{
    BT_LOG_TRACE(Renderer, "Initializing the rendering system...");

    if (IsInitialized())
    {
        BT_LOG_ERROR(Renderer, "The renderer was already initialized! Aborting.");
        return false;
    }

    s_renderer_data = btnew RendererData();
    if (!s_renderer_data)
    {
        BT_LOG_ERROR(Renderer, "Failed to allocate memory for the renderer data! Aborting.");
        return false;
    }

    s_renderer_data->renderer_api = description.renderer_api;
    
    switch (s_renderer_data->renderer_api)
    {
        case ERendererAPI::D3D11:   { s_interface = btnew D3D11Renderer();  break; }
        case ERendererAPI::Vulkan:  { s_interface = btnew VulkanRenderer(); break; }
    }

    if (!s_interface)
    {
        BT_LOG_ERROR(Renderer, "Failed to instantiate the renderer interface! Aborting.");
        return false;
    }

    if (!s_interface->Initialize())
    {
        BT_LOG_ERROR(Renderer, "Failed to initialize the renderer interface! Aborting.");
        return false;
    }

    RenderingContextDescription context_description = {};
    context_description.window = description.primary_window;

    s_renderer_data->primary_context = RenderingContext::Create(context_description);
    s_renderer_data->active_context = s_renderer_data->primary_context.Get();

    // At this point, the renderer must be in a valid state.
    Check(IsInitialized());

    BT_LOG_INFO(Renderer, "The rendering system was initialized successfully!");
    return true;
}

void Renderer::Shutdown()
{
    if (!IsInitialized())
    {
        BT_LOG_ERROR(Renderer, "The renderer was already shut down! Aborting.");
        return;
    }

    s_interface->Shutdown();

    btdelete s_interface;
    s_interface = nullptr;

    btdelete s_renderer_data;
    s_renderer_data = nullptr;
}

bool Renderer::IsInitialized()
{
    return s_renderer_data && s_interface;
}

ERendererAPI Renderer::GetRendererAPI()
{
    return s_renderer_data->renderer_api;
}

RenderingContext* Renderer::GetActiveContext()
{
    return s_renderer_data->active_context;
}

void Renderer::SetActiveContext(RenderingContext* new_context)
{
    if (new_context)
    {
        s_renderer_data->active_context = new_context;
    }
    else
    {
        s_renderer_data->active_context = s_renderer_data->primary_context.Get();
    }
}

void Renderer::BeginRenderPass(Ref<RenderPass> render_pass)
{
    s_interface->BeginRenderPass(render_pass);
}

void Renderer::EndRenderPass(Ref<RenderPass> render_pass)
{
    s_interface->EndRenderPass(render_pass);
}

} // namespace Basalt
