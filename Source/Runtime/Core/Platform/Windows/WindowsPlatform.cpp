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

} // namespace Basalt

#endif // BASALT_PLATFORM_WINDOWS
