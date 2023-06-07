// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/CoreDefines.h"
#include "Core/CoreTypes.h"

namespace Basalt
{

/**
 * Enumeration of the days of week, from Monday to Sunday.
 */
enum class EDayOfWeek : U8
{
    Monday = 1,
    Tuesday,
    Wednesday,
    Thursday,
    Friday,
    Saturday,
    Sunday,
};

/**
 * Enumeration of the months of the year, from January to December.
 */
enum class EMonthOfYear
{
    January = 1,
    February,
    March,
    April,
    May,
    June,
    July,
    August,
    September,
    October,
    November,
    December,
};

/**
 * Structure that represents a point in time, expressed as a calendar date.
 */
struct BASALT_API DateTime
{
public:
    /**
     * Gets the current local time of the system.
     */
    static DateTime Now();

public:
    /** The year of the date. */
    U32 year;

    /** The month of the date. */
    U8 month;

    /** The month of the date, expressed as a month of the year. */
    EMonthOfYear month_of_year;

    /** The day of the date. */
    U8 day;

    /** The day of the date, expressed as a day of the week. */
    EDayOfWeek day_of_week;
    
    /** The hour of the date. */
    U8 hour;

    /** The minute of the date. */
    U8 minute;

    /** The second of the date. */
    U8 second;

    /** The millisecond of the date. */
    U16 millisecond;
};

} // namespace Basalt
