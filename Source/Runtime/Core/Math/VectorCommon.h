// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

namespace Basalt
{
namespace Geometry
{

template<typename T>
FORCEINLINE Vector2<T>::Vector2(const Vector3<T>& vector3)
    : x(vector3.x)
    , y(vector3.y)
{}

template<typename T>
FORCEINLINE Vector2<T>::Vector2(const Vector4<T>& vector4)
    : x(vector4.x)
    , y(vector4.y)
{}

template<typename T>
FORCEINLINE Vector3<T>::Vector3(const Vector2<T>& vector2)
    : x(vector2.x)
    , y(vector2.y)
    , z(T(0))
{}

template<typename T>
FORCEINLINE Vector3<T>::Vector3(const Vector2<T>& vector2, T inZ)
    : x(vector2.x)
    , y(vector2.y)
    , z(inZ)
{}

template<typename T>
FORCEINLINE Vector3<T>::Vector3(const Vector4<T>& vector4)
    : x(vector4.x)
    , y(vector4.y)
    , z(vector4.z)
{}

template<typename T>
FORCEINLINE Vector4<T>::Vector4(const Vector2<T>& vector2)
    : x(vector2.x)
    , y(vector2.y)
    , z(T(0))
    , w(T(0))
{}

template<typename T>
FORCEINLINE Vector4<T>::Vector4(const Vector2<T>& vector2, T inZ, T inW)
    : x(vector2.x)
    , y(vector2.y)
    , z(inZ)
    , w(inW)
{}

template<typename T>
FORCEINLINE Vector4<T>::Vector4(const Vector3<T>& vector3)
    : x(vector3.x)
    , y(vector3.y)
    , z(vector3.z)
    , w(T(0))
{}

template<typename T>
FORCEINLINE Vector4<T>::Vector4(const Vector3<T>& vector3, T inW)
    : x(vector3.x)
    , y(vector3.y)
    , z(vector3.z)
    , w(inW)
{}

} // namespace Basalt::Geometry
} // namespace Basalt
