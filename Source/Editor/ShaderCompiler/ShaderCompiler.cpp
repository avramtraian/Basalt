// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "ShaderCompiler.h"

#include "Core/Containers/Strings/Utf8String.h"
#include "Core/Filesystem/FileManager.h"

#if BT_PLATFORM_WINDOWS
    #define NOMINMAX
    #define WIN32_LEAN_AND_MEAN
    // Required by the HLSL compiler (DXC).
    #include <Windows.h>
    #include <d3d12.h>
#endif // BT_PLATFORM_WINDOWS

#include <dxc/dxcapi.h>

// Translates shaders to Metal.
#include <spirv_cross/spirv_msl.hpp>
// Generates shader reflection information.
#include <spirv_cross/spirv_reflect.hpp>

namespace Basalt
{

/**
 * Structure that contains the data a shader compiler manages and uses.
 */
struct ShaderCompilerData
{
    /** The DXC compiler instance handle. */
    IDxcCompiler3* compiler = nullptr;

    /** The DXC utils instance handle. Required for creating various objects. */
    IDxcUtils* utils = nullptr;

    /** The default DXC include handler. */
    IDxcIncludeHandler* include_handler = nullptr;

    /** The bytecode format that the compiler generates. */
    EShaderBytecode bytecode_format = EShaderBytecode::None;

    /** The buffer where the reflection compiler is stored. */
    ScopedBuffer reflection_compiler_storage;

    /** The buffer where the Metal cross-compiler is stored. */
    ScopedBuffer msl_cross_compiler_storage;
};

bool ShaderCompiler::Initialize(EShaderBytecode bytecode_format)
{
    Checkf(m_data == nullptr, "Shader compiler was already initialized!");
    m_data = btnew ShaderCompilerData();

    if (FAILED(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&m_data->compiler))))
    {
        return false;
    }

    if (FAILED(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&m_data->utils))))
    {
        return false;
    }

    if (FAILED(m_data->utils->CreateDefaultIncludeHandler(&m_data->include_handler)))
    {
        return false;
    }

    m_data->bytecode_format = bytecode_format;
    m_data->reflection_compiler_storage.Invalidate(sizeof(spirv_cross::CompilerReflection));
    m_data->msl_cross_compiler_storage.Invalidate(sizeof(spirv_cross::CompilerMSL));

    return true;
}

void ShaderCompiler::Shutdown()
{
    Checkf(m_data != nullptr, "Shader compiler was already shut down!");

    if (m_data->compiler)
    {
        m_data->compiler->Release();
    }
    if (m_data->utils)
    {
        m_data->utils->Release();
    }
    if (m_data->include_handler)
    {
        m_data->include_handler->Release();
    }

    btdelete m_data;
    m_data = nullptr;
}

bool ShaderCompiler::Compile(const ShaderCompilationOptions& options, ShaderCompilationResult& out_result)
{
    // Compile shader to SPIR-V bytecode. Required for reflection.
    Buffer unoptimized_spirv_bytecode;
    ShaderCompilationOptions reflection_options = options;
    reflection_options.force_debug = true;

    if (!CompileSPIRV(reflection_options, unoptimized_spirv_bytecode))
    {
        // Failed to compile to SPIR-V.
        return false;
    }

    // NOTE(traian): Shader reflection is only performed on unoptimized SPIR-V shaders, in order
    //               to ensure consistency among all shading languages and platforms.
    if (!ReflectSPIRV(unoptimized_spirv_bytecode, out_result))
    {
        // Failed to reflect shader information.
        unoptimized_spirv_bytecode.Release();
        return false;
    }

#if BT_BUILD_DEBUG
    const bool compile_debug_shaders = true;
#else
    const bool compile_debug_shaders = options.force_debug;
#endif // BT_BUILD_DEBUG

    if (!compile_debug_shaders)
    {
        // The SPIR-V unoptimized bytecode is no longer necessary.
        unoptimized_spirv_bytecode.Release();
    }

    switch (m_data->bytecode_format)
    {
        case EShaderBytecode::DXIL:
        {
            // The SPIR-V bytecode is no longer necessary.
            unoptimized_spirv_bytecode.Release();

            // Compile shader to DXIL bytecode, to directly feed to the D3D11/D3D12 API.
            if (!CompileDXIL(options, out_result.bytecode))
            {
                // Failed to compile to DXIL.
                return false;
            }

            break;
        }

        case EShaderBytecode::SPIRV:
        {
            if (compile_debug_shaders)
            {
                // The final bytecode has the same optimization level as the SPIR-V bytecode used for reflection.
                out_result.bytecode = unoptimized_spirv_bytecode;
            }
            else
            {
                if (!CompileSPIRV(options, out_result.bytecode))
                {
                    // Failed to compile to SPIR-V.
                    return false;
                }
            }

            break;
        }

        case EShaderBytecode::Metal:
        {
            Buffer spirv_bytecode;

            if (compile_debug_shaders)
            {
                // The required bytecode has the same optimization level as the SPIR-V bytecode used for reflection.
                spirv_bytecode = unoptimized_spirv_bytecode;
            }
            else
            {
                if (!CompileSPIRV(options, spirv_bytecode))
                {
                    // Failed to compile to SPIR-V.
                    return false;
                }
            }

            if (!CrossCompileToMetal(spirv_bytecode, out_result.bytecode))
            {
                // Failed to cross-compile to MSL bytecode format.
                return false;
            }

            break;
        }
    }

    // The shader compilation was successful.
    return true;
}

