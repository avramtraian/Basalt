// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/CoreDefines.h"

namespace Basalt
{

struct BASALT_S_API WindowsPlatformTypes
{
    using U8    = unsigned char;
    using U16   = unsigned short;
    using U32   = unsigned int;
    using U64   = unsigned long long;
    using I8    = signed char;
    using I16   = signed short;
    using I32   = signed int;
    using I64   = signed long long;
    using F32   = float;
    using F64   = double;
    using B8    = bool;
    using B32   = signed int;
    using Usize = unsigned long long;
    using Ssize = signed long long;
};

} // namespace Basalt
