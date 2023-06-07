// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/CoreDefines.h"
#include "Core/CoreTypes.h"
#include "Core/Misc/CommandLine.h"
#include "Core/Containers/Array.h"
#include "Core/Containers/Span.h"
#include "Core/Memory/Buffer.h"
#include "Core/Misc/IterationDecision.h"

/**
 * The logger decides what log categories and severities are presented to the developer. These
 * configurations are done by manually specifying them, or by reading them from the command line.
 *
 * This is a list of all logging command line flags and switches:
 *  - log-{core, engine, asset-system, ...} -> Specifies what log categories are enabled. The
 *      arguments have the format 'log-{category name, words separated by '-'}'.
 *
 *  - log-verbosity={debug, trace, info, warn, error, fatal} -> Specifies the logging verbosity level.
 *      All log severities higher or equal to the specified value are also included.
 *
 * The build configuration is the ultimate decider of the enabled log severities, as logging macros
 * are expanding depending on the configuration.
 */

namespace Basalt
{

/**
 * Enumeration of all log severities, in ascending order.
 */
enum class ELogSeverity : U8
{
    Debug, Trace, Info, Warn, Error, Fatal,
    MaxEnumValue,
};

/**
 * Enumeration of all log categories.
 */
enum class ELogCategory : U32
{
    // Engine categories.
    Core, Engine, Editor, World, AssetSystem, Renderer, RendererRHI, Audio, Network,

    // Game categories.
    Game, Game01, Game02, Game03, Game04,

    MaxEnumValue,
};

/**
 * Structure that 
 */
class BASALT_API LogFilter
{
public:
    /** @return A filter that contains all log severities and categories. */
    FORCEINLINE static LogFilter All()
    {
        LogFilter result;
        
        for (U8 severity = 0; severity < (U8)ELogSeverity::MaxEnumValue; ++severity)
        {
            result.AddSeverity((ELogSeverity)severity);
        }
        for (U32 category = 0; category < (U8)ELogCategory::MaxEnumValue; ++category)
        {
            result.AddCategory((ELogCategory)category);
        }

        return result;
    }

    /**
     * Creates a log filter that includes both the provided filters.
     * Represents the union of the filters.
     * 
     * @param a The first filter.
     * @param b The second filter.
     * 
     * @return The union of the filters.
     */
    FORCEINLINE static LogFilter Union(LogFilter a, LogFilter b)
    {
        LogFilter result;
        result.m_severity_mask = a.m_severity_mask | b.m_severity_mask;
        result.m_category_mask = a.m_category_mask | b.m_category_mask;
        return result;
    }

    /**
     * Creates a log filter composed of severities and categories that
     * are included in both of the filters.
     * Represents the intersection of the filters.
     * 
     * @parm a The first filter.
     * @parm b The second filter.
     * 
     * @return The intersection of the filters.
     */
    FORCEINLINE static LogFilter Common(LogFilter a, LogFilter b)
    {
        LogFilter result;
        result.m_severity_mask = a.m_severity_mask & b.m_severity_mask;
        result.m_category_mask = a.m_category_mask & b.m_category_mask;
        return result;
    }

public:
    /** Default constructor. The created filter includes no severities or categories. */
    FORCEINLINE LogFilter()
        : m_severity_mask(0)
        , m_category_mask(0)
    {}

public:
    FORCEINLINE void AddSeverity(ELogSeverity severity) { m_severity_mask |= BIT((U8)severity); }
    FORCEINLINE void AddCategory(ELogCategory category) { m_category_mask |= BIT((U32)category); }

    FORCEINLINE void RemoveSeverity(ELogSeverity severity) { m_severity_mask &= (~BIT((U8)severity)); }
    FORCEINLINE void RemoveCategory(ELogCategory category) { m_category_mask &= (~BIT((U32)category)); }

    FORCEINLINE void RemoveAllSeverities() { m_severity_mask = 0; }
    FORCEINLINE void RemoveAllCategories() { m_category_mask = 0; }

public:
    FORCEINLINE bool HasSeverity(ELogSeverity severity) const { return m_severity_mask & BIT((U8)severity); }
    FORCEINLINE bool HasCategory(ELogCategory category) const { return m_category_mask & BIT((U32)category); }

private:
    /** Bit mask of the allowed log severities. */
    U8 m_severity_mask;

    /** Bit mask of the allowed log categories. */
    U32 m_category_mask;
};

/**
 * Structure that describes the logger and the logging system.
 */
struct LoggerDescription
{
    /** The size of a log page. It also represents (almost) the maximum length of a log message. */
    Usize page_size = 6 * KiB;

    /** The number of log pages to create. */
    Usize page_count = 8;

    /** Whether or not to replicate the logs to a file on disk. */
    bool replicate_logs_to_file = true;
};

/**
 * The logger class is responsible for management of the core logging system. It provides all
 * the engine-level API for generating logs during runtime.
 */
class Logger
{
public:
    /**
     * Structure that defines a log.
     */
    struct BASALT_S_API LogDefinition
    {
        ELogCategory category;
        ELogSeverity severity;
        StringView message;
    };

