// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/CoreDefines.h"
#include "Core/CoreTypes.h"
#include "Core/Misc/DateTime.h"

namespace Basalt
{

class Platform
{
public:
    static void GetCurrentDateTime(DateTime* out_date_time);
};

} // namespace Basalt
