// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "D3D11Shader.h"

#include "D3D11Renderer.h"

namespace Basalt
{

D3D11Shader::D3D11Shader(const ShaderDescription& description)
{
    for (U8 shader_stage = 0; shader_stage < (U8)EShaderStage::MaxEnumValue; ++shader_stage)
    {
        Buffer bytecode = description.bytecodes[shader_stage];

        if (bytecode.size > 0)
        {
            m_shader_modules[shader_stage] = CreateShaderModule((EShaderStage)shader_stage, bytecode);
        }
        else
        {
            m_shader_modules[shader_stage] = nullptr;
        }
    }
}

D3D11Shader::~D3D11Shader()
{
    for (U8 shader_stage = 0; shader_stage < (U8)EShaderStage::MaxEnumValue; ++shader_stage)
    {
        void* shader_module = m_shader_modules[shader_stage];
        if (shader_module != nullptr)
        {
            switch ((EShaderStage)shader_stage)
            {
                case EShaderStage::Vertex:      { ((ID3D11VertexShader*)shader_module)->Release();      break; }
                case EShaderStage::Pixel:       { ((ID3D11PixelShader*)shader_module)->Release();       break; }
                case EShaderStage::Compute:     { ((ID3D11ComputeShader*)shader_module)->Release();     break; }
                case EShaderStage::Geometry:    { ((ID3D11GeometryShader*)shader_module)->Release();    break; }
            }
        }
    }
}

void* D3D11Shader::CreateShaderModule(EShaderStage stage, Buffer bytecode)
{
    auto device = D3D11Renderer::GetDevice();

    void* shader_module = nullptr;
    switch (stage)
    {
        case EShaderStage::Vertex:      { BT_D3D11_CHECK(device->CreateVertexShader(bytecode.data,     bytecode.size, nullptr, (ID3D11VertexShader**)&shader_module));      break; }
        case EShaderStage::Pixel:       { BT_D3D11_CHECK(device->CreatePixelShader(bytecode.data,      bytecode.size, nullptr, (ID3D11PixelShader**)&shader_module));       break; }
        case EShaderStage::Compute:     { BT_D3D11_CHECK(device->CreateComputeShader(bytecode.data,    bytecode.size, nullptr, (ID3D11ComputeShader**)&shader_module));     break; }
        case EShaderStage::Geometry:    { BT_D3D11_CHECK(device->CreateGeometryShader(bytecode.data,   bytecode.size, nullptr, (ID3D11GeometryShader**)&shader_module));    break; }
    }

    return shader_module;
}

} // namespace Basalt
