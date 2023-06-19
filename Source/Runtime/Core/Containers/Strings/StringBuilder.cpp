// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "StringBuilder.h"

#include "Utf16String.h"
#include "Utf8String.h"

namespace Basalt
{

Usize StringBuilder::ToUTF16(StringView utf8_view, Buffer utf16_buffer, bool include_null_termination_char)
{
    Usize offset = 0;
    Usize buffer_offset = 0;

    while (offset < utf8_view.BytesCount())
    {
        U32 codepoint_width;
        UnicodeCodepoint codepoint = UTF8Calls::BytesToCodepoint(utf8_view.Data() + offset, &codepoint_width);
        Check(codepoint_width > 0); // Invalid UTF-8 string.
        offset += codepoint_width;

        U16 buffer[2] = {};
        U32 written = UTF16Calls::CodepointToBytes(codepoint, buffer);

        if (buffer_offset + written > utf16_buffer.size)
        {
            return InvalidSize;
        }

        Memory::Copy(utf16_buffer.data + buffer_offset, buffer, written);
        buffer_offset += written;
    }

    if (include_null_termination_char && utf16_buffer.As<wchar_t>()[buffer_offset / 2 - 1] != 0)
    {
        if (buffer_offset + sizeof(wchar_t) > utf16_buffer.size)
        {
            // The buffer is not sufficient to also store the null-termination character.
            return InvalidSize;
        }

        utf16_buffer.As<wchar_t>()[buffer_offset / 2] = 0;
        buffer_offset += sizeof(wchar_t);
    }

    return buffer_offset;
}

Usize StringBuilder::ToUTF16Dynamic(StringView utf8_view, Buffer& utf16_buffer, bool include_null_termination_char)
{
    if (utf16_buffer.size < utf8_view.BytesCount() * 2)
    {
        // The minimum size the buffer can be.
        // This size is reached only if the source string is composed from ASCII characters entirely.
        utf16_buffer.Invalidate(utf8_view.BytesCount() * 2);
    }

    Usize offset = 0;
    Usize buffer_offset = 0;

    while (offset < utf8_view.BytesCount())
    {
        U32 codepoint_width;
        UnicodeCodepoint codepoint = UTF8Calls::BytesToCodepoint(utf8_view.Data() + offset, &codepoint_width);
        Check(codepoint_width > 0); // Invalid UTF-8 string.
        offset += codepoint_width;

        U16 buffer[2] = {};
        U32 written = UTF16Calls::CodepointToBytes(codepoint, buffer);

        if (buffer_offset + written > utf16_buffer.size)
        {
            utf16_buffer.Resize(utf16_buffer.size + utf16_buffer.size / 2);
        }

        Memory::Copy(utf16_buffer.data + buffer_offset, buffer, written);
        buffer_offset += written;
    }

    if (include_null_termination_char && utf16_buffer.As<wchar_t>()[buffer_offset / 2 - 1] != 0)
    {
        if (buffer_offset + sizeof(wchar_t) > utf16_buffer.size)
        {
            utf16_buffer.Resize(buffer_offset + sizeof(wchar_t));
        }

        utf16_buffer.As<wchar_t>()[buffer_offset / 2] = 0;
        buffer_offset += sizeof(wchar_t);
    }

    return buffer_offset;
}

Usize StringBuilder::FromUTF16(const wchar_t* utf16_string, Buffer utf8_buffer, bool include_null_termination_char)
{
    Usize buffer_offset = 0;

    while (*utf16_string)
    {
        U32 codepoint_width;
        UnicodeCodepoint codepoint = UTF16Calls::BytesToCodepoint(utf16_string, &codepoint_width);
        Check(codepoint_width > 0); // Invalid UTF-16 string.
        utf16_string += codepoint_width / 2;

        U8 buffer[4] = {};
        U32 written = UTF8Calls::CodepointToBytes(codepoint, buffer);

        if (buffer_offset + written > utf8_buffer.size)
        {
            // The buffer is not sufficient.
            return InvalidSize;
        }

        Memory::Copy(utf8_buffer.data + buffer_offset, buffer, written);
        buffer_offset += written;
    }

    if (include_null_termination_char)
    {
        if (buffer_offset == utf8_buffer.size)
        {
            // The buffer is not sufficient to also store the null-termination character.
            return InvalidSize;
        }

        utf8_buffer.data[buffer_offset] = 0;
        ++buffer_offset;
    }

    return buffer_offset;
}

Usize StringBuilder::FromUTF16Dynamic(const wchar_t* utf16_string, Buffer& utf8_buffer, bool include_null_termination_char)
{
    Usize buffer_offset = 0;

    while (*utf16_string)
    {
        U32 codepoint_width;
        UnicodeCodepoint codepoint = UTF16Calls::BytesToCodepoint(utf16_string, &codepoint_width);
        Check(codepoint_width > 0); // Invalid UTF-16 string.
        utf16_string += codepoint_width / 2;

        U8 buffer[4] = {};
        U32 written = UTF8Calls::CodepointToBytes(codepoint, buffer);

        if (buffer_offset + written > utf8_buffer.size)
        {
            utf8_buffer.Resize(2 * utf8_buffer.size);
        }

        Memory::Copy(utf8_buffer.data + buffer_offset, buffer, written);
        buffer_offset += written;
    }

    if (include_null_termination_char)
    {
        if (buffer_offset == utf8_buffer.size)
        {
            utf8_buffer.Resize(buffer_offset + sizeof(char));
        }

        utf8_buffer.data[buffer_offset] = 0;
        ++buffer_offset;
    }

    return buffer_offset;
}

} // namespace Basalt
