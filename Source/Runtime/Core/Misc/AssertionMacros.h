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

#if BT_BUILD_DEBUG || (BT_BUILD_RELEASE && USE_DEBUG_CHECKS_IN_RELEASE) || (BT_BUILD_SHIPPING && USE_DEBUG_CHECKS_IN_SHIPPING)
    /**
     * Issues an assertion if the given expression is evaluated to false.
     * This macro is included only in debug builds. In other configurations, the
     * macro simply expands to nothing. It is important to remember that if the
     * debug checks are disabled, the given expression will not be called.
     */
    #define CheckDebug(EXPRESSION)                                          \
        if (!(EXPRESSION))                                                  \
        {                                                                   \
            ::Basalt::Debug::ReportAssertFailed(                            \
                #EXPRESSION, __FILE__, BT_FUNCTION, __LINE__);              \
            BT_DEBUGBREAK;                                              \
        }
    
     /**
      * Issues an assertion if the given expression is evaluated to false.
      * This macro is included only in debug builds. In other configurations, the
      * macro simply expands to nothing. It is important to remember that if the
      * debug checks are disabled, the given expression will not be called.
      * 
      * If the expression fails, a formatted message will be logged.
      */
    #define CheckDebugf(EXPRESSION, ...)                                    \
        if (!(EXPRESSION))                                                  \
        {                                                                   \
            ::Basalt::Debug::ReportAssertFailed(                            \
                #EXPRESSION, __FILE__, BT_FUNCTION, __LINE__, __VA_ARGS__); \
            BT_DEBUGBREAK;                                              \
        }

    #define CheckCodeDebug(...) __VA_ARGS__

#else
    #define CheckDebug(...) // Excluded from build.
    #define CheckDebugf(...) // Excluded from build.
    #define CheckCodeDebugf(...) // Excluded from build.
#endif // Debug checks enabled.

#if BT_BUILD_DEBUG || BT_BUILD_RELEASE || (BT_BUILD_SHIPPING && USE_CHECKS_IN_SHIPPING)
    /**
     * Issues an assertion if the given expression is evaluated to false.
     * This macro is included only in debug and release builds. In shipping
     * configurations, the macro simply expands to nothing. It is important
     * to remember that if the debug checks are disabled, the given expression
     * will not be called.
     */
    #define Check(EXPRESSION)                                               \
        if (!(EXPRESSION))                                                  \
        {                                                                   \
            ::Basalt::Debug::ReportAssertFailed(                            \
                #EXPRESSION, __FILE__, BT_FUNCTION, __LINE__);              \
            BT_DEBUGBREAK;                                              \
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
    #define Checkf(EXPRESSION, ...)                                         \
        if (!(EXPRESSION))                                                  \
        {                                                                   \
            ::Basalt::Debug::ReportAssertFailed(                            \
                #EXPRESSION, __FILE__, BT_FUNCTION, __LINE__, __VA_ARGS__); \
            BT_DEBUGBREAK;                                              \
        }

    #define CheckCode(...) __VA_ARGS__

#else
    #define Check(...) // Excluded from build.
    #define Checkf(...) // Excluded from build.
    #define CheckCode(...) // Excluded from build.
#endif // Checks enabled.

namespace Basalt
{
    
class BASALT_API Debug
{
public:
    static void ReportAssertFailed(
        const char* expression,
        const char* filepath,
        const char* function_name,
        U32 line_number,
        const char* message = nullptr, ...
    );
};

} // namespace Basalt
