// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/CoreDefines.h"
#include "Core/CoreTypes.h"

namespace Basalt
{

/** The namespace that contains all templated math objects. */
namespace Geometry
{

/**
 * A vector in 2-dimensional space, composed of X, Y components.
 * Has integer precision.
 * 
 * @tparam T The type of the vector's component. Must be of integer type.
 */
template<typename T>
struct BASALT_S_API IntVector2
{
public:
    /** @return A vector with all components initialized with 0. */
    FORCEINLINE static constexpr IntVector2 Zero() { return IntVector2(T(0), T(0)); }

public:
    /**
     * Default constructor. Initializes each components with 0.
     */
    FORCEINLINE IntVector2()
        : x(T(0))
        , y(T(0))
        , z(T(0))
    {}

    /**
     * Copy constructor. Uses component-wise copy.
     *
     * @param other The vector to copy.
     */
    FORCEINLINE IntVector2(const IntVector2& other)
        : x(other.x)
        , y(other.y)
        , z(other.z)
    {}

    /**
     * Initializes each component with its specified value.
     *
     * @param inX The value to initialize the X-component with.
     * @param inY The value to initialize the Y-component with.
     */
    FORCEINLINE IntVector2(T inX, T inY)
        : x(inX)
        , y(inY)
    {}

    /**
     * Initializes each component with the same value.
     *
     * @param scalar The value to initialize the components with.
     */
    FORCEINLINE IntVector2(T scalar)
        : x(scalar)
        , y(scalar)
    {}

    /**
     * Copy assignment operator. Uses component-wise copy.
     *
     * @param other The vector to copy.
     * @return Reference to this, after the assignment.
     */
    FORCEINLINE IntVector2& operator=(const IntVector2& other)
    {
        x = other.x;
        y = other.y;

        return *this;
    }

public:
    /**
     * Component-wise addition between this and another vector.
     *
     * @param rhs The vector to add to this.
     * @return The vector that resulted from the addition operation.
     */
    FORCEINLINE IntVector2 operator+(const IntVector2& rhs) const
    {
        return { x + rhs.x, y + rhs.y };
    }

    /**
     * Component-wise addition between this and another vector.
     * The result is stored in this vector.
     *
     * @param other The vector to add to this.
     * @return A copy of this vector, after the addition operation.
     */
    FORCEINLINE IntVector2 operator+=(const IntVector2& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    /**
     * Component-wise subtraction between this and another vector.
     *
     * @param rhs The vector to subtract from this.
     * @return The vector that resulted from the subtraction operation.
     */
    FORCEINLINE IntVector2 operator-(const IntVector2& rhs) const
    {
        return { x - rhs.x, y - rhs.y };
    }

    /**
     * Component-wise subtraction between this and another vector.
     * The result is stored in this vector.
     *
     * @param other The vector to subtract from this.
     * @return A copy of this vector, after the subtraction operation.
     */
    FORCEINLINE IntVector2 operator-=(const IntVector2& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    /**
     * Multiplies each of this vector components with a scalar value.
     *
     * @param scalar The value to multiply the components with.
     * @return A new vector resulted from the multiplication (or scaling) operation.
     */
    FORCEINLINE IntVector2 operator*(T scalar) const
    {
        return { x * scalar, y * scalar };
    }

    /**
     * Multiplies each of this vector components with a scalar value.
     * The result is stored in this vector, so this operation acts
     * like a scale.
     *
     * @param scalar The value to multiply the components with. Could
     *               also be interpreted like the scaling multiplier.
     *
     * @return A copy of this, after the multiplication (or scaling) operation.
     */
    FORCEINLINE IntVector2 operator*=(T scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

public:
    /**
     * Equality check operator.
     *
     * @param other The vector to check against.
     * @return True if the vectors are equivalent; False otherwise.
     */
    FORCEINLINE bool operator==(const IntVector2& other) const
    {
        return
            (x == other.x) &&
            (y == other.y);
    }

    /**
     * Inequality check operator.
     *
     * @param other The vector to check against.
     * @return True if the vectors are not equivalent; False otherwise.
     */
    FORCEINLINE bool operator!=(const IntVector2& other) const
    {
        return !(*this == other);
    }

public:
    /** The vector's component on the X-axis. */
    T x;
    
    /** The vector's component on the Y-axis. */
    T y;
};

} // namespace Basalt::Geometry

/** A 3-component vector with 32-bit signed integer precision. */
using IntVector2i = Geometry::IntVector2<I32>;

/** A 3-component vector with 32-bit unsigned integer precision. */
using IntVector2u = Geometry::IntVector2<U32>;

} // namespace Basalt
