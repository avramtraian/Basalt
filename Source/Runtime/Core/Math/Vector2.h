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
struct Vector3;
template<typename T>
struct Vector4;

/**
 * A vector in 2-dimensional space, composed of X, Y components.
 * Has a floating point precision.
 * 
 * @tparam T The floating point precision of the vector. Must be float or double.
 */
template<typename T>
struct BASALT_S_API Vector2
{
public:
    /**
     * Calculates the dot product between two vectors.
     *
     * @param rhs The first vector.
     * @param lhs The second vector.
     *
     * @return The dot product.
     */
    FORCEINLINE static T Dot(const Vector2& lhs, const Vector2& rhs)
    {
        return (lhs.x * rhs.x) + (lhs.y * rhs.y);
    }

    /**
     * Normalizes a vector. The normal of a vector has the same length
     * as the original vector, but it is unit length.
     *
     * @param value The vector to normalize.
     *
     * @return The normalized vector.
     */
    FORCEINLINE static Vector2 Normalize(const Vector2& value)
    {
        const Vector2 result = value / value.Length();
        return result;
    }

    /** @return A vector with all components initialized with 0. */
    FORCEINLINE constexpr static Vector2 Zero() { return { T(0), T(0) }; }

    /** @return An unit vector that points along the X-axis. */
    FORCEINLINE constexpr static Vector2 UnitX() { return { T(1), T(0) }; }

    /** @return An unit vector that points along the Y-axis. */
    FORCEINLINE constexpr static Vector2 UnitY() { return { T(0), T(1) }; }

public:
    /**
     * Default constructor. Initializes each components with 0.
     */
    FORCEINLINE Vector2()
        : x(T(0))
        , y(T(0))
    {}

    /**
     * Copy constructor. Uses component-wise copy.
     *
     * @param other The vector to copy.
     */
    FORCEINLINE Vector2(const Vector2& other)
        : x(other.x)
        , y(other.y)
    {}

    /**
     * Initializes each component with its specified value.
     *
     * @param inX The value to initialize the X-component with.
     * @param inY The value to initialize the Y-component with.
     */
    FORCEINLINE Vector2(T inX, T inY)
        : x(inX)
        , y(inY)
    {}

    /**
     * Initializes each component with the same value.
     *
     * @param scalar The value to initialize the components with.
     */
    FORCEINLINE explicit Vector2(T scalar)
        : x(scalar)
        , y(scalar)
    {}

    /**
     * Copy/Assignment operator. Uses component-wise copy.
     *
     * @param other The vector to copy.
     * @return Reference to this, after the assignment.
     */
    FORCEINLINE Vector2& operator=(const Vector2& other)
    {
        x = other.x;
        y = other.y;
        return *this;
    }

public:
    /**
     * Converts a 3-component vector to a 2-component vector.
     * This operation is performed by dropping the Z-component from
     * the 3-component vector and only copying the X and Y components.
     *
     * @param vector3 The 3-component vector used to initialize this.
     */
    FORCEINLINE explicit Vector2(const Vector3<T>& vector3); // Defined in "VectorCommon.h".

    /**
     * Converts a 4-component vector to a 2-component vector.
     * This operation is performed by dropping the Z and W components from
     * the 4-component vector and only copying the X and Y components.
     *
     * @param vector4 The 4-component vector used to initialize this.
     */
    FORCEINLINE explicit Vector2(const Vector4<T>& vector4); // Defined in "VectorCommon.h".

public:
    /** @return The squared length of the vector. */
    FORCEINLINE T LengthSquared() const { return Vector2::Dot(*this, *this); }

    /** @return The length of the vector. */
    FORCEINLINE T Length() const { return Math::Sqrt(LengthSquared()); }

    /** @return True if this vector is unit length (normalized); False otherwise. */
    FORCEINLINE bool IsNormalized() const { return LengthSquared() == T(1); }

    /** @return The normalized version of this vector. */
    FORCEINLINE Vector2 Normalize() const { return Vector2::Normalize(*this); }

    /**
     * Gets the value of the component on a given axis.
     *
     * @param axis The axis to get.
     * @return A reference to the component on the given axis.
     */
    FORCEINLINE T& GetAxis(EAxis axis) { return (&x)[(Usize)axis]; }
    FORCEINLINE const T& GetAxis(EAxis axis) const { return (&x)[(Usize)axis]; }

public:
    /**
     * Component-wise addition between this and another vector.
     *
     * @param rhs The vector to add to this.
     * @return The vector that resulted from the addition operation.
     */
    FORCEINLINE Vector2 operator+(const Vector2& rhs) const
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
    FORCEINLINE Vector2 operator+=(const Vector2& other)
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
    FORCEINLINE Vector2 operator-(const Vector2& rhs) const
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
    FORCEINLINE Vector2 operator-=(const Vector2& other)
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
    FORCEINLINE Vector2 operator*(T scalar) const
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
    FORCEINLINE Vector2 operator*=(T scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    /**
     * Divides each of this vector components with a scalar value.
     *
     * @param scalar The value to divide the components with.
     * @return A new vector resulted from the division (or scaling) operation.
     */
    FORCEINLINE Vector2 operator/(T scalar) const { return (*this) * (T(1) / scalar); }

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
    FORCEINLINE Vector2 operator/=(T scalar) { return (*this) *= (T(1) / scalar); }

public:
    /**
     * Equality check operator.
     *
     * @param other The vector to check against.
     * @return True if the vectors are equivalent; False otherwise.
     */
    FORCEINLINE bool operator==(const Vector2& other) const
    {
        return
            Math::AreNearlyEqual(x, other.x) &&
            Math::AreNearlyEqual(y, other.y);
    }

    /**
     * Inequality check operator.
     *
     * @param other The vector to check against.
     * @return True if the vectors are not equivalent; False otherwise.
     */
    FORCEINLINE bool operator!=(const Vector2& other) const
    {
        return !(*this == other);
    }

public:
    /** The vector's component on the X-axis. */
    T x;

    /** The vector's component on the Y-axis. */
    T y;
};

template<typename T>
FORCEINLINE static Vector2<T> operator*(T scalar, const Vector2<T>& vector)
{
    // Wrapper around the Vector2's scalar multiplication operator.
    return vector * scalar;
}

} // namespace Basalt::Math

/** A 2-component vector with 32-bit floating point precision. */
using Vector2f = Math::Vector2<F32>;

/** A 2-component vector with 64-bit floating point precision. */
using Vector2d = Math::Vector2<F64>;

} // namespace Basalt