namespace Utils
{

FORCEINLINE static LPCWSTR ShaderStageToDXC(ShaderStage stage)
{
    switch (stage)
    {
        case ShaderStage::Vertex:   return L"vs_6_5";
        case ShaderStage::Pixel:    return L"ps_6_5";
        case ShaderStage::Compute:  return L"cs_6_5";
        case ShaderStage::Geometry: return L"gs_6_5";
    }

    Checkf(false, "Invalid EShaderStage!");
    return nullptr;
}

FORCEINLINE static LPCWSTR ShaderStageToDefaultEntryPointName(ShaderStage stage)
{
    switch (stage)
    {
        case ShaderStage::Vertex:   return L"VSMain";
        case ShaderStage::Pixel:    return L"PSMain";
        case ShaderStage::Compute:  return L"CSMain";
        case ShaderStage::Geometry: return L"GSMain";
    }

    Checkf(false, "Invalid EShaderStage!");
    return nullptr;
}

static void BuildArguments(const ShaderCompilationOptions& options, EShaderBytecode bytecode_format, Array<LPCWSTR>& out_arguments)
{
    // The DXC compiler can only directly produce DXIL or SPIR-V bytecode.
    Check(bytecode_format == EShaderBytecode::DXIL || bytecode_format == EShaderBytecode::SPIRV);
    out_arguments.SetCapacity(12 + 2 * options.defines.Count());

    out_arguments.Add(L"-E");
    if (!options.entry_point.IsEmpty())
    {
        ScopedBuffer entry_point;
        StringBuilder::ToUTF16Dynamic(options.entry_point.ToView(), entry_point.RawBuffer(), true);
        out_arguments.Add(entry_point.As<wchar_t>());
    }
    else
    {
        // Set the default entry point name, specific for each shader stage.
        out_arguments.Add(ShaderStageToDefaultEntryPointName(options.shader_stage));
    }

    out_arguments.Add(L"-T");
    out_arguments.Add(ShaderStageToDXC(options.shader_stage));

    out_arguments.Add(DXC_ARG_WARNINGS_ARE_ERRORS);

    if (!options.force_debug)
    {
#if BT_BUILD_DEBUG
        out_arguments.Add(DXC_ARG_DEBUG);
#else
        out_arguments.Add(DXC_ARG_OPTIMIZATION_LEVEL3);
#endif // BT_BUILD_DEBUG
    }
    else
    {
        out_arguments.Add(DXC_ARG_DEBUG);
    }

    if (bytecode_format == EShaderBytecode::SPIRV)
    {
        out_arguments.Add(L"-spirv");
        out_arguments.Add(L"-fspv-target-env=vulkan1.3");
        out_arguments.Add(L"-fspv-reflect");
    }

    Array<ScopedBuffer> define_arguments;
    define_arguments.SetCapacity(options.defines.Count());

    for (const auto& define : options.defines)
    {
        ScopedBuffer& buffer = define_arguments.Emplace();
        StringBuilder::ToUTF16Dynamic(define.ToView(), buffer.RawBuffer(), true);
        out_arguments.Add(L"-D");
        out_arguments.Add(buffer.As<wchar_t>());
    }
}

static bool CompileShader(ShaderCompilerData* compiler_data, Buffer source_code, Array<LPCWSTR>& arguments, Buffer& out_bytecode)
{
    DxcBuffer source_buffer = {};
    source_buffer.Ptr = source_code.data;
    source_buffer.Size = source_code.size;
    source_buffer.Encoding = DXC_CP_UTF8;

    IDxcResult* result = nullptr;
    MAYBE_UNUSED HRESULT hr_compile = compiler_data->compiler->Compile(
        &source_buffer, *arguments, (UINT32)arguments.Count(),
        compiler_data->include_handler, IID_PPV_ARGS(&result)
    );

    // NOTE(traian): There is no documentation about the result value of the IDxcCompiler3::Compile.
    //               We assume that the compiler instance should always be able to be engaged, as
    //               actual compile errors caused by the user are classified as success.
    Checkf(SUCCEEDED(hr_compile), "Failed to engage the shader compilation process!");

    IDxcBlobUtf8* compile_errors = nullptr;
    MAYBE_UNUSED HRESULT hr_errors = result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&compile_errors), 0);
    Checkf(SUCCEEDED(hr_errors), "Failed to obtain the shader compilation errors!");

    if (compile_errors)
    {
        if (compile_errors->GetStringLength() > 0)
        {
            BT_LOG_ERROR(Renderer, "Shader compilation failed: %s", (const char*)compile_errors->GetBufferPointer());
            compile_errors->Release();
            return false;
        }

        compile_errors->Release();
    }

    IDxcBlob* shader_module = nullptr;
    MAYBE_UNUSED HRESULT hr_shader_module = result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shader_module), 0);
    Checkf(SUCCEEDED(hr_shader_module), "Failed to obtain the bytecode from the shader compilation!");

    out_bytecode.Resize(shader_module->GetBufferSize());
    Memory::Copy(out_bytecode.data, shader_module->GetBufferPointer(), out_bytecode.size);

    shader_module->Release();
    return true;
}

} // namespace Basalt::Utils

