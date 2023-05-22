// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "Engine/Launch.h"
#include "EditorEngine.h"

namespace Basalt
{

Engine* InstantiateEngine()
{
    return bnew EditorEngine();
}

} // namespace Basalt
