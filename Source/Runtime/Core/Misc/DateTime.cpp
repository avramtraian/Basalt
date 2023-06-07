// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "DateTime.h"

#include "Core/Platform/Platform.h"

namespace Basalt
{

DateTime DateTime::Now()
{
    DateTime result = {};
    Platform::GetCurrentDateTime(&result);
    return result;
}

} // namespace Basalt
