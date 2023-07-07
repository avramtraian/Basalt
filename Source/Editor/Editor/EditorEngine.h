// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/Misc/BinaryID.h"
#include "Core/Misc/Version.h"
#include "Engine/Engine.h"
#include "Renderer/Shader.h"
#include "ShaderCompiler/ShaderCompiler.h"

namespace Basalt
{

class EditorEngine : public Engine
{
public:
	virtual void GenerateEngineCallbacks() override;

    virtual bool PostInitialize() override;
    virtual void PreShutdown() override;

private:
    struct ShaderCacheFileDescription
    {
        BinaryID<4> file_id;
        U32 reserved = 0;
        Version file_version;
        U64 source_code_hash;
    };

    struct ShaderCacheFileStructure
    {
        U32 shader_stages_mask;
        U32 bytecode_containers_sizes[ShaderStagesCount];
    };

	void CompileEngineShaders();

    bool CompileShaderIfNotCached(StringView shader_filepath, StringView shader_name);

	bool CompileShader(const String& source_code, U64 source_code_hash, StringView cache_filepath);

    U32 ReadShaderStages(const String& source_code, StringView shader_name);

    bool IsShaderDirty(U64 source_code_hash, StringView cache_filepath);

    bool ReadShaderFile(StringView filepath, String& out_source_code, U64& out_source_code_hash);

    void CacheShaderBytecode(
        StringView cache_filepath,
        U64 source_code_hash,
        U32 shader_stages_mask,
        ArrayView<const ScopedBuffer> bytecodes
    );

private:
	ShaderCompiler m_shader_compiler;
};

extern EditorEngine* g_editor_engine;

} // namespace Basalt