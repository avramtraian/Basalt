// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "D3D11Renderer.h"

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
