// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/CoreDefines.h"
#include "Core/CoreTypes.h"
#include "Core/Containers/Array.h"
#include "Core/Containers/Optional.h"
#include "Core/Containers/StringView.h"
#include "Core/Containers/String.h"

namespace Basalt
{

/**
 * 
 */
class BASALT_API CommandLine
{
public:
    static constexpr Usize MaxCommandLineSize = 16 * KiB;

    struct Switch
    {
        StringView name;
        StringView value;
    };

    enum class ETokenType
    {
        Flag, Switch
    };

public:
    static const char* Get();

    static bool Set(const char* command_line);
    static void Parse();

    static const char* GetOriginal();

    static bool HasFlag(StringView flag);

    static Optional<StringView> GetSwitchValue(StringView switch_name);

    static void Append(StringView argument);

private:
    static void ConstructFromToken(Usize token_begin, Usize token_end, ETokenType type, Usize switch_value_offset = 0);

private:
    static char s_command_line[MaxCommandLineSize];
    static char s_original_command_line[MaxCommandLineSize];

    static Array<StringView> m_flags;
    static Array<Switch> m_switches;
};

} // namespace Basalt
