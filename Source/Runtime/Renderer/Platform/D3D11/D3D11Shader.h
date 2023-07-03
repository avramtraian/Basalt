// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "D3D11.h"
#include "Renderer/Shader.h"

namespace Basalt
{

class D3D11Shader : public Shader
{
public:
    D3D11Shader(const ShaderDescription& description);
    virtual ~D3D11Shader() override;

private:
    void* CreateShaderModule(ShaderStage stage, Buffer bytecode);

private:
    FixedArray<void*, (U8)ShaderStage::MaxEnumValue> m_shader_modules;
};

} // namespace Basalt
