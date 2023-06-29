// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/CoreDefines.h"
#include "Core/CoreTypes.h"
#include "Core/Misc/AssertionMacros.h"

namespace Basalt
{

class BASALT_API Hasher
{
public:
    template<typename T>
    static U64 Compute(const T& value);
};

} // namespace Basalt
