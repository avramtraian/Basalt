// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/CoreDefines.h"
#include "Core/CoreTypes.h"

namespace Basalt
{
namespace Math
{

/**
 * Enumeration of different kinds of gamma corrections that the engine converts from/to.
 */
enum class EGammeSpace : U8
{
    /** Value that represents an error. */
    Invalid = 0,

    /** No gamma correction will be applied. */
    Linear,
    
    /** A simplified sRGB gamma correction. Raises the color value to the 1/2.2 power. */
    Pow22,

    /** Use the standard sRGB gamma correction. */
    sRGB,
};

/**
 * A linear RGBA color, with 32-bit per component.
 */
struct BASALT_S_API LinearColor
{
public:
    /**
     * Linearly interpolation between two linear colors.
     * 
     * @param a The first color.
     * @param b The second color.
     * @param t The percentage of the interpolation. When this is 0, the returned color is
     *          `a` and when it is 1, the returned color is `b`.
     * 
     * @return The color resulted from the interpolation.
     */
    FORCEINLINE static LinearColor Lerp(const LinearColor& a, const LinearColor& b, float t)
    {
        LinearColor result;
        result.r = a.r + t * (b.r - a.r);
        result.g = a.g + t * (b.g - a.g);
        result.b = a.b + t * (b.b - a.b);
        result.a = a.a + t * (b.a - a.a);
        return result;
    }

    /** @return A white linear color. */
    FORCEINLINE static constexpr LinearColor White() { return LinearColor(1.0F, 1.0F, 1.0F); }
   
    /** @return A black linear color. */
    FORCEINLINE static constexpr LinearColor Black() { return LinearColor(0.0F, 0.0F, 0.0F); }
    
    /** @return A pure red linear color. */
    FORCEINLINE static constexpr LinearColor PureRed() { return LinearColor(1.0F, 0.0F, 0.0F); }
    
    /** @return A pure green linear color. */
    FORCEINLINE static constexpr LinearColor PureGreen() { return LinearColor(0.0F, 1.0F, 0.0F); }
    
    /** @return A pure blue linear color. */
    FORCEINLINE static constexpr LinearColor PureBlue() { return LinearColor(0.0F, 0.0F, 1.0F); }

    /** @return A red linear color. */
    FORCEINLINE static constexpr LinearColor Red() { return LinearColor(0.9F, 0.1F, 0.2F); }

    /** @return A green linear color. */
    FORCEINLINE static constexpr LinearColor Green() { return LinearColor(0.2F, 0.9F, 0.1F); }

    /** @return A blue linear color. */
    FORCEINLINE static constexpr LinearColor Blue() { return LinearColor(0.1F, 0.2F, 0.9F); }

public:
    /** Default constructor. Initializes all components with 0. */
    FORCEINLINE constexpr LinearColor()
        : r(0.0F)
        , g(0.0F)
        , b(0.0F)
        , a(0.0F)
    {}

    /**
     * Copy constructor. Uses component-wise copy.
     * 
     * @param other The color to copy.
     */
    FORCEINLINE constexpr LinearColor(const LinearColor& other)
        : r(other.r)
        , g(other.g)
        , b(other.b)
        , a(other.a)
    {}

    /**
     * Constructor that initializes each component with the corresponding value.
     * 
     * @param red The value to initialize the red component with.
     * @param green The value to initialize the green component with.
     * @param blue The value to initialize the blue component with.
     * @param alpha The value to initialize the alpha component with.
     */
    FORCEINLINE constexpr LinearColor(float red, float green, float blue, float alpha)
        : r(red)
        , g(green)
        , b(blue)
        , a(alpha)
    {}

    /**
     * Constructor that initializes the RGB components with the corresponding value.
     * The alpha component is initialized with 1.0.
     *
     * @param red The value to initialize the red component with.
     * @param green The value to initialize the green component with.
     * @param blue The value to initialize the blue component with.
     */
    FORCEINLINE constexpr LinearColor(float red, float green, float blue)
        : r(red)
        , g(green)
        , b(blue)
        , a(1.0F)
    {}

    /**
     * Constructor that initializes all components with the given value.
     * 
     * @param scalar The value to initialize the components with.
     */
    FORCEINLINE constexpr LinearColor(float scalar)
        : r(scalar)
        , g(scalar)
        , b(scalar)
        , a(scalar)
    {}

    /**
     * Constructor that initializes the RGB component with the same value.
     * 
     * @param rgb The value to initialize the RGB components with.
     * @param alpha The value to initialize the alpha component with.
     */
    FORCEINLINE LinearColor(float rgb, float alpha)
        : r(rgb)
        , g(rgb)
        , b(rgb)
        , a(alpha)
    {}

    /**
     * Copy assignment operator. Uses component-wise copy.
     * 
     * @param other The color to copy.
     * @return Reference to this, after the copy operation.
     */
    FORCEINLINE LinearColor& operator=(const LinearColor& other)
    {
        r = other.r;
        g = other.g;
        b = other.b;
        a = other.a;

        return *this;
    }

public:
    /**
     * Component-wise addition between this and another color.
     *
     * @param rhs The color to add to this.
     * @return The color that resulted from the addition operation.
     */
    FORCEINLINE LinearColor operator+(const LinearColor& rhs) const
    {
        return { r + rhs.r, g + rhs.g, b + rhs.b, a + rhs.a };
    }

    /**
     * Component-wise addition between this and another color.
     * The result is stored in this color.
     *
     * @param other The color to add to this.
     * @return A copy of this color, after the addition operation.
     */
    FORCEINLINE LinearColor operator+=(const LinearColor& rhs)
    {
        r += rhs.r;
        g += rhs.g;
        b += rhs.b;
        a += rhs.a;
        return *this;
    }

    /**
     * Component-wise subtraction between this and another color.
     *
     * @param rhs The color to subtract from this.
     * @return The color that resulted from the subtraction operation.
     */
    FORCEINLINE LinearColor operator-(const LinearColor& rhs) const
    {
        return { r - rhs.r, g - rhs.g, b - rhs.b, a - rhs.a };
    }

    /**
     * Component-wise subtraction between this and another color.
     * The result is stored in this color.
     *
     * @param other The color to subtract from this.
     * @return A copy of this color, after the subtraction operation.
     */
    FORCEINLINE LinearColor operator-=(const LinearColor& rhs)
    {
        r -= rhs.r;
        g -= rhs.g;
        b -= rhs.b;
        a -= rhs.a;
        return *this;
    }

public:
    /** The colors components. */
    float r, g, b, a;
};

} // namespace Basalt::Math

using Math::EGammeSpace;
using Math::LinearColor;

} // namespace Basalt
