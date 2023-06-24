// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/CoreDefines.h"
#include "Core/CoreTypes.h"
#include "Core/Containers/Strings/StringView.h"
#include "Core/Math/Color.h"
#include "Core/Misc/DateTime.h"

namespace Basalt
{

class Platform
{
public:
    static bool IsConsoleAttached();

    static void CreateConsole();

    static void DestroyConsole();

    static void WriteToConsole(StringView message, LinearColor text_color, LinearColor background_color);

public:
    static String GetExecutablePath();

public:
    static void GetCurrentDateTime(DateTime* out_date_time);
};

} // namespace Basalt
