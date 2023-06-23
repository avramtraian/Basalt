// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "D3D11Texture.h"

#include "D3D11Renderer.h"

namespace Basalt
{

namespace Utils
{

FORCEINLINE UINT TextureFormatBitsPerPixel(ETextureFormat format)
{
    switch (format)
    {
        case ETextureFormat::R8G8B8A8: return 32;
    }

    Checkf(false, "Invalid ETextureFormat!");
    return 0;
}

FORCEINLINE D3D11_TEXTURE_ADDRESS_MODE TextureAddressModeToDX(ETextureAddressMode address_mode)
{
    switch (address_mode)
    {
        case ETextureAddressMode::Repeat:           return D3D11_TEXTURE_ADDRESS_WRAP;
        case ETextureAddressMode::MirroredRepeat:   return D3D11_TEXTURE_ADDRESS_MIRROR;
        case ETextureAddressMode::ClampToEdge:      return D3D11_TEXTURE_ADDRESS_CLAMP;
        case ETextureAddressMode::ClampToBorder:    return D3D11_TEXTURE_ADDRESS_BORDER;
    }

    Checkf(false, "Invalid ETextureAddressMode!");
    return D3D11_TEXTURE_ADDRESS_WRAP;
}

FORCEINLINE D3D11_FILTER TextureFilteringToDX(ETextureFiltering mag_filter, ETextureFiltering min_filter)
{
    if (min_filter == ETextureFiltering::Linear)
    {
        if (mag_filter == ETextureFiltering::Linear)
        {
            return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        }
        else if (mag_filter == ETextureFiltering::Nearest)
        {
            return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
        }
    }
    else if (min_filter == ETextureFiltering::Nearest)
    {
        if (mag_filter == ETextureFiltering::Linear)
        {
            return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
        }
        else if (mag_filter == ETextureFiltering::Nearest)
        {
            return D3D11_FILTER_MIN_MAG_MIP_POINT;
        }
    }

    Checkf(false, "Invalid ETextureFiltering!");
    return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
}

} // namespace Basalt::Utils

D3D11Texture2D::D3D11Texture2D(const Texture2DDescription& description)
    : m_description(description)
{
    auto* device = D3D11Renderer::GetDevice();

    D3D11_TEXTURE2D_DESC texture_description = {};
    texture_description.Width = m_description.width;
    texture_description.Height = m_description.height;
    texture_description.MipLevels = 1;
    texture_description.ArraySize = 1;
    texture_description.Format = Utils::TextureFormatToDXGIFormat(m_description.format);
    texture_description.SampleDesc.Count = 1;
    texture_description.SampleDesc.Quality = 0;
    texture_description.Usage = D3D11_USAGE_DEFAULT;
    texture_description.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA texture_data = {};
    texture_data.pSysMem = m_description.texture_data;
    texture_data.SysMemPitch = m_description.texture_data ? (Utils::TextureFormatBitsPerPixel(m_description.format) / 8) : 0;

    BT_D3D11_CHECK(device->CreateTexture2D(&texture_description, &texture_data, &m_texture_handle));

    D3D11_SAMPLER_DESC sampler_description = {};
    sampler_description.AddressU = Utils::TextureAddressModeToDX(m_description.address_mode_u);
    sampler_description.AddressV = Utils::TextureAddressModeToDX(m_description.address_mode_v);
    sampler_description.AddressW = Utils::TextureAddressModeToDX(m_description.address_mode_w);
    sampler_description.Filter = Utils::TextureFilteringToDX(m_description.mag_filter, m_description.min_filter);
    Memory::Copy(sampler_description.BorderColor, m_description.border_color, sizeof(m_description.border_color));

    // NOTE(traian): Cache the sampler states and reuse them.
    BT_D3D11_CHECK(device->CreateSamplerState(&sampler_description, &m_sampler));

}

D3D11Texture2D::~D3D11Texture2D()
{
    m_sampler->Release();
    m_texture_handle->Release();
}

} // namespace Basalt
