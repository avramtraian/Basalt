// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Renderer/Texture.h"

#include "D3D11.h"

namespace Basalt
{

class D3D11Texture2D : public Texture2D
{
public:
    D3D11Texture2D(const Texture2DDescription& description);
    virtual ~D3D11Texture2D() override;

    FORCEINLINE Texture2DDescription& GetDescription() { return m_description; }
    FORCEINLINE const Texture2DDescription& GetDescription() const { return m_description; }

private:
    Texture2DDescription m_description;
    
    ID3D11Texture2D* m_texture_handle = nullptr;
    ID3D11SamplerState* m_sampler = nullptr;
};

} // namespace Basalt
