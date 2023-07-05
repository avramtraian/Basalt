// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "EditorEngine.h"

#include "Core/Memory/MemoryOperators.h"

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

    return true;
}

void EditorEngine::PreShutdown()
{
    Check(g_editor_engine);
    g_editor_engine = nullptr;
}

} // namespace Basalt
