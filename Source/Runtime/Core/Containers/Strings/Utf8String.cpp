// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "Utf8String.h"

#include "Core/Misc/AssertionMacros.h"

namespace Basalt
{

Usize UTF8Calls::Length(const char* string)
{
    Usize length = 0;
    
    while (*string)
    {
        const U32 codepoint_width = UTF8Calls::BytesToCodepointWidth(string);
        if (codepoint_width == 0)
        {
            return InvalidSize;
        }

        string += codepoint_width;
        ++length;
    }

    return length;
}

Usize UTF8Calls::BytesCount(const char* string, Usize length)
{
    Usize bytes_count = 0;

    for (Usize index = 0; index < length; ++index)
    {
        const U32 width = UTF8Calls::BytesToCodepointWidth(string + bytes_count);
        Check(width > 0); // Invalid UTF-8 string.
        bytes_count += width;
    }

    return bytes_count;
}

U32 UTF8Calls::CodepointToBytes(UnicodeCodepoint codepoint, void* buffer)
{
    U8* bytes = (U8*)buffer;

    if (0x0000 <= codepoint && codepoint <= 0x007F)
    {
        bytes[0] = (U8)codepoint;
        return 1;
    }

    if (0x0080 <= codepoint && codepoint <= 0x07FF)
    {
        bytes[0] = ((codepoint >> 6) & 0x1F) | 0xC0;
        bytes[1] = ((codepoint >> 0) & 0x3F) | 0x80;
        return 2;
    }

    if (0x0800 <= codepoint && codepoint <= 0xFFFF)
    {
        bytes[0] = ((codepoint >> 12) & 0x0F) | 0xE0;
        bytes[1] = ((codepoint >> 6)  & 0x3F) | 0x80;
        bytes[2] = ((codepoint >> 0)  & 0x3F) | 0x80;
        return 3;
    }

    if (0x10000 <= codepoint)
    {
        bytes[0] = ((codepoint >> 18) & 0x07) | 0xF0;
        bytes[1] = ((codepoint >> 12) & 0x3F) | 0x80;
        bytes[2] = ((codepoint >> 6)  & 0x3F) | 0x80;
        bytes[3] = ((codepoint >> 0)  & 0x3F) | 0x80;
        return 4;
    }

    return 0;
}

UnicodeCodepoint UTF8Calls::BytesToCodepoint(const void* buffer, U32* out_codepoint_width /*= nullptr*/)
{
    const U8* bytes = (const U8*)buffer;

    if ((bytes[0] & 0x80) == 0x00)
    {
        if (out_codepoint_width)
        {
            *out_codepoint_width = 1;
        }

        return bytes[0];
    }

    if ((bytes[0] & 0xE0) == 0xC0)
    {
        if (out_codepoint_width)
        {
            *out_codepoint_width = 2;
        }

        UnicodeCodepoint codepoint = 0;
        codepoint += (bytes[0] & 0x1F) << 6;
        codepoint += (bytes[1] & 0x3F) << 0;
        return codepoint;
    }

    if ((bytes[0] & 0xF0) == 0xE0)
    {
        if (out_codepoint_width)
        {
            *out_codepoint_width = 3;
        }

        UnicodeCodepoint codepoint = 0;
        codepoint += (bytes[0] & 0x1F) << 12;
        codepoint += (bytes[1] & 0x3F) << 6;
        codepoint += (bytes[2] & 0x3F) << 0;
        return codepoint;
    }

    if ((bytes[0] & 0xF8) == 0xF0)
    {
        if (out_codepoint_width)
        {
            *out_codepoint_width = 4;
        }

        UnicodeCodepoint codepoint = 0;
        codepoint += (bytes[0] & 0x1F) << 18;
        codepoint += (bytes[1] & 0x3F) << 12;
        codepoint += (bytes[2] & 0x3F) << 6;
        codepoint += (bytes[3] & 0x3F) << 0;
        return codepoint;
    }

    if (out_codepoint_width)
    {
        *out_codepoint_width = 0;
    }
    return InvalidUnicodeCodepoint;
}

U32 UTF8Calls::BytesToCodepointWidth(const void* buffer)
{
    const U8 byte = *(const U8*)buffer;

    if ((byte & 0x80) == 0x00)
    {
        return 1;
    }

    if ((byte & 0xE0) == 0xC0)
    {
        return 2;
    }

    if ((byte & 0xF0) == 0xE0)
    {
        return 3;
    }

    if ((byte & 0xF8) == 0xF0)
    {
        return 4;
    }

    return 0;
}

bool UTF8Calls::Validate(const char* string, Usize bytes_count)
{
    Usize offset = 0;
    while (offset < bytes_count)
    {
        U32 codepoint_width = UTF8Calls::BytesToCodepointWidth(string + offset);
        if (codepoint_width == 0)
        {
            return false;
        }

        offset += codepoint_width;
    }

    return (offset == bytes_count);
}

bool UTF8Calls::Validate(const char* null_terminated_string)
{
    while (*null_terminated_string)
    {
        U32 codepoint_width = UTF8Calls::BytesToCodepointWidth(null_terminated_string);
        if (codepoint_width == 0)
        {
            return false;
        }

        null_terminated_string += codepoint_width;
    }

    return true;
}

} // namespace Basalt
