// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "Core/CoreDefines.h"

#if BASALT_PLATFORM_WINDOWS

#include "Core/Platform/Platform.h"
#include "Core/Math/MathUtilities.h"

#include <Windows.h>
#include <Windowsx.h>

namespace Basalt
{

struct WindowsPlatformData
{
    HANDLE console_handle = INVALID_HANDLE_VALUE;
};

static WindowsPlatformData s_platform_data;

void Platform::GetCurrentDateTime(DateTime* out_date_time)
{
    Check(out_date_time);

    SYSTEMTIME system_time = {};
    GetLocalTime(&system_time);

    out_date_time->year             = (U32)system_time.wYear;
    out_date_time->month            = (U8)system_time.wMonth;
    out_date_time->month_of_year    = (EMonthOfYear)system_time.wMonth;
    out_date_time->day              = (U8)system_time.wDay;
    out_date_time->day_of_week      = (EDayOfWeek)system_time.wDayOfWeek;
    out_date_time->hour             = (U8)system_time.wHour;
    out_date_time->minute           = (U8)system_time.wMinute;
    out_date_time->second           = (U8)system_time.wSecond;
    out_date_time->millisecond      = (U16)system_time.wMilliseconds;
}

bool Platform::IsConsoleAttached()
{
    return (s_platform_data.console_handle != INVALID_HANDLE_VALUE);
}

void Platform::CreateConsole()
{
    if (Platform::IsConsoleAttached())
    {
        DestroyConsole();
    }

    AllocConsole();
    SetConsoleTitleA("Basalt Editor - Developer console");
    s_platform_data.console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
}

void Platform::DestroyConsole()
{
    if (Platform::IsConsoleAttached())
    {
        FreeConsole();
        s_platform_data.console_handle = INVALID_HANDLE_VALUE;
    }
}

void Platform::WriteToConsole(StringView message, LinearColor text_color, LinearColor background_color)
{
    if (!IsConsoleAttached())
    {
        return;
    }

    WORD attributes = 0;

    if (Math::AreNearlyEqual(text_color.r, 1.0F)) { attributes |= FOREGROUND_RED; }
    if (Math::AreNearlyEqual(text_color.g, 1.0F)) { attributes |= FOREGROUND_GREEN; }
    if (Math::AreNearlyEqual(text_color.b, 1.0F)) { attributes |= FOREGROUND_BLUE; }
    if (Math::AreNearlyEqual(text_color.a, 1.0F)) { attributes |= FOREGROUND_INTENSITY; }

    if (Math::AreNearlyEqual(background_color.r, 1.0F)) { attributes |= BACKGROUND_RED; }
    if (Math::AreNearlyEqual(background_color.g, 1.0F)) { attributes |= BACKGROUND_GREEN; }
    if (Math::AreNearlyEqual(background_color.b, 1.0F)) { attributes |= BACKGROUND_BLUE; }
    if (Math::AreNearlyEqual(background_color.a, 1.0F)) { attributes |= BACKGROUND_INTENSITY; }

    SetConsoleTextAttribute(s_platform_data.console_handle, attributes);
    WriteConsoleA(s_platform_data.console_handle, *message, (DWORD)message.BytesCount(), NULL, NULL);
}

} // namespace Basalt

#endif // BASALT_PLATFORM_WINDOWS
