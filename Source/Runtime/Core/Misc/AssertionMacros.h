// Copyright(c) 2023 Basalt Technology Group.All rights reserved.

#pragma once

#include "Core/CoreDefines.h"
#include "Core/CoreTypes.h"

/**
 * Enables checks even in shipping build. Useful when you have Shipping-only
 * bugs that are hard to track down. Default value should be 0 (false).
 */
#define USE_CHECKS_IN_SHIPPING          0

/**
 * Enables debug checks in release build. Default value should be 0 (false).
 */
#define USE_DEBUG_CHECKS_IN_RELEASE     0

/**
 * Enables checks even in shipping build. Useful when you have Shipping-only
 * bugs that are hard to track down. Default value should be 0 (false).
 */
#define USE_DEBUG_CHECKS_IN_SHIPPING    0

#if BASALT_BUILD_DEBUG || (BASALT_BUILD_RELEASE && USE_DEBUG_CHECKS_IN_RELEASE) || (BASALT_BUILD_SHIPPING && USE_DEBUG_CHECKS_IN_SHIPPING)
    /**
     * Issues an assertion if the given expression is evaluated to false.
     * This macro is included only in debug builds. In other configurations, the
     * macro simply expands to nothing. It is important to remember that if the
     * debug checks are disabled, the given expression will not be called.
     */
    #define CheckDebug(EXPRESSION)                      \
        if (!(EXPRESSION))                              \
        {                                               \
            BASALT_DEBUGBREAK;                          \
        }
    
     /**
      * Issues an assertion if the given expression is evaluated to false.
      * This macro is included only in debug builds. In other configurations, the
      * macro simply expands to nothing. It is important to remember that if the
      * debug checks are disabled, the given expression will not be called.
      * 
      * If the expression fails, a formatted message will be logged.
      */
    #define CheckDebugf(EXPRESSION, ...)                \
        if (!(EXPRESSION))                              \
        {                                               \
            BASALT_DEBUGBREAK;                          \
        }
#else
    #define CheckDebug(...) // Excluded from build.
    #define CheckDebugf(...) // Excluded from build.
#endif // Debug checks enabled.

#if BASALT_BUILD_DEBUG || BASALT_BUILD_RELEASE || (BASALT_BUILD_SHIPPING && USE_CHECKS_IN_SHIPPING)
    /**
     * Issues an assertion if the given expression is evaluated to false.
     * This macro is included only in debug and release builds. In shipping
     * configurations, the macro simply expands to nothing. It is important
     * to remember that if the debug checks are disabled, the given expression
     * will not be called.
     */
    #define Check(EXPRESSION)                           \
        if (!(EXPRESSION))                              \
        {                                               \
            BASALT_DEBUGBREAK;                          \
        }
    
     /**
      * Issues an assertion if the given expression is evaluated to false.
      * This macro is included only in debug and release builds. In shipping
      * configurations, the macro simply expands to nothing. It is important
      * to remember that if the debug checks are disabled, the given expression
      * will not be called.
      *
      * If the expression fails, a formatted message will be logged.
      */
    #define Checkf(EXPRESSION, ...)                     \
        if (!(EXPRESSION))                              \
        {                                               \
            BASALT_DEBUGBREAK;                          \
        }
#else
    #define Check(...) // Excluded from build.
    #define Checkf(...) // Excluded from build.
#endif // Checks enabled.
