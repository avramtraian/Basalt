// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/Core.h"

namespace Basalt
{

class BASALT_API Config
{
public:
    BT_NON_COPYABLE(Config);
    BT_NON_MOVABLE(Config);

public:
    Config() = default;

    bool Initialize();

public:
#if BT_BUILD_EDITOR
    /** @return The absolute path to the engine directory. */
    FORCEINLINE const String& GetEnginePath() const { return m_engine_path; }
#endif // BT_BUILD_EDITOR

    /** @return The absolute path to the current game directory. */
    FORCEINLINE const String& GetGamePath() const { return m_game_path; }

    /** @return The name of the current game. */
    FORCEINLINE const String& GetGameName() const { return m_game_name; }

private:
#if BT_BUILD_EDITOR
    String m_engine_path;
#endif // BT_BUILD_EDITOR

    String m_game_path;
    String m_game_name;
};

} // namespace Basalt
