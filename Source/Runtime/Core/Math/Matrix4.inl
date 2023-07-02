// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#if BT_PLATFORM_WINDOWS
    #include "MathDirectX.h"
#endif // BT_PLATFORM_WINDOWS

namespace Basalt
{
namespace Math
{

template<typename T>
FORCEINLINE constexpr Matrix4<T> Matrix4<T>::Identity()
{
    return Matrix4(
        Vector4<T>::UnitX(),
        Vector4<T>::UnitY(),
        Vector4<T>::UnitZ(),
        Vector4<T>::UnitW()
    );
}

template<typename T>
FORCEINLINE constexpr Matrix4<T>::Matrix4()
    : m{}
{}

template<typename T>
FORCEINLINE constexpr Matrix4<T>::Matrix4(const Vector4<T>& x, const Vector4<T>& y, const Vector4<T> z, const Vector4<T>& w)
{
    rows[EAxis_X] = x;
    rows[EAxis_Y] = y;
    rows[EAxis_Z] = z;
    rows[EAxis_W] = w;
}

template<typename T>
FORCEINLINE Matrix4<T>::Matrix4(const Matrix4& other)
{
    rows[EAxis_X] = other.rows[EAxis_X];
    rows[EAxis_Y] = other.rows[EAxis_Y];
    rows[EAxis_Z] = other.rows[EAxis_Z];
    rows[EAxis_W] = other.rows[EAxis_W];
}

template<typename T>
FORCEINLINE Matrix4<T>& Matrix4<T>::operator=(const Matrix4& other)
{
    rows[EAxis_X] = other.rows[EAxis_X];
    rows[EAxis_Y] = other.rows[EAxis_Y];
    rows[EAxis_Z] = other.rows[EAxis_Z];
    rows[EAxis_W] = other.rows[EAxis_W];

    return *this;
}

template<typename T>
FORCEINLINE Matrix4<T> Matrix4<T>::Translate(const Vector4<T>& translation)
{
    Matrix4<T> result;
    result.rows[EAxis_X] = Vector4<T>::UnitX();
    result.rows[EAxis_Y] = Vector4<T>::UnitY();
    result.rows[EAxis_Z] = Vector4<T>::UnitZ();
    result.rows[EAxis_W] = translation;
    return result;
}

template<typename T>
FORCEINLINE Matrix4<T> Matrix4<T>::Translate(const Vector3<T>& translation)
{
    Matrix4<T> result;
    result.rows[EAxis_X] = Vector4<T>::UnitX();
    result.rows[EAxis_Y] = Vector4<T>::UnitY();
    result.rows[EAxis_Z] = Vector4<T>::UnitZ();
    result.rows[EAxis_W] = Vector4(translation, T(1));
    return result;
}

template<typename T>
FORCEINLINE Matrix4<T> Matrix4<T>::Scale(float scaleFactor)
{
    Matrix4<T> result;
    result.rows[EAxis_X] = Vector4(T(scaleFactor), T(0), T(0), T(0));
    result.rows[EAxis_Y] = Vector4(T(0), T(scaleFactor), T(0), T(0));
    result.rows[EAxis_Z] = Vector4(T(0), T(0), T(scaleFactor), T(0));
    result.rows[EAxis_W] = Vector4(T(0), T(0), T(0), T(1));
    return result;
}

template<typename T>
FORCEINLINE Matrix4<T> Matrix4<T>::Scale(const Vector3<T>& scale)
{
    Matrix4<T> result;
    result.rows[EAxis_X] = Vector4(scale.x, T(0), T(0), T(0));
    result.rows[EAxis_Y] = Vector4(T(0), scale.y, T(0), T(0));
    result.rows[EAxis_Z] = Vector4(T(0), T(0), scale.z, T(0));
    result.rows[EAxis_W] = Vector4(T(0), T(0), T(0), T(1));
    return result;
}

template<typename T>
FORCEINLINE Matrix4<T> Matrix4<T>::RotateX(float angleX)
{
    Matrix4 result;
    VectorMatrixRotateX(&result, angleX);
    return result;
}

template<typename T>
FORCEINLINE Matrix4<T> Matrix4<T>::RotateY(float angleY)
{
    Matrix4 result;
    VectorMatrixRotateY(&result, angleY);
    return result;
}

template<typename T>
FORCEINLINE Matrix4<T> Matrix4<T>::RotateZ(float angleZ)
{
    Matrix4 result;
    VectorMatrixRotateZ(&result, angleZ);
    return result;
}

template<typename T>
FORCEINLINE Matrix4<T> Matrix4<T>::RotateRollPitchYaw(float roll, float pitch, float yaw)
{
    Matrix4 result;
    VectorMatrixRotateRollPitchYaw(&result, roll, pitch, yaw);
    return result;
}

template<typename T>
FORCEINLINE Matrix4<T> Matrix4<T>::PerspectiveRH(float fov, float aspectRatio, float nearZ, float farZ)
{

}

template<typename T>
FORCEINLINE Matrix4<T> Matrix4<T>::PerspectiveLH(float fov, float aspectRatio, float nearZ, float farZ)
{

}

template<typename T>
FORCEINLINE Matrix4<T> Matrix4<T>::OrthographicRH(float viewWidth, float viewHeight, float nearZ, float farZ)
{

}

template<typename T>
FORCEINLINE Matrix4<T> Matrix4<T>::OrthographicLH(float viewWidth, float viewHeight, float nearZ, float farZ)
{

}

template<typename T>
FORCEINLINE Matrix4<T> Matrix4<T>::operator*(const Matrix4<T>& rhs) const
{
    Matrix4<T> result;
    VectorMatrixMultiply(&result, this, &rhs);
    return result;
}

template<typename T>
FORCEINLINE Vector4<T> operator*(const Vector4<T>& vector, const Matrix4<T>& matrix)
{
    Vector4<T> result;
    VectorMatrixTransformPoint(&result, &vector, &matrix);
    return result;
}

template<typename T>
FORCEINLINE Vector3<T> operator*(const Vector3<T>& vector, const Matrix4<T>& matrix)
{
    Vector4<T> result;
    Vector4<T> vec = Vector4<T>(vector, 1.0F);
    VectorMatrixTransformPoint(&result, &vec, &matrix);
    return Vector3<T>(result);
}

} // namespace Basalt::Math
} // namespace Basalt
