// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "MathUtilities.h"
#include "Vector3.h"
#include "Vector4.h"

namespace Basalt
{
namespace Math
{

/**
 * Structure that represents a 4x4 matrix with floating point precision.
 * The matrix is stored in memory as a row-major matrix, so each basis
 * vector is stored as a row.
 * 
 * @tparam T The type of the matrix elements. Must be a floating point type.
 */
template<typename T>
struct BASALT_S_API Matrix4
{
public:
    /** @return A 4x4 identity matrix. */
    FORCEINLINE static constexpr Matrix4 Identity();

/** Transformations. */
public:
    FORCEINLINE static Matrix4 Translate(const Vector4<T>& translation);

    FORCEINLINE static Matrix4 Translate(const Vector3<T>& translation);

    FORCEINLINE static Matrix4 Scale(float scaleFactor);

    FORCEINLINE static Matrix4 Scale(const Vector3<T>& scale);

    FORCEINLINE static Matrix4 RotateX(float angleX);

    FORCEINLINE static Matrix4 RotateY(float angleY);

    FORCEINLINE static Matrix4 RotateZ(float angleZ);

    FORCEINLINE static Matrix4 RotateRollPitchYaw(float roll, float pitch, float yaw);

    FORCEINLINE static Matrix4 PerspectiveRH(float fov, float aspectRatio, float nearZ, float farZ);

    FORCEINLINE static Matrix4 PerspectiveLH(float fov, float aspectRatio, float nearZ, float farZ);

    FORCEINLINE static Matrix4 OrthographicRH(float viewWidth, float viewHeight, float nearZ, float farZ);

    FORCEINLINE static Matrix4 OrthographicLH(float viewWidth, float viewHeight, float nearZ, float farZ);

public:
    /** Default constructor. Initializes all elements with zero. */
    FORCEINLINE constexpr Matrix4();

    /**
     * Constructs a matrix based on the given basis vectors.
     * 
     * @param x The basis vector on the X-axis.
     * @param y The basis vector on the Y-axis.
     * @param z The basis vector on the Z-axis.
     * @param w The basis vector on the W-axis.
     */
    FORCEINLINE constexpr Matrix4(const Vector4<T>& x, const Vector4<T>& y, const Vector4<T> z, const Vector4<T>& w);

    /**
     * Copy constructor. Copies each element from another matrix.
     * 
     * @param other The matrix to copy.
     */
    FORCEINLINE Matrix4(const Matrix4& other);

    /**
     * Copy assignment operator. Copies each element from another matrix.
     * 
     * @param other The matrix to copy.
     * @return Reference to this after the copy operation.
     */
    FORCEINLINE Matrix4& operator=(const Matrix4& other);

public:
    FORCEINLINE Matrix4 Inverse() const;
    FORCEINLINE Matrix4 Transpose() const;

public:
    FORCEINLINE Matrix4 operator*(const Matrix4& rhs) const;

public:
    union
    {
        /** The rows of the matrix. Also interpreted as the basis vectors. */
        Vector4<T> rows[4];

        /** The raw matrix memory representation. */
        T m[4][4];
    };
};

template<typename T>
FORCEINLINE Vector4<T> operator*(const Vector4<T>& vector, const Matrix4<T>& matrix);

template<typename T>
FORCEINLINE Vector3<T> operator*(const Vector3<T>& vector, const Matrix4<T>& matrix);

} // namespace Basalt::Math

/** A 4x4 matrix with 32-bit floating point precision. */
using Matrix4f = Math::Matrix4<F32>;

/** A 4x4 matrix with 64-bit floating point precision. */
using Matrix4d = Math::Matrix4<F64>;

} // namespace Basalt

/** Definitions of the Matrix4 functions. */
#include "Matrix4.inl"
