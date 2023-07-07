// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/Core.h"
#include "Renderer/RendererAPI.h"

namespace Basalt
{

enum class ShaderStage : U8
{
    Vertex,
    Hull,
    Domain,
    Geometry,
    Pixel,
    Compute,
};

constexpr U8 ShaderStagesCount = (U8)ShaderStage::Compute + 1;

struct ShaderDescription
{
    FixedArray<Buffer, ShaderStagesCount> bytecodes;

    const char* debug_name = "Unnamed";
};

class Shader : public RefCounted
{
public:
    static Ref<Shader> Create(const ShaderDescription& description);
    virtual ~Shader() override = default;
};

namespace Utils
{

FORCEINLINE const char* ShaderStageToString(ShaderStage stage)
{
    switch (stage)
    {
        case ShaderStage::Vertex:   return "Vertex";
        case ShaderStage::Hull:     return "Hull";
        case ShaderStage::Domain:   return "Domain";
        case ShaderStage::Geometry: return "Geometry";
        case ShaderStage::Pixel:    return "Pixel";
        case ShaderStage::Compute:  return "Compute";
    }

    Checkf(false, "Invalid ShaderStage!");
    return "Unknown";
}

} // namespace Utils

} // namespace Basalt
