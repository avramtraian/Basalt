// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Logger.h"

/**
 * 
 */
#define WITHOUT_LOGGING         0

#if BT_BUILD_DEBUG
    #define ENABLE_LOGS_DEBUG   1
    #define ENABLE_LOGS_TRACE   1
    #define ENABLE_LOGS_INFO    1
    #define ENABLE_LOGS_WARN    1
    #define ENABLE_LOGS_ERROR   1
    #define ENABLE_LOGS_FATAL   1
#endif // BT_BUILD_DEBUG

#if BT_BUILD_RELEASE
    #define ENABLE_LOGS_DEBUG   0
    #define ENABLE_LOGS_TRACE   0
    #define ENABLE_LOGS_INFO    1
    #define ENABLE_LOGS_WARN    1
    #define ENABLE_LOGS_ERROR   1
    #define ENABLE_LOGS_FATAL   1
#endif // BT_BUILD_RELEASE

#if BT_BUILD_SHIPPING
    #define ENABLE_LOGS_DEBUG   0
    #define ENABLE_LOGS_TRACE   0
    #define ENABLE_LOGS_INFO    0
    #define ENABLE_LOGS_WARN    0
    #define ENABLE_LOGS_ERROR   0
    #define ENABLE_LOGS_FATAL   1
#endif // BT_BUILD_SHIPPING

#if !WITHOUT_LOGGING && ENABLE_LOGS_DEBUG
    #define BT_LOG_DEBUG(CATEGORY, FORMAT, ...)     \
        ::Basalt::g_logger->Submit(::Basalt::ELogCategory::CATEGORY, ::Basalt::ELogSeverity::Debug, FORMAT, __VA_ARGS__)
#else
    #define BT_LOG_DEBUG(...) // Excluded from build.
#endif // Debug logs enabled.

#if !WITHOUT_LOGGING && ENABLE_LOGS_TRACE
#define BT_LOG_TRACE(CATEGORY, FORMAT, ...)     \
        ::Basalt::g_logger->Submit(::Basalt::ELogCategory::CATEGORY, ::Basalt::ELogSeverity::Trace, FORMAT, __VA_ARGS__)
#else
    #define BT_LOG_TRACE(...) // Excluded from build.
#endif // Trace logs enabled.

#if !WITHOUT_LOGGING && ENABLE_LOGS_INFO
#define BT_LOG_INFO(CATEGORY, FORMAT, ...)      \
        ::Basalt::g_logger->Submit(::Basalt::ELogCategory::CATEGORY, ::Basalt::ELogSeverity::Info, FORMAT, __VA_ARGS__)
#else
    #define BT_LOG_INFO(...) // Excluded from build.
#endif // Info logs enabled.

#if !WITHOUT_LOGGING && ENABLE_LOGS_WARN
#define BT_LOG_WARN(CATEGORY, FORMAT, ...)      \
        ::Basalt::g_logger->Submit(::Basalt::ELogCategory::CATEGORY, ::Basalt::ELogSeverity::Warn, FORMAT, __VA_ARGS__)
#else
    #define BT_LOG_WARN(...) // Excluded from build.
#endif // Warn logs enabled.

#if !WITHOUT_LOGGING && ENABLE_LOGS_ERROR
#define BT_LOG_ERROR(CATEGORY, FORMAT, ...)     \
        ::Basalt::g_logger->Submit(::Basalt::ELogCategory::CATEGORY, ::Basalt::ELogSeverity::Error, FORMAT, __VA_ARGS__)
#else
    #define BT_LOG_ERROR(...) // Excluded from build.
#endif // Error logs enabled.

#if !WITHOUT_LOGGING && ENABLE_LOGS_FATAL
#define BT_LOG_FATAL(CATEGORY, FORMAT, ...)     \
        ::Basalt::g_logger->Submit(::Basalt::ELogCategory::CATEGORY, ::Basalt::ELogSeverity::Fatal, FORMAT, __VA_ARGS__)
#else
    #define BT_LOG_FATAL(...) // Excluded from build.
#endif // Fatal logs enabled.
