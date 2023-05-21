// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/CoreDefines.h"
#include "Core/CoreTypes.h"
#include "MathUtilities.h"

namespace Basalt
{

/** The namespace that contains all templated math objects. */
namespace Geometry
{

/**
 * A vector in 3-dimensional space, composed of X, Y, Z components.
 * Has a floating point precision.
 * 
 * @tparam T The floating point precision of the vector. Must be float or double.
 */
template<typename T>
struct BASALT_S_API Vector3
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
    FORCEINLINE static T Dot(const Vector3& rhs, const Vector3& lhs)
    {
        const T result = (rhs.x * lhs.x) + (rhs.y * lhs.y) + (rhs.z * lhs.z);
        return result;
    }

    /**
     * Calculates the cross product between two vectors.
     *
     * @param rhs The first vector.
     * @param lhs The second vector.
     *
     * @return The cross product.
     */
    FORCEINLINE static Vector3 Cross(const Vector3& rhs, const Vector3& lhs)
    {
        const Vector3 result = Vector3
        (
            rhs.y * lhs.z - rhs.z * lhs.y,
            rhs.z * lhs.x - rhs.x * lhs.z,
            rhs.x * lhs.y - rhs.y * lhs.x
        );
        return result;
    }

    /**
     * Normalizes a vector. The normal of a vector has the same length
     * as the original vector, but it is unit length.
     * 
     * @param value The vector to normalize.
     * 
     * @return The normalized vector.
     */
    FORCEINLINE static Vector3 Normalize(const Vector3& value)
    {
        const Vector3 result = value / value.Length();
        return result;
    }

    /** @return A vector with all components initialized with 0. */
    FORCEINLINE constexpr static Vector3 Zero() { return { T(0), T(0), T(0) }; }

    /** @return An unit vector that points along the X-axis. */
    FORCEINLINE constexpr static Vector3 UnitX() { return { T(1), T(0), T(0) }; }
    
    /** @return An unit vector that points along the Y-axis. */
    FORCEINLINE constexpr static Vector3 UnitY() { return { T(0), T(1), T(0) }; }
    
    /** @return An unit vector that points along the Z-axis. */
    FORCEINLINE constexpr static Vector3 UnitZ() { return { T(0), T(0), T(1) }; }

public:
    /**
     * Default constructor. Initializes each components with 0.
     */
    FORCEINLINE Vector3()
        : x(T(0))
        , y(T(0))
        , z(T(0))
    {}

    /**
     * Copy constructor. Uses component-wise copy.
     * 
     * @param other The vector to copy.
     */
    FORCEINLINE Vector3(const Vector3& other)
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
    FORCEINLINE Vector3(T inX, T inY, T inZ)
        : x(inX)
        , y(inY)
        , z(inZ)
    {}

    /**
     * Initializes each component with the same value.
     * 
     * @param scalar The value to initialize the components with.
     */
    FORCEINLINE Vector3(T scalar)
        : x(scalar)
        , y(scalar)
        , z(scalar)
    {}

    /**
     * Copy/Assignment operator. Uses component-wise copy.
     * 
     * @param other The vector to copy.
     * @return Reference to this, after the assignment.
     */
    FORCEINLINE Vector3& operator=(const Vector3& other)
    {
        x = other.x;
        y = other.y;
        z = other.z;
        return *this;
    }

public:
    /** @return The squared length of the vector. */
    FORCEINLINE T LengthSquared() const { return Vector3::Dot(*this, *this); }

    /** @return The length of the vector. */
    FORCEINLINE T Length() const { return Math::Sqrt(LengthSquared()); }

    /** @return True if this vector is unit length (normalized); False otherwise. */
    FORCEINLINE bool IsNormalized() const { return LengthSquared() == T(1); }

    /** @return The normalized version of this vector. */
    FORCEINLINE Vector3 Normalize() const { return Vector3::Normalize(*this); }

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
    FORCEINLINE Vector3 operator+(const Vector3& rhs) const
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
    FORCEINLINE Vector3 operator+=(const Vector3& other)
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
    FORCEINLINE Vector3 operator-(const Vector3& rhs) const
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
    FORCEINLINE Vector3 operator-=(const Vector3& other)
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
    FORCEINLINE Vector3 operator*(T scalar) const
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
    FORCEINLINE Vector3 operator*=(T scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    /**
     * Divides each of this vector components with a scalar value.
     * 
     * @param scalar The value to divide the components with.
     * @return A new vector resulted from the division (or scaling) operation.
     */
    FORCEINLINE Vector3 operator/(T scalar) const { return (*this) * (T(1) / scalar); }

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
    FORCEINLINE Vector3 operator/=(T scalar) { return (*this) *= (T(1) / scalar); }

public:
    /**
     * Equality check operator.
     * 
     * @param other The vector to check against.
     * @return True if the vectors are equivalent; False otherwise.
     */
    FORCEINLINE bool operator==(const Vector3& other) const
    {
        return
            Math::AreNearlyEqual(x, other.x) &&
            Math::AreNearlyEqual(y, other.y) &&
            Math::AreNearlyEqual(z, other.z);
    }

    /**
     * Inequality check operator.
     *
     * @param other The vector to check against.
     * @return True if the vectors are not equivalent; False otherwise.
     */
    FORCEINLINE bool operator!=(const Vector3& other) const
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

template<typename T>
FORCEINLINE static Vector3<T> operator*(T scalar, const Vector3<T>& vector)
{
    // Wrapper around the Vector3's scalar multiplication operator.
    return vector * scalar;
}

} // namespace Basalt::Geometry

/** A 3-component vector with 32-bit floating point precision. */
using Vector3f = Geometry::Vector3<F32>;

/** A 3-component vector with 64-bit floating point precision. */
using Vector3d = Geometry::Vector3<F64>;

} // namespace Basalt
