// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "EditorEngine.h"

#include "Core/Filesystem/FileManager.h"
#include "Core/Memory/MemoryOperators.h"
#include "Renderer/Renderer.h"

// Constants related to cached shader bytecode files.
#define BT_SHADER_FILE_EXTENSION            ".btshader"sv
#define BT_SHADER_FILE_STAGES_DIRECTIVE     "#pragma stages"sv
#define BT_SHADER_FILE_LATEST_VERSION       Version(1, 0, 0, 0)
#define BT_SHADER_FILE_STAGE_ID_VERTEX      "vertex"sv
#define BT_SHADER_FILE_STAGE_ID_HULL        "hull"sv
#define BT_SHADER_FILE_STAGE_ID_DOMAIN      "domain"sv
#define BT_SHADER_FILE_STAGE_ID_GEOMETRY    "geometry"sv
#define BT_SHADER_FILE_STAGE_ID_PIXEL       "pixel"sv
#define BT_SHADER_FILE_STAGE_ID_COMPUTE     "compute"sv

// Path relative the game directory where the shader cache files are located.
#define BT_CACHED_SHADERS_PATH              "/Saved/Shaders/"sv
#define BT_ENGINE_RUNTIME_SHADERS_PATH      "/Content/Runtime/Shaders"sv

namespace Basalt
{

EditorEngine* g_editor_engine = nullptr;

void EditorEngine::GenerateEngineCallbacks()
{
}

bool EditorEngine::PostInitialize()
{
    Check(!g_editor_engine);
    g_editor_engine = this;

    ShaderBytecodeFormat bytecode_format = ShaderBytecodeFormat::None;
    switch (Renderer::GetRendererAPI())
    {
        case RendererAPI::D3D11:
        case RendererAPI::D3D12:
            bytecode_format = ShaderBytecodeFormat::DXIL;
            break;
        case RendererAPI::Vulkan:
            bytecode_format = ShaderBytecodeFormat::SPIRV;
            break;
        case RendererAPI::Metal:
            bytecode_format = ShaderBytecodeFormat::Metal;
            break;
    }

    if (!m_shader_compiler.Initialize(bytecode_format))
    {
        BT_LOG_ERROR(Editor, "Failed to initialize the shader compiler!");
        return false;
    }

    CompileEngineShaders();
    return true;
}

void EditorEngine::PreShutdown()
{
    m_shader_compiler.Shutdown();

    Check(g_editor_engine);
    g_editor_engine = nullptr;
}

void EditorEngine::CompileEngineShaders()
{
    struct DirtyShader
    {
        String cache_filepath;
        String source_code;
        U64 source_code_hash = 0;
    };

    class ShaderVisitor : public DirectoryVisitor
    {
    public:
        ShaderVisitor(Array<DirtyShader>& dirty_shaders, const String& cached_shaders_path)
            : m_dirty_shaders(dirty_shaders)
            , m_cached_shaders_path(cached_shaders_path)
        {}

        virtual IterationDecision Visit(const String& filepath, bool is_directory) override
        {
            if (is_directory || filepath.Extension() != ".hlsl"sv)
            {
                // Ignore directories or files that are not shaders.
                return IterationDecision::Continue;
            }

            String source_code;
            U64 source_code_hash;

            if (!g_editor_engine->ReadShaderFile(filepath.ToView(), source_code, source_code_hash))
            {
                // Failed to read the shader source code file.
                return IterationDecision::Continue;
            }

            String cache_filepath = m_cached_shaders_path + filepath.Stem() + BT_SHADER_FILE_EXTENSION;
            if (g_editor_engine->IsShaderDirty(source_code_hash, cache_filepath.ToView()))
            {
                DirtyShader dirty_shader;
                dirty_shader.cache_filepath = Move(cache_filepath);
                dirty_shader.source_code = Move(source_code);
                dirty_shader.source_code_hash = source_code_hash;
                m_dirty_shaders.Add(Move(dirty_shader));
            }

            return IterationDecision::Continue;
        }

    private:
        Array<DirtyShader>& m_dirty_shaders;
        const String& m_cached_shaders_path;
    };

    // List of shaders that need to be recompiled.
    Array<DirtyShader> dirty_engine_shaders;

    // Get the path where the runtime engine shaders are located.
    const String engine_shaders_path = GetConfig().GetEnginePath() + BT_ENGINE_RUNTIME_SHADERS_PATH;
    // Get the path to the directory where shader cache files are located.
    const String cached_shaders_path = GetConfig().GetGamePath() + BT_CACHED_SHADERS_PATH;
    
    // Iterate over all shader files and store each shader source code.
    ShaderVisitor visitor = ShaderVisitor(dirty_engine_shaders, cached_shaders_path);
    FileManager::IterateDirectoryRecursively(engine_shaders_path.ToView(), visitor);

    if (!dirty_engine_shaders.IsEmpty())
    if (!dirty_engine_shaders.IsEmpty())
    {
        BT_LOG_INFO(Editor, "Comiling %d shaders...", dirty_engine_shaders.Count());
        for (auto& shader : dirty_engine_shaders)
        {
            CompileShader(shader.source_code, shader.source_code_hash, shader.cache_filepath.ToView());
        }
    }
}

bool EditorEngine::CompileShaderIfNotCached(StringView shader_filepath, StringView shader_name)
{
    // Get the filepath where the shader cache file is located.
    String shader_cache_path = GetConfig().GetGamePath() + BT_CACHED_SHADERS_PATH + shader_name + BT_SHADER_FILE_EXTENSION;

    String source_code;
    U64 source_code_hash;

    // Read the shader source code.
    if (!ReadShaderFile(shader_filepath, source_code, source_code_hash))
    {
        // Failed to read the shader source code file.
        return false;
    }

    if (IsShaderDirty(source_code_hash, shader_cache_path.ToView()))
    {
        // The shader needs to be recompiled.
        return CompileShader(source_code, source_code_hash, shader_cache_path.ToView());
    }

    // The shader cache is valid.
    return true;
}

bool EditorEngine::CompileShader(const String& source_code, U64 source_code_hash, StringView cache_filepath)
{
    const U32 shader_stages = ReadShaderStages(source_code, cache_filepath.Stem());
    if (shader_stages == 0)
    {
        BT_LOG_ERROR(Editor, "No shader stages were specified for: '%s'", String(cache_filepath.Stem()).c_str());
        return false;
    }

    ScopedBuffer bytecodes[ShaderStagesCount] = {};
    Usize bytecodes_count = 0;

    for (U8 shader_stage = 0; shader_stage < ShaderStagesCount; ++shader_stage)
    {
        if (shader_stages & BIT(shader_stage))
        {
            ShaderCompilationOptions options;
            options.shader_stage = (ShaderStage)shader_stage;
            options.source_code = source_code;

            ShaderCompilationResult result;
            if (!m_shader_compiler.Compile(options, result))
            {
                BT_LOG_ERROR(
                    Editor, "Failed to compile shader: '%s', stage: '%s'",
                    String(cache_filepath.Stem()).c_str(), Utils::ShaderStageToString((ShaderStage)shader_stage));
                return false;
            }

            bytecodes[bytecodes_count++].RawBuffer() = result.bytecode;
        }
    }
    
    CacheShaderBytecode(cache_filepath, source_code_hash, shader_stages, ArrayView<const ScopedBuffer>(bytecodes, bytecodes_count));
    return true;
}

U32 EditorEngine::ReadShaderStages(const String& source_code, StringView shader_name)
{
    U32 shader_stages_mask = 0;

    const Usize stages_line_offset = source_code.FindFirstOf(BT_SHADER_FILE_STAGES_DIRECTIVE);
    if (stages_line_offset == StringView::InvalidPos)
    {
        BT_LOG_ERROR(Editor, "Shader '%s' doesn't specify '%s'!", String(shader_name).c_str(), String(BT_SHADER_FILE_STAGES_DIRECTIVE).c_str());
        return 0;
    }

    Usize offset = stages_line_offset + BT_SHADER_FILE_STAGES_DIRECTIVE.BytesCount();
    Usize stages_line_end_offset = source_code.FindFirstOf('\n', stages_line_offset);

    if (stages_line_end_offset == StringView::InvalidPos)
    {
        // The stages directive is located on the last line of the shader file.
        stages_line_end_offset = source_code.BytesCount() - 1;
    }
    else if (source_code[stages_line_end_offset - 1] == '\r')
    {
        // The CRLF new-line sequence is used.
        --stages_line_end_offset;
    }

    Usize stage_begin_offset = StringView::InvalidPos;

    Array<StringView> shader_stages;

    for (offset; offset < stages_line_end_offset; ++offset)
    {
        char character = source_code[offset];
        if (character == ' ')
        {
            if (stage_begin_offset != StringView::InvalidPos)
            {
                StringView stage = source_code.Substring(stage_begin_offset, offset - stage_begin_offset);
                shader_stages.Add(stage);
                
                // The shader stage ends.
                stage_begin_offset = StringView::InvalidPos;
            }

            // Spaces are ignored.
            continue;
        }
        if (character == ',')
        {
            if (stage_begin_offset == StringView::InvalidPos)
            {
                continue;
            }

            StringView stage = source_code.Substring(stage_begin_offset, offset - stage_begin_offset);
            shader_stages.Add(stage);

            // The shader stage ends.
            stage_begin_offset = StringView::InvalidPos;
        }
        else if (('a' <= character && character <= 'z') || ('A' <= character && character <= 'Z'))
        {
            if (stage_begin_offset == StringView::InvalidPos)
            {
                // A new shader stage begins.
                stage_begin_offset = offset;
            }
        }
        else
        {
            BT_LOG_ERROR(Editor, "Only letters are allowed in the shader stages!");
            return 0;
        }
    }

    if (stage_begin_offset != StringView::InvalidPos)
    {
        StringView stage = source_code.Substring(stage_begin_offset, stages_line_end_offset - stage_begin_offset);
        shader_stages.Add(stage);
    }

    StringView shader_stages_ids[] =
    {
        BT_SHADER_FILE_STAGE_ID_VERTEX,
        BT_SHADER_FILE_STAGE_ID_HULL,
        BT_SHADER_FILE_STAGE_ID_DOMAIN,
        BT_SHADER_FILE_STAGE_ID_GEOMETRY,
        BT_SHADER_FILE_STAGE_ID_PIXEL,
        BT_SHADER_FILE_STAGE_ID_COMPUTE,
    };
    static_assert(BT_ARRAY_COUNT(shader_stages_ids) == ShaderStagesCount);

    for (StringView stage : shader_stages)
    {
        bool is_valid_stage = false;
        for (Usize shader_stage = 0; shader_stage < ShaderStagesCount; ++shader_stage)
        {
            if (stage == shader_stages_ids[shader_stage])
            {
                if (shader_stages_mask & BIT(shader_stage))
                {
                    BT_LOG_WARN(Editor, "Shader stage '%s' specified multiple times!", String(stage).c_str());
                }

                // Add the shader stage to the bit-mask.
                shader_stages_mask |= BIT(shader_stage);
                is_valid_stage = true;
                break;
            }
        }

        if (!is_valid_stage)
        {
            BT_LOG_ERROR(Editor, "Unknown shader stage '%s'!", String(stage).c_str());
        }
    }

    return shader_stages_mask;
}

bool EditorEngine::IsShaderDirty(U64 source_code_hash, StringView cache_filepath)
{
    FileReader cache_reader;
    EFileError file_error_code = FileManager::CreateReader(&cache_reader, cache_filepath);
    if (!cache_reader.IsValid() || file_error_code != EFileError::Success)
    {
        // There is no shader cache file, so the shader must be recompiled.
        return true;
    }

    ShaderCacheFileDescription cache_description;
    cache_reader.Read<ShaderCacheFileDescription>(&cache_description);
    cache_reader.Close();

    if (cache_description.source_code_hash != source_code_hash)
    {
        // The shader cache file is not valid, so the shader must be recompiled.
        return true;
    }

    // The shader cache is valid.
    return false;
}

bool EditorEngine::ReadShaderFile(StringView filepath, String& out_source_code, U64& out_source_code_hash)
{
    // Open the file for reading.
    FileReader source_code_reader;
    EFileError file_error_code = FileManager::CreateReader(&source_code_reader, filepath);
    if (!source_code_reader.IsValid() || file_error_code != EFileError::Success)
    {
        BT_LOG_ERROR(Editor, "Failed to open shader file: '%s'", String(filepath).c_str());
        return false;
    }

    // Read the shader source code.
    source_code_reader.ReadAllAsString(out_source_code);
    // Compute the shader source code hash.
    out_source_code_hash = Hasher::Compute<String>(out_source_code);

    return true;
}

void EditorEngine::CacheShaderBytecode(StringView cache_filepath, U64 source_code_hash, U32 shader_stages_mask, ArrayView<const ScopedBuffer> bytecodes)
{
    FileWriter cache_writer;
    EFileError file_error_code = FileManager::CreateWriter(&cache_writer, cache_filepath);
    if (!cache_writer.IsValid() || file_error_code != EFileError::Success)
    {
        BT_LOG_ERROR(Editor, "Failed to open cache file '%s' for writing!", String(cache_filepath).c_str());
        return;
    }

    ShaderCacheFileDescription cache_description = {};
    cache_description.file_id = "BTSF";
    cache_description.file_version = BT_SHADER_FILE_LATEST_VERSION;
    cache_description.source_code_hash = source_code_hash;

    ShaderCacheFileStructure cache_structure = {};
    cache_structure.shader_stages_mask = shader_stages_mask;

    for (Usize index = 0; index < bytecodes.Count(); ++index)
    {
        cache_structure.bytecode_containers_sizes[index] = (U32)bytecodes[index].Size();
    }

    cache_writer.Write<ShaderCacheFileDescription>(cache_description);
    cache_writer.Write<ShaderCacheFileStructure>(cache_structure);

    for (const ScopedBuffer& bytecode : bytecodes)
    {
        cache_writer.WriteBytes(bytecode.Data(), bytecode.Size());
    }
}

} // namespace Basalt
