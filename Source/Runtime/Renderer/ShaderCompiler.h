// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/Core.h"
#include "RendererAPI.h"

namespace Basalt
{

enum class EShaderBytecode
{
    None = 0,
    DXIL, SPIRV, Metal,
    MaxEnumValue
};

struct ShaderCompilerDescription
{
    EShaderBytecode bytecode_format = EShaderBytecode::DXIL;
};

enum class EShaderStage
{
    None = 0,
    Vertex, Pixel, Compute, Geometry,
    MaxEnumValue
};

struct ShaderCompilationOptions
{
    Array<String> defines;
    EShaderStage shader_stage = EShaderStage::None;
    String entry_point;
    StringView filepath;
    bool force_debug = false;
};

struct ShaderCompilationResult
{
    Buffer bytecode;
    // TODO(traian): Reflection data.
};

class ShaderCompiler
{
public:
    ShaderCompiler() = default;
    ~ShaderCompiler();

    bool Initialize(const ShaderCompilerDescription& description);
    
    bool Compile(const ShaderCompilationOptions& options, ShaderCompilationResult& out_result);

private:
    bool CompileSPIRV(const ShaderCompilationOptions& options, Buffer& out_bytecode);
    bool CompileDXIL(const ShaderCompilationOptions& options, Buffer& out_bytecode);

    bool ReflectSPIRV(Buffer bytecode, ShaderCompilationResult& out_result);

    bool CrossCompileToMetal(Buffer spirv_bytecode, Buffer& out_bytecode);

private:
    struct ShaderCompilerData* m_data = nullptr;
};

} // namespace Basalt
