// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/CoreDefines.h"
#include "Core/CoreTypes.h"
#include "MathUtilities.h"

namespace Basalt
{
namespace Math
{

// Forward declaration. Used for conversion constructors.
template<typename T>
struct Vector2;
template<typename T>
struct Vector3;

/**
 * A vector in 4-dimensional space, composed of X, Y, Z, W components.
 * Has a floating point precision.
 * 
 * @tparam T The floating point precision of the vector. Must be float or double.
 */
template<typename T>
struct BASALT_S_API Vector4
{
public:
    /** @return A vector with all components initialized with 0. */
    FORCEINLINE constexpr static Vector4 Zero() { return { T(0), T(0), T(0), T(0) }; }

    /** @return An unit vector that points along the X-axis. */
    FORCEINLINE constexpr static Vector4 UnitX() { return { T(1), T(0), T(0), T(0) }; }
    
    /** @return An unit vector that points along the Y-axis. */
    FORCEINLINE constexpr static Vector4 UnitY() { return { T(0), T(1), T(0), T(0) }; }
    
    /** @return An unit vector that points along the Z-axis. */
    FORCEINLINE constexpr static Vector4 UnitZ() { return { T(0), T(0), T(1), T(0) }; }

    /** @return An unit vector that points along the Z-axis. */
    FORCEINLINE constexpr static Vector4 UnitW() { return { T(0), T(0), T(0), T(1) }; }

public:
    /**
     * Default constructor. Initializes each components with 0.
     */
    FORCEINLINE Vector4()
        : x(T(0))
        , y(T(0))
        , z(T(0))
        , w(T(0))
    {}

    /**
     * Copy constructor. Uses component-wise copy.
     * 
     * @param other The vector to copy.
     */
    FORCEINLINE Vector4(const Vector4& other)
        : x(other.x)
        , y(other.y)
        , z(other.z)
        , w(other.w)
    {}

    /**
     * Initializes each component with its specified value.
     * 
     * @param inX The value to initialize the X-component with.
     * @param inY The value to initialize the Y-component with.
     * @param inZ The value to initialize the Z-component with.
     * @param inW The value to initialize the W-component with.
     */
    FORCEINLINE Vector4(T inX, T inY, T inZ, T inW)
        : x(inX)
        , y(inY)
        , z(inZ)
        , w(inW)
    {}

    /**
     * Initializes each component with the same value.
     * 
     * @param scalar The value to initialize the components with.
     */
    FORCEINLINE explicit Vector4(T scalar)
        : x(scalar)
        , y(scalar)
        , z(scalar)
        , w(scalar)
    {}

    /**
     * Copy/Assignment operator. Uses component-wise copy.
     * 
     * @param other The vector to copy.
     * @return Reference to this, after the assignment.
     */
    FORCEINLINE Vector4& operator=(const Vector4& other)
    {
        x = other.x;
        y = other.y;
        z = other.z;
        w = other.w;
        return *this;
    }

public:
    /**
     * Converts a 2-component vector to a 4-component vector.
     * This operation is performed by copying the X and Y components
     * from the 2-component vector and initializing the Z and W components with 0.
     * 
     * @param vector2 The 2-component vector used to initialize this.
     */
    FORCEINLINE explicit Vector4(const Vector2<T>& vector2); // Defined in "VectorCommon.h".

    /**
     * Converts a 2-component vector to a 4-component vector.
     * This operation is performed by copying the X and Y components
     * from the 2-component vector and initializing Z and W with the
     * given values.
     * 
     * @param vector2 The 2-component vector used to initialize this.
     * @param inZ The value used to initialize the Z-component.
     * @param inZ The value used to initialize the W-component.
     */
    FORCEINLINE Vector4(const Vector2<T>& vector2, T inZ, T inW); // Defined in "VectorCommon.h".

    /**
     * Converts a 3-component vector to a 4-component vector.
     * This operation is performed by copying the X, Y and Z components
     * from the 3-component vector and initializing the W-component with 0.
     * 
     * @param vector3 The 3-component vector used to initialize this.
     */
    FORCEINLINE explicit Vector4(const Vector3<T>& vector3); // Defined in "VectorCommon.h".

