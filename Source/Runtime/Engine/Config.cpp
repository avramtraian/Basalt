// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "Config.h"
#include "Core/Platform/Platform.h"

namespace Basalt
{

bool Config::Initialize()
{
    // Get the absolute path where the executable is located.
    const String exectuable_path = Platform::GetExecutablePath();

#if BT_BUILD_EDITOR
    m_engine_path = exectuable_path.ParentDirectory(4);
    m_game_name = "Sandbox"sv;
    m_game_path = m_engine_path + "/Sandbox"sv;
#else
    m_game_path = exectuable_path.ParentDirectory(4);
    m_game_name = exectuable_path.Stem();
#endif // BT_BUILD_EDITOR

    return true;
}

} // namespace Basalt
