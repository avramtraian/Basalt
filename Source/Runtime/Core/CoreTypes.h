// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "CoreDefines.h"

#if BASALT_PLATFORM_WINDOWS
    #include "Platform/Windows/WindowsPlatformTypes.h"
    using PlatformTypes = Basalt::WindowsPlatformTypes;
#endif // BASALT_PLATFORM_WINDOWS

namespace Basalt
{

/** An 8-bit unsigned integer. */
using U8 = PlatformTypes::U8;

/** A 16-bit unsigned integer. */
using U16 = PlatformTypes::U16;

/** A 32-bit unsigned integer. */
using U32 = PlatformTypes::U32;

/** A 64-bit unsigned integer. */
using U64 = PlatformTypes::U64;

/** An 8-bit signed integer. */
using I8 = PlatformTypes::I8;

/** A 16-bit signed integer. */
using I16 = PlatformTypes::I16;

/** A 32-bit signed integer. */
using I32 = PlatformTypes::I32;

/** A 64-bit signed integer. */
using I64 = PlatformTypes::I64;

/** A 32-bit floating point number. */
using F32 = PlatformTypes::F32;

/** A 64-bit floating point number. */
using F64 = PlatformTypes::F64;

/** An 8-bit boolean value. */
using B8 = PlatformTypes::B8;

/** A 32-bit boolean value. */
using B32 = PlatformTypes::B32;

/** An unsigned integer the same size as a pointer. */
using Usize = PlatformTypes::Usize;

/** A signed integer the same size as a pointer. */
using Ssize = PlatformTypes::Ssize;

/** Value that represents an invalid size. Usually interpreted as an error. */
constexpr Usize InvalidSize = static_cast<Usize>(-1);

static_assert(sizeof(U8) == 1);
static_assert(sizeof(U16) == 2);
static_assert(sizeof(U32) == 4);
static_assert(sizeof(U64) == 8);

static_assert(sizeof(I8) == 1);
static_assert(sizeof(I16) == 2);
static_assert(sizeof(I32) == 4);
static_assert(sizeof(I64) == 8);

static_assert(sizeof(F32) == 4);
static_assert(sizeof(F64) == 8);

static_assert(sizeof(B8) == 1);
static_assert(sizeof(B32) == 4);

static_assert(sizeof(Usize) == sizeof(void*));
static_assert(sizeof(Ssize) == sizeof(void*));

template<typename T>
struct RemoveReference
{
    using Type = T;
};

template<typename T>
struct RemoveReference<T&>
{
    using Type = T;
};

template<typename T>
struct RemoveReference<T&&>
{
    using Type = T;
};

template<typename T>
using RemoveReferenceType = typename RemoveReference<T>::Type;

template<typename T>
constexpr RemoveReferenceType<T>&& Move(T&& object) noexcept
{
    return static_cast<RemoveReferenceType<T>&&>(object);
}

template< class T >
constexpr T&& Forward(RemoveReferenceType<T>& object) noexcept
{
    return static_cast<T&&>(object);
}

template< class T >
constexpr T&& Forward(RemoveReferenceType<T>&& object) noexcept
{
    return static_cast<T&&>(object);
}

#define BT_NON_COPYABLE(TYPENAME)   TYPENAME(const TYPENAME&) = delete
#define BT_NON_MOVABLE(TYPENAME)    TYPENAME(TYPENAME&&) noexcept = delete

} // namespace Basalt
