// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/Core.h"
#include "RendererAPI.h"

namespace Basalt
{

enum class EShaderStage : U8
{
    Vertex, Pixel, Compute, Geometry,
    MaxEnumValue
};

struct ShaderDescription
{
    FixedArray<Buffer, (U8)EShaderStage::MaxEnumValue> bytecodes;

    const char* debug_name = "Unnamed";
};

class Shader : public RefCounted
{
public:
    static Ref<Shader> Create(const ShaderDescription& description);
    virtual ~Shader() override = default;
};

} // namespace Basalt
