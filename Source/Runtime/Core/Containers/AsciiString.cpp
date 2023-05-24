// Copyright(c) 2023 Basalt Technology Group.All rights reserved.

#include "AsciiString.h"

namespace Basalt
{

Usize ASCIICalls::Length(const char* string)
{
    const char* first = string;
    while (*(string++)) {}
    return (string - first) - 1;
}

} // namespace Basalt