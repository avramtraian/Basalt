// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "StringBuilder.h"

#include "Utf16String.h"
#include "Utf8String.h"

namespace Basalt
{

Usize StringBuilder::ToUTF16(StringView utf8_view, Buffer utf16_buffer)
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

    return buffer_offset;
}

Usize StringBuilder::DynamicToUTF16(StringView utf8_view, Buffer& utf16_buffer)
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
            Buffer new_buffer = Buffer(utf16_buffer.size + utf16_buffer.size / 2);
            Memory::Copy(new_buffer.data, utf16_buffer.data, buffer_offset);
            utf16_buffer.Release();
            utf16_buffer = new_buffer;
        }

        Memory::Copy(utf16_buffer.data + buffer_offset, buffer, written);
        buffer_offset += written;
    }

    return buffer_offset;
}

void StringBuilder::SetCapacity(Usize new_capacity)
{

}

} // namespace Basalt
