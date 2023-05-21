// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/CoreDefines.h"
#include "Core/CoreTypes.h"

#include <cmath>
#include <cfloat>

namespace Basalt
{

/** Four-dimensional space axes. */
enum class Axis
{
    X = 0, Y = 1, Z = 2, W = 3
};

/**
 * Structure that contains all the math utilities function
 * and constants. Each function and constant has two version,
 * with 32-bit precision and 64-bit precision respectively.
 */
struct BASALT_API Math
{
public:
    static constexpr float PI = 3.1415926535F;
    static constexpr float TwoPI = 2.0F * PI;
    static constexpr float HalfPI = 0.5F * PI;

    static constexpr float InvPI = 1.0F / PI;
    static constexpr float InvTwoPI = 1.0F / TwoPI;
    static constexpr float InvHalfPI = 1.0F / HalfPI;

    static constexpr float SmallNumber = 1e-6F;
    static constexpr float KindaSmallNumber = 1e-4F;

    static constexpr float DegToRad = PI / 180.0F;
    static constexpr float RadToDeg = 180.0F / PI;

public:
    template<typename T>
    FORCEINLINE static T Max(T a, T b)
    {
        return (a > b) ? a : b;
    }

    template<typename T>
    FORCEINLINE static T Min(T a, T b)
    {
        return (a < b) ? a : b;
    }

    template<typename T>
    FORCEINLINE static T Clamp(T value, T min, T max)
    {
        return Math::Max(min, Math::Min(max, value));
    }

    template<typename T>
    FORCEINLINE static T Abs(T value)
    {
        return (value > T(0)) ? value : -value;
    }

    template<typename T>
    FORCEINLINE static T Sign(T value)
    {
        return (value > T(0)) ? T(1) : T(-1);
    }

    template<typename T>
    FORCEINLINE static I32 SignAsInt(T value)
    {
        return (value > T(0)) ? 1 : -1;
    }
    
    template<typename T>
    FORCEINLINE static bool AreNearlyEqual(T a, T b, T tolerance = Math::SmallNumber)
    {
        return -tolerance <= (a - b) && (a - b) <= tolerance;
    }

public:
    /**
     * Calculates the square root of a positive number.
     * 
     * @param value The value to compute its square root.
     * @return The square root.
     */
    FORCEINLINE static float Sqrt(float value) { return sqrtf(value); }
    FORCEINLINE static double Sqrt(double value) { return sqrt(value); }

    /**
     * Calculates the sine of an angle.
     * 
     * @param value The angle, expressed in radians.
     * @return The sine of the angle.
     */
    FORCEINLINE static float Sin(float value) { return sinf(value); }
    FORCEINLINE static double Sin(double value) { return sin(value); }

    /**
     * Calculates the cosine of an angle.
     *
     * @param value The angle, expressed in radians.
     * @return The cosine of the angle.
     */
    FORCEINLINE static float Cos(float value) { return cosf(value); }
    FORCEINLINE static double Cos(double value) { return cos(value); }

    /**
     * Calculates the tangent of an angle.
     *
     * @param value The angle, expressed in radians.
     * @return The tangent of the angle.
     */
    FORCEINLINE static float Tan(float value) { return tanf(value); }
    FORCEINLINE static double Tan(double value) { return tan(value); }

    /**
     * Calculates the inverse sine of a number.
     * 
     * @param value The number to calculate its inverse sine.
     * @return The inverse sine, expressed in radians.
     */
    FORCEINLINE static float Asin(float value) { return asinf(value); }
    FORCEINLINE static double Asin(double value) { return asin(value); }

    /**
     * Calculates the inverse cosine of a number.
     *
     * @param value The number to calculate its inverse cosine.
     * @return The inverse cosine, expressed in radians.
     */
    FORCEINLINE static float Acos(float value) { return acosf(value); }
    FORCEINLINE static double Acos(double value) { return acos(value); }

    /**
     * Calculates the inverse tangent of a number.
     *
     * @param value The number to calculate its inverse tangent.
     * @return The inverse tangent, expressed in radians.
     */
    FORCEINLINE static float Atan(float value) { return atanf(value); }
    FORCEINLINE static double Atan(double value) { return atan(value); }
};

} // namespace Basalt
