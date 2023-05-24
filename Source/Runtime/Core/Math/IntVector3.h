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
 * A vector in 3-dimensional space, composed of X, Y, Z components.
 * Has integer precision.
 * 
 * @tparam T The type of the vector's component. Must be of integer type.
 */
template<typename T>
struct BASALT_S_API IntVector3
{
public:
    /** @return A vector with all components initialized with 0. */
    FORCEINLINE static constexpr IntVector3 Zero() { return IntVector3(T(0), T(0), T(0)); }

public:
    /**
     * Default constructor. Initializes each components with 0.
     */
    FORCEINLINE IntVector3()
        : x(T(0))
        , y(T(0))
        , z(T(0))
    {}

    /**
     * Copy constructor. Uses component-wise copy.
     *
     * @param other The vector to copy.
     */
    FORCEINLINE IntVector3(const IntVector3& other)
        : x(other.x)
        , y(other.y)
        , z(other.z)
    {}

    /**
     * Initializes each component with its specified value.
     *
     * @param inX The value to initialize the X-component with.
     * @param inY The value to initialize the Y-component with.
     * @param inZ The value to initialize the Z-component with.
     */
    FORCEINLINE IntVector3(T inX, T inY, T inZ)
        : x(inX)
        , y(inY)
        , z(inZ)
    {}

    /**
     * Initializes each component with the same value.
     *
     * @param scalar The value to initialize the components with.
     */
    FORCEINLINE IntVector3(T scalar)
        : x(scalar)
        , y(scalar)
        , z(scalar)
    {}

    /**
     * Copy assignment operator. Uses component-wise copy.
     *
     * @param other The vector to copy.
     * @return Reference to this, after the assignment.
     */
    FORCEINLINE IntVector3& operator=(const IntVector3& other)
    {
        x = other.x;
        y = other.y;
        z = other.z;

        return *this;
    }

public:
    /**
     * Component-wise addition between this and another vector.
     *
     * @param rhs The vector to add to this.
     * @return The vector that resulted from the addition operation.
     */
    FORCEINLINE IntVector3 operator+(const IntVector3& rhs) const
    {
        return { x + rhs.x, y + rhs.y, z + rhs.z };
    }

    /**
     * Component-wise addition between this and another vector.
     * The result is stored in this vector.
     *
     * @param other The vector to add to this.
     * @return A copy of this vector, after the addition operation.
     */
    FORCEINLINE IntVector3 operator+=(const IntVector3& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    /**
     * Component-wise subtraction between this and another vector.
     *
     * @param rhs The vector to subtract from this.
     * @return The vector that resulted from the subtraction operation.
     */
    FORCEINLINE IntVector3 operator-(const IntVector3& rhs) const
    {
        return { x - rhs.x, y - rhs.y, z - rhs.z };
    }

    /**
     * Component-wise subtraction between this and another vector.
     * The result is stored in this vector.
     *
     * @param other The vector to subtract from this.
     * @return A copy of this vector, after the subtraction operation.
     */
    FORCEINLINE IntVector3 operator-=(const IntVector3& other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    /**
     * Multiplies each of this vector components with a scalar value.
     *
     * @param scalar The value to multiply the components with.
     * @return A new vector resulted from the multiplication (or scaling) operation.
     */
    FORCEINLINE IntVector3 operator*(T scalar) const
    {
        return { x * scalar, y * scalar, z * scalar };
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
    FORCEINLINE IntVector3 operator*=(T scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

public:
    /**
     * Equality check operator.
     *
     * @param other The vector to check against.
     * @return True if the vectors are equivalent; False otherwise.
     */
    FORCEINLINE bool operator==(const IntVector3& other) const
    {
        return
            (x == other.x) &&
            (y == other.y) &&
            (z == other.z);
    }

    /**
     * Inequality check operator.
     *
     * @param other The vector to check against.
     * @return True if the vectors are not equivalent; False otherwise.
     */
    FORCEINLINE bool operator!=(const IntVector3& other) const
    {
        return !(*this == other);
    }

public:
    /** The vector's component on the X-axis. */
    T x;
    
    /** The vector's component on the Y-axis. */
    T y;
    
    /** The vector's component on the Z-axis. */
    T z;
};

} // namespace Basalt::Geometry

/** A 3-component vector with 32-bit signed integer precision. */
using IntVector3i = Geometry::IntVector3<I32>;

/** A 3-component vector with 32-bit unsigned integer precision. */
using IntVector3u = Geometry::IntVector3<U32>;

} // namespace Basalt
