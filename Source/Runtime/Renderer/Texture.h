// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/Core.h"
#include "Image.h"
#include "RendererAPI.h"

namespace Basalt
{

enum class ETextureFiltering
{
    None = 0,
    Nearest,
    Linear,
    MaxEnumValue
};

enum class ETextureAddressMode
{
    None = 0,
    Repeat,
    MirroredRepeat,
    ClampToEdge,
    ClampToBorder,
    MaxEnumValue
};

struct BASALT_S_API Texture2DDescription
{
    // The dimensions of the texture.
    U32 width = 0;
    U32 height = 0;

    const void* texture_data = nullptr;

    // The format of the texture.
    EImageFormat format = EImageFormat::None;

    // The texture mag/mig filtering mode.
    ETextureFiltering mag_filter = ETextureFiltering::Linear;
    ETextureFiltering min_filter = ETextureFiltering::Linear;

    // The texture address mode.
    ETextureAddressMode address_mode_u = ETextureAddressMode::ClampToEdge;
    ETextureAddressMode address_mode_v = ETextureAddressMode::ClampToEdge;
    ETextureAddressMode address_mode_w = ETextureAddressMode::ClampToEdge;

    float border_color[4] = { 1.0F, 0.0F, 1.0F, 1.0F };
};

class BASALT_API Texture2D : public RefCounted
{
public:
    static Ref<Texture2D> Create(const Texture2DDescription& description);

public:
    virtual ~Texture2D() = default;
};

} // namespace Basalt
