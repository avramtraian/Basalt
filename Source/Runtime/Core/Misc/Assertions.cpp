// Copyright(c) 2023 Basalt Technology Group.All rights reserved.

#include "AssertionMacros.h"

#include "Core/Containers/Strings/StringBuilder.h"
#include "Core/Logging/LogMacros.h"
#include "Core/Memory/Buffer.h"

#if BASALT_PLATFORM_WINDOWS
    #include <Windows.h>
#endif // BASALT_PLATFORM_WINDOWS

namespace Basalt
{

void Debug::ReportAssertFailed(const char* expression, const char* filepath, const char* function_name, U32 line_number, const char* message /*= nullptr*/, ...)
{
    // NOTE(traian): I hate this piece of code from the bottom of my heart.
    //               But, this is so low priority that I just don't care
    //               anymore about it. Maybe I will rework it at some point...

    ScopedBuffer user_message_buffer;
    
    if (message)
    {
        va_list arg_list;
        va_start(arg_list, message);
        StringBuilder::FormatDynamicList(user_message_buffer.RawBuffer(), StringView::FromUTF8(message), arg_list);
        va_end(arg_list);
    }
    else
    {
        StringView default_message = "No message available :(\0"sv;
        user_message_buffer.RawBuffer().Resize(default_message.BytesCount());
        Memory::Copy(user_message_buffer.Data(), *default_message, default_message.BytesCount());
    }

    BT_LOG_ERROR(Core, "--- ASSERTION FAILED ---");
    BT_LOG_ERROR(Core, "File: %s [%d]", filepath, line_number);
    BT_LOG_ERROR(Core, "");
    BT_LOG_ERROR(Core, "Expression: '%s'", expression);
    BT_LOG_ERROR(Core, "");
    BT_LOG_ERROR(Core, "%s", user_message_buffer.As<char>());
    BT_LOG_ERROR(Core, "-------------------------------------------------------------");

    ScopedBuffer content_buffer;
    StringBuilder::FormatDynamic(
        content_buffer.RawBuffer(), "File: %s [%d]\n\nExpression: '%s'\n\n%s"sv,
        filepath, line_number, expression, user_message_buffer.As<const char>());

    ScopedBuffer utf16_content_buffer;
    StringBuilder::ToUTF16Dynamic(StringView(content_buffer.As<char>(), content_buffer.Size() - 1), utf16_content_buffer.RawBuffer(), true);

    StringView title = "The Basalt-Runtime has crashed"sv;
    ScopedBuffer utf16_title_buffer;
    StringBuilder::ToUTF16Dynamic(title, utf16_title_buffer.RawBuffer(), true);

    // TODO(traian): Don't use the Win32 API directly. Implement a platform-agnostic interface for opening message pop-up's.
#if BASALT_PLATFORM_WINDOWS
    ::MessageBox(NULL, utf16_content_buffer.As<wchar_t>(), utf16_title_buffer.As<wchar_t>(), MB_ICONERROR | MB_OK);
#endif // BASALT_PLATFORM_WINDOWS
}

} // namespace Basalt
