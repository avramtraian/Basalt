// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "D3D11Shader.h"

#include "D3D11Renderer.h"

namespace Basalt
{

D3D11Shader::D3D11Shader(const ShaderDescription& description)
{
    for (U8 shader_stage = 0; shader_stage < ShaderStagesCount; ++shader_stage)
    {
        Buffer bytecode = description.bytecodes[shader_stage];

        if (bytecode.size > 0)
        {
            m_shader_modules[shader_stage] = CreateShaderModule((ShaderStage)shader_stage, bytecode);
        }
        else
        {
            m_shader_modules[shader_stage] = nullptr;
        }
    }
}

D3D11Shader::~D3D11Shader()
{
    for (U8 shader_stage = 0; shader_stage < ShaderStagesCount; ++shader_stage)
    {
        void* shader_module = m_shader_modules[shader_stage];
        if (shader_module != nullptr)
        {
            switch ((ShaderStage)shader_stage)
            {
                case ShaderStage::Vertex:       { ((ID3D11VertexShader*)shader_module)->Release();      break; }
                case ShaderStage::Hull:         { ((ID3D11HullShader*)shader_module)->Release();        break; }
                case ShaderStage::Domain:       { ((ID3D11DomainShader*)shader_module)->Release();      break; }
                case ShaderStage::Geometry:     { ((ID3D11GeometryShader*)shader_module)->Release();    break; }
                case ShaderStage::Pixel:        { ((ID3D11PixelShader*)shader_module)->Release();       break; }
                case ShaderStage::Compute:      { ((ID3D11ComputeShader*)shader_module)->Release();     break; }
            }
        }
    }
}

void* D3D11Shader::CreateShaderModule(ShaderStage stage, Buffer bytecode)
{
    auto device = D3D11Renderer::GetDevice();

    void* shader_module = nullptr;
    switch (stage)
    {
        case ShaderStage::Vertex:       { BT_D3D11_CHECK(device->CreateVertexShader(bytecode.data,      bytecode.size, nullptr, (ID3D11VertexShader**)&shader_module));     break; }
        case ShaderStage::Hull:         { BT_D3D11_CHECK(device->CreateHullShader(bytecode.data,        bytecode.size, nullptr, (ID3D11HullShader**)&shader_module));       break; }
        case ShaderStage::Domain:       { BT_D3D11_CHECK(device->CreateDomainShader(bytecode.data,      bytecode.size, nullptr, (ID3D11DomainShader**)&shader_module));     break; }
        case ShaderStage::Geometry:     { BT_D3D11_CHECK(device->CreateGeometryShader(bytecode.data,    bytecode.size, nullptr, (ID3D11GeometryShader**)&shader_module));   break; }
        case ShaderStage::Pixel:        { BT_D3D11_CHECK(device->CreatePixelShader(bytecode.data,       bytecode.size, nullptr, (ID3D11PixelShader**)&shader_module));      break; }
        case ShaderStage::Compute:      { BT_D3D11_CHECK(device->CreateComputeShader(bytecode.data,     bytecode.size, nullptr, (ID3D11ComputeShader**)&shader_module));    break; }
    }

    return shader_module;
}

} // namespace Basalt
