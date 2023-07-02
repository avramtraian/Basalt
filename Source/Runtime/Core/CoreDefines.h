// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

///
/// Platform macros.
///

#ifndef BT_PLATFORM_WINDOWS
    #define BT_PLATFORM_WINDOWS     0
#endif // BT_PLATFORM_WINDOWS

#ifndef BT_PLATFORM_LINUX
    #define BT_PLATFORM_LINUX       0
#endif // BT_PLATFORM_LINUX

#ifndef BT_PLATFORM_MACOS
    #define BT_PLATFORM_MACOS       0
#endif // BT_PLATFORM_MACOS

#ifndef BT_PLATFORM_XBOX
    #define BT_PLATFORM_XBOX        0
#endif // BT_PLATFORM_XBOX

#ifndef BT_PLATFORM_PLAYSTATION
    #define BT_PLATFORM_PLAYSTATION 0
#endif // BT_PLATFORM_PLAYSTATION

#ifndef BT_PLATFORM_IOS
    #define BT_PLATFORM_IOS         0
#endif // BT_PLATFORM_IOS

#ifndef BT_PLATFORM_ANDROID
    #define BT_PLATFORM_ANDROID     0
#endif // BT_PLATFORM_ANDROID

#if BT_PLATFORM_WINDOWS || BT_PLATFORM_LINUX || BT_PLATFORM_MACOS
    #define BT_PLATFORM_DESKTOP     1
#else
    #define BT_PLATFORM_DESKTOP     0
#endif // Desktop platforms.

#if BT_PLATFORM_XBOX || BT_PLATFORM_PLAYSTATION
    #define BT_PLATFORM_CONSOLE     1
#else
    #define BT_PLATFORM_CONSOLE     0
#endif // Console platforms.

#if BT_PLATFORM_IOS || BT_PLATFORM_ANDROID
    #define BT_PLATFORM_MOBILE      1
#else
    #define BT_PLATFORM_MOBILE      0
#endif // Mobile platforms.

#if !BT_PLATFORM_DESKTOP && !BT_PLATFORM_CONSOLE && !BT_PLATFORM_MOBILE
    #error No platform was selected!
#endif // Any platform.

///
/// API import/export specifiers.
///

#if BT_PLATFORM_WINDOWS
    #define BT_API_SPECIFIER_EXPORT __declspec(dllexport)
    #define BT_API_SPECIFIER_IMPORT __declspec(dllimport)
#endif // BT_PLATFORM_WINDOWS

#ifndef BT_API_SPECIFIER_EXPORT
    #define BT_API_SPECIFIER_EXPORT
#endif // BT_API_SPECIFIER_EXPORT

#ifndef BT_API_SPECIFIER_IMPORT
    #define BT_API_SPECIFIER_IMPORT
#endif // BT_API_SPECIFIER_IMPORT

#ifdef BT_EXPORT_API
    #define BASALT_API              BT_API_SPECIFIER_EXPORT
#else
    #define BASALT_API              BT_API_SPECIFIER_IMPORT
#endif // BT_EXPORT_API

/**
 * Specifier that expands to nothing, but increases the code readability.
 * This specifier should be used for struct/classes that are templated
 * or PODs, in order to show that they are part of the public API.
 */
#define BASALT_S_API

///
/// Build configuration macros.
///

#ifndef BT_BUILD_DEBUG
    #define BT_BUILD_DEBUG          0
#endif // BT_BUILD_DEBUG

#ifndef BT_BUILD_RELEASE
    #define BT_BUILD_RELEASE        0
#endif // BT_BUILD_RELEASE

#ifndef BT_BUILD_SHIPPING
    #define BT_BUILD_SHIPPING       0
#endif // BT_BUILD_SHIPPING

/**
 * Build engine target macros.
 */

#ifndef BT_BUILD_GAME
    #define BT_BUILD_GAME           0
#endif // BT_BUILD_GAME

#ifndef BT_BUILD_EDITOR
    #define BT_BUILD_EDITOR         0
#endif // BT_BUILD_EDITOR

#if !BT_BUILD_GAME && !BT_BUILD_EDITOR
    #error No engine target was specified!
#endif // Any engine target.

///
/// Compiler-detection macros.
///

#ifdef _MSC_BUILD
    #define BT_COMPILER_MSVC        1
#else
    #define BT_COMPILER_MSVC        0
#endif // _MSC_BUILD

#ifdef __clang__
    #define BT_COMPILER_CLANG       1
#else
    #define BT_COMPILER_CLANG       0
#endif // __clang__

#ifdef __gcc__
    #define BT_COMPILER_GCC         1
#else
    #define BT_COMPILER_GCC         0
#endif // __clang__

#if !BT_COMPILER_MSVC && !BT_COMPILER_CLANG && !BT_COMPILER_GCC
    #error Unknown/Unsupported compiler!
#endif // All compilers.

///
/// Compiler-specific attributes & utilities.
///

#if BT_COMPILER_MSVC
    #define FORCEINLINE             __forceinline
    #define BT_DEBUGBREAK           __debugbreak()
    #define BT_FUNCTION             __FUNCSIG__
#endif // BT_COMPILER_MSVC

#if BT_COMPILER_MSVC
    // class 'A' needs to have dll-interface to be used by clients of class 'B'
    #pragma warning(disable : 4251)
#endif // BT_COMPILER_MSVC

///
/// Compiler-agnostic attributes & utilities.
///

#define NODISCARD                   [[nodiscard]]
#define MAYBE_UNUSED                [[maybe_unused]]
#define LIKELY                      [[likely]]
#define UNLIKELY                    [[unlikely]]

#define LOW_32BIT                   ((U64)0xFFFFFFFF)
#define HIGH_32BIT                  ((U64)(LOW_32BIT << 32))

/** Bit manipulation utilities. */
#define BIT(N)                      (1 << (N))
#define GET_BIT(X, N)               ((X) & BIT(N))
#define ENABLE_BIT(X, N)            ((X) |= BIT(N))
#define DISABLE_BIT(X, N)           ((X) &= (~BIT(N)))

#define BT_ARRAY_COUNT(A)           (sizeof(A) / sizeof(A[0]))

/** Multiples of the unit byte. Allows for easier conversions. */
#define KiB                         (1024)
#define MiB                         (1024 * 1024)
#define GiB                         (1024 * 1024 * 1024)
