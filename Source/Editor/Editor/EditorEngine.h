// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Engine/Engine.h"

namespace Basalt
{

class EditorEngine : public Engine
{
public:
	virtual void GenerateEngineCallbacks() override;

    virtual bool PostInitialize() override;
    virtual void PreShutdown() override;
};

extern EditorEngine* g_editor_engine;

} // namespace Basalt