    /**
     * Converts a 3-component vector to a 4-component vector.
     * This operation is performed by copying the X, Y and Z components
     * from the 3-component vector and initializing the W-component with
     * the given value.
     * 
     * @param vector3 The 3-component vector used to initialize this.
     * @param inW The value used to initialize the W-component.
     */
    FORCEINLINE Vector4(const Vector3<T>& vector3, T inW); // Defined in "VectorCommon.h".

public:
    /**
     * Gets the value of the component on a given axis.
     *
     * @param axis The axis to get.
     * @return A reference to the component on the given axis.
     */
    FORCEINLINE T& GetAxis(Axis axis) { return (&x)[(Usize)axis]; }
    FORCEINLINE const T& GetAxis(Axis axis) const { return (&x)[(Usize)axis]; }

public:
    /**
     * Component-wise addition between this and another vector.
     * 
     * @param rhs The vector to add to this.
     * @return The vector that resulted from the addition operation.
     */
    FORCEINLINE Vector4 operator+(const Vector4& rhs) const
    {
        return { x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w };
    }

    /**
     * Component-wise addition between this and another vector.
     * The result is stored in this vector.
     * 
     * @param other The vector to add to this.
     * @return A copy of this vector, after the addition operation.
     */
    FORCEINLINE Vector4 operator+=(const Vector4& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
        return *this;
    }

    /**
     * Component-wise subtraction between this and another vector.
     *
     * @param rhs The vector to subtract from this.
     * @return The vector that resulted from the subtraction operation.
     */
    FORCEINLINE Vector4 operator-(const Vector4& rhs) const
    {
        return { x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w };
    }

    /**
     * Component-wise subtraction between this and another vector.
     * The result is stored in this vector.
     *
     * @param other The vector to subtract from this.
     * @return A copy of this vector, after the subtraction operation.
     */
    FORCEINLINE Vector4 operator-=(const Vector4& other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
        return *this;
    }

    /**
     * Multiplies each of this vector components with a scalar value.
     * 
     * @param scalar The value to multiply the components with.
     * @return A new vector resulted from the multiplication (or scaling) operation.
     */
    FORCEINLINE Vector4 operator*(T scalar) const
    {
        return { x * scalar, y * scalar, z * scalar, w * scalar };
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
    FORCEINLINE Vector4 operator*=(T scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        w *= scalar;
        return *this;
    }

    /**
     * Divides each of this vector components with a scalar value.
     * 
     * @param scalar The value to divide the components with.
     * @return A new vector resulted from the division (or scaling) operation.
     */
    FORCEINLINE Vector4 operator/(T scalar) const { return (*this) * (T(1) / scalar); }

    /**
     * Divides each of this vector components with a scalar value.
     * The result is stored in this vector, so this operation acts
     * like a scale.
     *
     * @param scalar The value to divide the components with. Could
     *               also be interpreted like the scaling multiplier.
     * 
     * @return A copy of this, after the multiplication (or scaling) operation.
     */
    FORCEINLINE Vector4 operator/=(T scalar) { return (*this) *= (T(1) / scalar); }

public:
    /**
     * Equality check operator.
     * 
     * @param other The vector to check against.
     * @return True if the vectors are equivalent; False otherwise.
     */
    FORCEINLINE bool operator==(const Vector4& other) const
    {
        return
            Math::AreNearlyEqual(x, other.x) &&
            Math::AreNearlyEqual(y, other.y) &&
            Math::AreNearlyEqual(z, other.z) &&
            Math::AreNearlyEqual(w, other.w);
    }

    /**
     * Inequality check operator.
     *
     * @param other The vector to check against.
     * @return True if the vectors are not equivalent; False otherwise.
     */
    FORCEINLINE bool operator!=(const Vector4& other) const
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

    /** The vector's component on the W-axis. */
    T w;
};

template<typename T>
FORCEINLINE static Vector4<T> operator*(T scalar, const Vector4<T>& vector)
{
    // Wrapper around the Vector4's scalar multiplication operator.
    return vector * scalar;
}

} // namespace Basalt::Math

/** A 4-component vector with 32-bit floating point precision. */
using Vector4f = Math::Vector4<F32>;

/** A 4-component vector with 64-bit floating point precision. */
using Vector4d = Math::Vector4<F64>;

} // namespace Basalt
