// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

///
/// Platform macros.
///

#ifndef BASALT_PLATFORM_WINDOWS
    #define BASALT_PLATFORM_WINDOWS     0
#endif // BASALT_PLATFORM_WINDOWS

#ifndef BASALT_PLATFORM_LINUX
    #define BASALT_PLATFORM_LINUX       0
#endif // BASALT_PLATFORM_LINUX

#ifndef BASALT_PLATFORM_MACOS
    #define BASALT_PLATFORM_MACOS       0
#endif // BASALT_PLATFORM_MACOS

#ifndef BASALT_PLATFORM_XBOX
    #define BASALT_PLATFORM_XBOX        0
#endif // BASALT_PLATFORM_XBOX

#ifndef BASALT_PLATFORM_PLAYSTATION
    #define BASALT_PLATFORM_PLAYSTATION 0
#endif // BASALT_PLATFORM_PLAYSTATION

#ifndef BASALT_PLATFORM_IOS
    #define BASALT_PLATFORM_IOS         0
#endif // BASALT_PLATFORM_IOS

#ifndef BASALT_PLATFORM_ANDROID
    #define BASALT_PLATFORM_ANDROID     0
#endif // BASALT_PLATFORM_ANDROID

#if BASALT_PLATFORM_WINDOWS || BASALT_PLATFORM_LINUX || BASALT_PLATFORM_MACOS
    #define BASALT_PLATFORM_DESKTOP     1
#else
    #define BASALT_PLATFORM_DESKTOP     0
#endif // Desktop platforms.

#if BASALT_PLATFORM_XBOX || BASALT_PLATFORM_PLAYSTATION
    #define BASALT_PLATFORM_CONSOLE     1
#else
    #define BASALT_PLATFORM_CONSOLE     0
#endif // Console platforms.

#if BASALT_PLATFORM_IOS || BASALT_PLATFORM_ANDROID
    #define BASALT_PLATFORM_MOBILE      1
#else
    #define BASALT_PLATFORM_MOBILE      0
#endif // Mobile platforms.

#if !BASALT_PLATFORM_DESKTOP && !BASALT_PLATFORM_CONSOLE && !BASALT_PLATFORM_MOBILE
    #error No platform was selected!
#endif // Any platform.

///
/// API import/export specifiers.
///

#if BASALT_PLATFORM_WINDOWS
    #define BASALT_API_SPECIFIER_EXPORT __declspec(dllexport)
    #define BASALT_API_SPECIFIER_IMPORT __declspec(dllimport)
#endif // BASALT_PLATFORM_WINDOWS

#ifndef BASALT_API_SPECIFIER_EXPORT
    #define BASALT_API_SPECIFIER_EXPORT
#endif // BASALT_API_SPECIFIER_EXPORT

#ifndef BASALT_API_SPECIFIER_IMPORT
    #define BASALT_API_SPECIFIER_IMPORT
#endif // BASALT_API_SPECIFIER_IMPORT

#ifdef BASALT_EXPORT_API
    #define BASALT_API                  BASALT_API_SPECIFIER_EXPORT
#else
    #define BASALT_API                  BASALT_API_SPECIFIER_IMPORT
#endif // BASALT_EXPORT_API

/**
 * Specifier that expands to nothing, but increases the code readability.
 * This specifier should be used for struct/classes that are templated
 * or PODs, in order to show that they are part of the public API.
 */
#define BASALT_S_API

///
/// Build configuration macros.
///

#ifndef BASALT_BUILD_DEBUG
    #define BASALT_BUILD_DEBUG          0
#endif // BASALT_BUILD_DEBUG

#ifndef BASALT_BUILD_RELEASE
    #define BASALT_BUILD_RELEASE        0
#endif // BASALT_BUILD_RELEASE

#ifndef BASALT_BUILD_SHIPPING
    #define BASALT_BUILD_SHIPPING       0
#endif // BASALT_BUILD_SHIPPING

/**
 * Build engine target macros.
 */

#ifndef BASALT_BUILD_GAME
    #define BASALT_BUILD_GAME           0
#endif // BASALT_BUILD_GAME

#ifndef BASALT_BUILD_EDITOR
    #define BASALT_BUILD_EDITOR         0
#endif // BASALT_BUILD_EDITOR

#if !BASALT_BUILD_GAME && !BASALT_BUILD_EDITOR
    #error No engine target was specified!
#endif // Any engine target.

///
/// Compiler-detection macros.
///

#ifdef _MSC_BUILD
    #define BASALT_COMPILER_MSVC        1
#else
    #define BASALT_COMPILER_MSVC        0
#endif // _MSC_BUILD

#ifdef __clang__
    #define BASALT_COMPILER_CLANG       1
#else
    #define BASALT_COMPILER_CLANG       0
#endif // __clang__

#ifdef __gcc__
    #define BASALT_COMPILER_GCC         1
#else
    #define BASALT_COMPILER_GCC         0
#endif // __clang__

#if !BASALT_COMPILER_MSVC && !BASALT_COMPILER_CLANG && !BASALT_COMPILER_GCC
    #error Unknown/Unsupported compiler!
#endif // All compilers.

///
/// Compiler-specific attributes & utilities.
///

#if BASALT_COMPILER_MSVC
    #define FORCEINLINE                 __forceinline
    #define BASALT_DEBUGBREAK           __debugbreak()
#endif // BASALT_COMPILER_MSVC

#if BASALT_COMPILER_MSVC
    // class 'A' needs to have dll-interface to be used by clients of class 'B'
    #pragma warning(disable : 4251)
#endif // BASALT_COMPILER_MSVC

///
/// Compiler-agnostic attributes & utilities.
///

#define NODISCARD                       [[nodiscard]]
#define MAYBE_UNUSED                    [[maybe_unused]]
#define LIKELY                          [[likely]]
#define UNLIKELY                        [[unlikely]]

/** Bit manipulation utilities. */
#define BIT(N)                          (1 << (N))
#define GET_BIT(X, N)                   ((X) & BIT(N))
#define ENABLE_BIT(X, N)                ((X) |= BIT(N))
#define DISABLE_BIT(X, N)               ((X) &= (~BIT(N)))

/** Multiples of the unit byte. Allows for easier conversions. */
#define KiB                             (1024)
#define MiB                             (1024 * 1024)
#define GiB                             (1024 * 1024 * 1024)
