// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "Renderer.h"

#include "Platform/D3D11/D3D11Renderer.h"
// #include "Platform/D3D12/D3D12Renderer.h"
// #include "Platform/Vulkan/VulkanRenderer.h"
// #include "Platform/Metal/MetalRenderer.h"

namespace Basalt
{

RendererInterface* Renderer::s_interface = nullptr;

struct RendererData
{
    ERendererAPI renderer_api;
};

static RendererData* s_data = nullptr;

bool Renderer::Initialize(const RendererDescription& description)
{
    if (IsInitialized())
    {
        BT_LOG_ERROR(Renderer, "The renderer was already initialized! Aborting.");
        return false;
    }

    s_data = btnew RendererData();
    if (!s_data)
    {
        BT_LOG_ERROR(Renderer, "Failed to allocate memory for the renderer data! Aborting.");
        return false;
    }

    s_data->renderer_api = description.renderer_api;
    
    switch (s_data->renderer_api)
    {
        case ERendererAPI::D3D11:   s_interface = btnew D3D11Renderer();
        // case ERendererAPI::D3D12:   s_interface = btnew D3D12Renderer();
        // case ERendererAPI::Vulkan:  s_interface = btnew VulkanRenderer();
        // case ERendererAPI::Metal:   s_interface = btnew MetalRenderer();
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

    // At this point, the renderer must be in a valid state.
    Check(IsInitialized());

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

    btdelete s_data;
    s_data = nullptr;
}

bool Renderer::IsInitialized()
{
    return s_data && s_interface;
}

} // namespace Basalt
