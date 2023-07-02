// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "Config.h"
#include "Core/Platform/Platform.h"

namespace Basalt
{

bool Config::Initialize()
{
#if BT_BUILD_EDITOR
    m_engine_path = Platform::GetExecutablePath();
#endif // BT_BUILD_EDITOR

    return true;
}

} // namespace Basalt
