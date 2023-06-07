// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "Logger.h"
#include "Core/Misc/AssertionMacros.h"
#include "Core/Containers/Utf8String.h"
#include "Core/Memory/Memory.h"
#include "Core/Platform/Platform.h"

#include <cstdio>
#include <cstdarg>

namespace Basalt
{

BASALT_API Logger* g_logger = nullptr;

bool Logger::Initialize(const LoggerDescription& description)
{
    Check(g_logger == nullptr);
    if (g_logger)
    {
        // The logger instance was already created.
        return false;
    }

    g_logger = bnew Logger();

    Check(g_logger);
    if (!g_logger)
    {
        // Failed to allocate memory for the logger instance.
        return false;
    }

    g_logger->m_filter = LogFilter::All();
    g_logger->ParseCommandLine();
    g_logger->CreatePages(description);

    g_logger->m_formatting_buffer.Invalidate(g_logger->m_page_size + 32);

    Platform::CreateConsole();
    
    // Initialization is successful.
    return true;
}

void Logger::Shutdown()
{
    Check(g_logger);
    if (!g_logger)
    {
        // The logger instance was already destroyed or wasn't created.
        return;
    }

    Platform::DestroyConsole();

    g_logger->m_pages_buffer.Release();
    g_logger->m_formatting_buffer.Release();

    delete g_logger;
    g_logger = nullptr;
}

namespace Utils
{

FORCEINLINE static constexpr LinearColor SeverityToTextColor(ELogSeverity severity)
{
    switch (severity)
    {
        case ELogSeverity::Debug: return LinearColor(1.0F, 0.0F, 1.0F, 0.0F);
        case ELogSeverity::Trace: return LinearColor(0.0F, 0.0F, 0.0F, 1.0F);
        case ELogSeverity::Info:  return LinearColor(0.0F, 1.0F, 0.0F, 0.0F);
        case ELogSeverity::Warn:  return LinearColor(1.0F, 1.0F, 0.0F, 1.0F);
        case ELogSeverity::Error: return LinearColor(1.0F, 0.0F, 0.0F, 1.0F);
        case ELogSeverity::Fatal: return LinearColor(1.0F, 1.0F, 1.0F, 1.0F);
    }

    return {};
}

FORCEINLINE static constexpr LinearColor SeverityToBackgroundColor(ELogSeverity severity)
{
    switch (severity)
    {
        case ELogSeverity::Debug: return LinearColor(0.0F, 0.0F, 0.0F, 0.0F);
        case ELogSeverity::Trace: return LinearColor(0.0F, 0.0F, 0.0F, 0.0F);
        case ELogSeverity::Info:  return LinearColor(0.0F, 0.0F, 0.0F, 0.0F);
        case ELogSeverity::Warn:  return LinearColor(0.0F, 0.0F, 0.0F, 0.0F);
        case ELogSeverity::Error: return LinearColor(0.0F, 0.0F, 0.0F, 0.0F);
        case ELogSeverity::Fatal: return LinearColor(1.0F, 0.0F, 0.0F, 0.0F);
    }

    return {};
}

} // namespace Basalt::Utils

void Logger::Submit(ELogCategory category, ELogSeverity severity, const char* format, ...)
{
    if (!m_filter.HasCategory(category) || !m_filter.HasSeverity(severity))
    {
        return;
    }

    va_list arg_list;
    va_start(arg_list, format);
    StringView message = WriteToPage(category, severity, format, arg_list);
    va_end(arg_list);

    DateTime date_time = DateTime::Now();

    Usize written = snprintf(
        m_formatting_buffer.As<char>(), m_formatting_buffer.size,
        "[%02d:%02d:%02d][%s][%s]: %s\n",
        date_time.hour, date_time.minute, date_time.second,
        LogSeverityToString(severity).c_str(), LogCategoryToString(category).c_str(), message.c_str()
    );

    Check(written <= m_formatting_buffer.size); // Formatting failed (no space).

    StringView log = StringView(m_formatting_buffer.As<const char>(), written);
    Platform::WriteToConsole(log, Utils::SeverityToTextColor(severity), Utils::SeverityToBackgroundColor(severity));
}

void Logger::IterateLogs(LogFilter filter, LogPredicate log_predicate) const
{
    Usize page_index = (m_active_page_index + 1) % m_pages.Count();
    
    for (Usize index = 0; index < m_pages.Count(); ++index)
    {
        const LogPage& page = m_pages[page_index];

        Usize offset = 0;
        for (Usize log_index = 0; log_index < page.logs_count; ++log_index)
        {
            LogHeader* header = (LogHeader*)(m_pages_buffer.data + page.buffer_offset + offset);
            offset += sizeof(LogHeader) + header->message_bytes_count;

            LogDefinition definition = {};
            definition.category = header->category;
            definition.severity = header->severity;

            if (filter.HasSeverity(header->severity) && filter.HasCategory(header->category))
            {
                definition.message = StringView((const char*)(header + 1), header->message_bytes_count - 1);
                if (log_predicate(definition) == IterationDecision::Break)
                {
                    return;
                }
            }
        }

       page_index = (page_index + 1) % m_pages.Count();
    }
}

void Logger::ParseCommandLine()
{
    // Parse the command line to set the logging categories.
    // If no arguments are specified, all categories will be included.

    LogFilter filter;
    bool update_filter_categories = false;

#define QUERY_COMMAND_LINE_FLAG(NAME, ENUM)         \
    if (CommandLine::HasFlag("log-"##NAME##sv))     \
    {                                               \
        filter.AddCategory(ELogCategory::ENUM);     \
        update_filter_categories = true;            \
    }

    QUERY_COMMAND_LINE_FLAG("core",         Core);
    QUERY_COMMAND_LINE_FLAG("engine",       Engine);
    QUERY_COMMAND_LINE_FLAG("editor",       Editor);
    QUERY_COMMAND_LINE_FLAG("world",        World);
    QUERY_COMMAND_LINE_FLAG("asset-system", AssetSystem);
    QUERY_COMMAND_LINE_FLAG("renderer",     Renderer);
    QUERY_COMMAND_LINE_FLAG("renderer-rhi", RendererRHI);
    QUERY_COMMAND_LINE_FLAG("audio",        Audio);
    QUERY_COMMAND_LINE_FLAG("network",      Network);

#undef QUERY_COMMAND_LINE_FLAG

    if (update_filter_categories)
    {
        m_filter.RemoveAllCategories();
        m_filter = LogFilter::Union(m_filter, filter);
    }

    // Read the command line to set the logging lowest severity.
    // If no arguments are specified, all severities allowed in the build configuration will be included.

    Optional<StringView> verbosity = CommandLine::GetSwitchValue("log-verbosity"sv);
    if (verbosity)
    {
        m_filter.RemoveAllSeverities();

        // NOTE: If an invalid value is provided to the 'log-verbosity' switch, the debug severity will be set.
        ELogSeverity severity_level = ELogSeverity::Debug;

        if (*verbosity == "debug"sv)    { severity_level = ELogSeverity::Debug; }
        if (*verbosity == "trace"sv)    { severity_level = ELogSeverity::Trace; }
        if (*verbosity == "info"sv)     { severity_level = ELogSeverity::Info; }
        if (*verbosity == "warn"sv)     { severity_level = ELogSeverity::Warn; }
        if (*verbosity == "error"sv)    { severity_level = ELogSeverity::Error; }
        if (*verbosity == "fatal"sv)    { severity_level = ELogSeverity::Fatal; }

        for (U8 severity = (U8)severity_level; severity < (U8)ELogSeverity::MaxEnumValue; ++severity)
        {
            m_filter.AddSeverity((ELogSeverity)severity);
        }
    }
}

void Logger::CreatePages(const LoggerDescription& description)
{
    m_page_size = description.page_size;
    m_pages.SetCountDefaulted(description.page_count);

    Usize offset = 0;
    for (LogPage& page : m_pages)
    {
        page.buffer_offset = offset;
        page.insert_offset = 0;
        page.logs_count = 0;

        offset += m_page_size;
    }

    m_pages_buffer.Invalidate(m_pages.Count() * m_page_size);
}

StringView Logger::WriteToPage(ELogCategory category, ELogSeverity severity, const char* format, va_list arg_list)
{
    // Buffer that is big enough to store most logs.
    U8 stack_buffer[512] = {};

    Usize message_bytes_count;
    U8* heap_buffer = FormatLogMessage(Span<U8>(stack_buffer, BT_ARRAY_COUNT(stack_buffer)), &message_bytes_count, format, arg_list);
    U8* buffer = heap_buffer ? heap_buffer : stack_buffer;
    
    LogPage* active_page = &m_pages[m_active_page_index];

    // The log message must fit on a single empty page. Otherwise, there is no way the message can be stored
    // without increasing the log page size.
    // NOTE (traian): Think about wrapping messages across multiple pages.
    Check(sizeof(LogHeader) + message_bytes_count <= m_page_size);

    if (active_page->insert_offset + sizeof(LogHeader) + message_bytes_count > m_page_size)
    {
        m_active_page_index = (m_active_page_index + 1) % m_pages.Count();
        active_page = &m_pages[m_active_page_index];
        active_page->insert_offset = 0;
        active_page->logs_count = 0;
    }

    LogHeader* header = (LogHeader*)(m_pages_buffer.data + active_page->buffer_offset + active_page->insert_offset);
    active_page->insert_offset += sizeof(LogHeader) + message_bytes_count;
    active_page->logs_count++;

    header->category = category;
    header->severity = severity;
    header->message_bytes_count = message_bytes_count;

    char* message_data = (char*)(header + 1);
    Memory::Copy(message_data, buffer, message_bytes_count);
    if (heap_buffer)
    {
        delete[] heap_buffer;
    }

    return StringView(message_data, message_bytes_count - 1);
}

U8* Logger::FormatLogMessage(Span<U8> stack_buffer, Usize* out_written_bytes, const char* format, va_list arg_list)
{
    Usize buffer_size = stack_buffer.Count();
    U8* heap_buffer = nullptr;

    Usize written = vsnprintf((char*)stack_buffer.Data(), buffer_size, format, arg_list);
    while (written >= buffer_size)
    {
        buffer_size *= 2;
        delete[] heap_buffer;
        heap_buffer = bnew U8[buffer_size];

        written = vsnprintf((char*)heap_buffer, buffer_size, format, arg_list);
    }

    *out_written_bytes = written + 1;
    return heap_buffer;
}

} // namespace Basalt
