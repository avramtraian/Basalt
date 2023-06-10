// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "CommandLine.h"

#include "Core/Containers/Strings/AsciiString.h"
#include "Core/Memory/Memory.h"

namespace Basalt
{

char CommandLine::s_command_line[];
char CommandLine::s_original_command_line[];

Array<StringView> CommandLine::m_flags;
Array<CommandLine::Switch> CommandLine::m_switches;

const char* CommandLine::Get()
{
    return s_command_line;
}

bool CommandLine::Set(const char* command_line)
{
    const Usize cmd_line_length = ASCIICalls::Length(command_line);
    Check(cmd_line_length < MaxCommandLineSize);
    if (cmd_line_length >= MaxCommandLineSize)
    {
        // The given command line is too big.
        return false;
    }

    Memory::Copy(s_command_line, command_line, cmd_line_length + 1);
    Memory::Copy(s_original_command_line, command_line, cmd_line_length + 1);
    
    return true;
}

void CommandLine::Parse()
{
    Usize token_offset = 0;
    bool is_parsing_token = false;
    Usize switch_value_offset = 0;
    ETokenType token_type = ETokenType::Flag;

    const Usize cmd_line_length = ASCIICalls::Length(s_command_line);
    for (Usize i = 0; i < cmd_line_length; ++i)
    {
        const char c = s_command_line[i];

        if (c == '-' && !is_parsing_token)
        {
            is_parsing_token = true;
            token_offset = i + 1;
        }
        else if (c == ' ' && is_parsing_token)
        {
            ConstructFromToken(token_offset, i, token_type, switch_value_offset);

            is_parsing_token = false;
            token_type = ETokenType::Flag;
        }
        else if (c == '=' && is_parsing_token)
        {
            token_type = ETokenType::Switch;
            switch_value_offset = i + 1;
        }
    }

    ConstructFromToken(token_offset, cmd_line_length, token_type, switch_value_offset);
}

const char* CommandLine::GetOriginal()
{
    return s_original_command_line;
}

bool CommandLine::HasFlag(StringView flag)
{
    return m_flags.Contains(flag);
}

Optional<StringView> CommandLine::GetSwitchValue(StringView switch_name)
{
    for (const Switch& s : m_switches)
    {
        if (switch_name == s.name)
        {
            return s.value;
        }
    }

    return {};
}

void CommandLine::Append(StringView argument)
{
    const Usize offset = ASCIICalls::Length(s_command_line);
    Check(offset + argument.BytesCount() < MaxCommandLineSize);
    Memory::Copy(s_command_line + offset, *argument, argument.BytesCount());
    s_command_line[offset + argument.BytesCount()] = 0;
}

void CommandLine::ConstructFromToken(Usize token_begin, Usize token_end, ETokenType type, Usize switch_value_offset /*= 0*/)
{
    if (type == ETokenType::Switch)
    {
        StringView name = StringView(s_command_line + token_begin, switch_value_offset - token_begin - 1);
        StringView value = StringView(s_command_line + switch_value_offset, token_end - switch_value_offset);
        m_switches.Add({ name, value });
    }
    else
    {
        StringView flag = StringView(s_command_line + token_begin, token_end - token_begin);
        m_flags.Add(flag);
    }
}

} // namespace Basalt