    /**
     * The function pointer invoked for every log entry, while iterating over them.
     */
    using LogPredicate = IterationDecision(*)(const LogDefinition&);

public:
    /**
     * Creates and initializes the logger instance. All logs must be generated after this
     * function is called. Otherwise, the logs will not be registered at all.
     * 
     * @param description The description of the logger.
     * @return Whether or not the initialization was successful.
     */
    static bool Initialize(const LoggerDescription& description);
    
    /**
     * Destroys the logger instance. No logs can be generated after this function is called,
     * as they will not be registered at all.
     */
    static void Shutdown();

public:
    /**
     * Submits a message to the logger.
     * The message will actually be logged only if it is included in the logger global filter.
     * If the console logging is enabled, the logs will have different colors, based on their
     * severities.
     * 
     * @param category The category of the log.
     * @param severity The severity of the log.
     * @param format The unformatted log message.
     * @param ... Formatting arguments.
     */
    BASALT_API void Submit(ELogCategory category, ELogSeverity severity, const char* format, ...);

    /**
     * Iterates over all the logs currently stored by the logger.
     * You can filter the logs by providing this function with a `LogFilter` structure.
     * 
     * @param filter The log filter. Only logs that are included in this function will be iterated.
     * @param log_predicate The function called on every log entry. If this returns false, the
     *                     iteration will be stopped.
     */
    BASALT_API void IterateLogs(LogFilter filter, LogPredicate log_predicate) const;

private:
    /** Header of a log entry stored in the logs buffer. */
    struct LogHeader
    {
        ELogCategory category;
        ELogSeverity severity;
        Usize message_bytes_count;
    };

    struct LogPage
    {
        /** The offset in bytes where the pages is stored in the logs buffer. */
        Usize buffer_offset;
        
        /** The offset in bytes, relative to the beginning of the page, where the next log entry will be inserted. */
        Usize insert_offset;

        /** The number of logs currently stored in the page. */
        Usize logs_count;
    };

private:
    /** Default constructor. It is only called internally during the `Initialize` function. */
    Logger() = default;

    // It must not be possible to create any `Logger` class instances.
    BT_NON_COPYABLE(Logger);
    BT_NON_MOVABLE(Logger);

private:
    /**
     * Parses the command line in order to determine what categories and severities should be included in the filter.
     */
    void ParseCommandLine();

    void CreatePages(const LoggerDescription& description);

    StringView WriteToPage(ELogCategory category, ELogSeverity severity, const char* message, va_list arg_list);

    U8* FormatLogMessage(Span<U8> buffer, Usize* out_written_bytes, const char* format, va_list arg_list);

private:
    /** The log filter that the logger uses. Only logs included in this filter are presented to the developer. */
    LogFilter m_filter;

    /** The buffer where the log pages are stored. */
    Buffer m_pages_buffer;

    /** The size of a page. */
    Usize m_page_size;

    /** The list of pages. */
    Array<LogPage> m_pages;

    /** The index of the page where the next log will be inserted, if possible. */
    Usize m_active_page_index;

    /** The buffer where the message is formatted for console display or file writing. */
    Buffer m_formatting_buffer;

private:
    friend class GameEngine;
};

/**
 * Pointer to the global logger instance.
 * Provides access to the logging system during runtime.
 */
BASALT_API extern Logger* g_logger;

/**
 * Converts a log severity to a readable string.
 */
FORCEINLINE static constexpr StringView LogSeverityToString(ELogSeverity severity)
{
    switch (severity)
    {
        case ELogSeverity::Debug:   return "Debug"sv;
        case ELogSeverity::Trace:   return "Trace"sv;
        case ELogSeverity::Info:    return "Info"sv;
        case ELogSeverity::Warn:    return "Warn"sv;
        case ELogSeverity::Error:   return "Error"sv;
        case ELogSeverity::Fatal:   return "Fatal"sv;
    }

    return "Unknown"sv;
}

/**
 * Converts a log category to a readable string.
 */
FORCEINLINE static constexpr StringView LogCategoryToString(ELogCategory severity)
{
    switch (severity)
    {
        case ELogCategory::Core:        return "Core"sv;
        case ELogCategory::Engine:      return "Engine"sv;
        case ELogCategory::Editor:      return "Editor"sv;
        case ELogCategory::World:       return "World"sv;
        case ELogCategory::AssetSystem: return "AssetSystem"sv;
        case ELogCategory::Renderer:    return "Renderer"sv;
        case ELogCategory::RendererRHI: return "RendererRHI"sv;
        case ELogCategory::Audio:       return "Audio"sv;
        case ELogCategory::Network:     return "Network"sv;

        case ELogCategory::Game:        return "Game"sv;
        case ELogCategory::Game01:      return "Game01"sv;
        case ELogCategory::Game02:      return "Game02"sv;
        case ELogCategory::Game03:      return "Game03"sv;
        case ELogCategory::Game04:      return "Game04"sv;
    }

    return "Unknown"sv;
}

} // namespace Basalt