bool ShaderCompiler::CompileSPIRV(const ShaderCompilationOptions& options, Buffer& out_bytecode)
{
    // Build the compiler argument list.
    Array<LPCWSTR> arguments;
    Utils::BuildArguments(options, EShaderBytecode::SPIRV, arguments);

    Buffer source_code_buffer;
    source_code_buffer.data = (U8*)options.source_code.Data();
    source_code_buffer.size = options.source_code.BytesCount();

    // Engage the HLSL compiler.
    if (!Utils::CompileShader(m_data, source_code_buffer, arguments, out_bytecode))
    {
        // Failed to compile the shader.
        return false;
    }
    
    // The shader compilation was successful.
    return true;
}

bool ShaderCompiler::CompileDXIL(const ShaderCompilationOptions& options, Buffer& out_bytecode)
{
    // Build the compiler argument list.
    Array<LPCWSTR> arguments;
    Utils::BuildArguments(options, EShaderBytecode::DXIL, arguments);

    Buffer source_code_buffer;
    source_code_buffer.data = (U8*)options.source_code.Data();
    source_code_buffer.size = options.source_code.BytesCount();

    // Engage the HLSL compiler.
    if (!Utils::CompileShader(m_data, source_code_buffer, arguments, out_bytecode))
    {
        // Failed to compile the shader.
        return false;
    }

    // The shader compilation was successful.
    return true;
}

bool ShaderCompiler::ReflectSPIRV(Buffer bytecode, ShaderCompilationResult& out_result)
{
    // Allocating the reflection compiler.
    new (m_data->reflection_compiler_storage.Data()) spirv_cross::CompilerReflection(bytecode.As<U32>(), bytecode.CountOf<U32>());
    auto& compiler = *m_data->reflection_compiler_storage.As<spirv_cross::CompilerReflection>();

    // Getting the shader resources.
    spirv_cross::ShaderResources resources = compiler.get_shader_resources();

    // TODO(traian): Fill out the shader reflection data.
    // ...

    // Releasing the reflection compiler.
    compiler.~CompilerReflection();

    // The shader reflection was successful.
    return true;
}

bool ShaderCompiler::CrossCompileToMetal(Buffer spirv_bytecode, Buffer& out_bytecode)
{
    // Allocating the MSL cross-compiler.
    new (m_data->msl_cross_compiler_storage.Data()) spirv_cross::CompilerMSL(spirv_bytecode.As<U32>(), spirv_bytecode.CountOf<U32>());
    auto& compiler = *m_data->msl_cross_compiler_storage.As<spirv_cross::CompilerMSL>();

    std::string msl_source_code = compiler.compile();
    // Releasing the MSL cross-compiler.
    compiler.~CompilerMSL();

    Checkf(false, "Cross-compiling shaders to Metal Shading Language is currently not supported!");
    return false;

    // The shader compilation was successful.
    return true;
}

} // namespace Basalt
