// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "Utf8String.h"

namespace Basalt
{

Usize UTF8Calls::Length(const char* string)
{
    Usize length = 0;
    
    while (*string)
    {
        const U32 codepointWidth = UTF8Calls::BytesToCodepointWidth(string);
        if (codepointWidth == 0)
        {
            return InvalidSize;
        }

        string += codepointWidth;
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

U32 UTF8Calls::CodepointToBytes(UTF8Codepoint codepoint, void* buffer)
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

UTF8Codepoint UTF8Calls::BytesToCodepoint(const void* buffer, U32* outCodepointWidth /*= nullptr*/)
{
    const U8* bytes = (const U8*)buffer;

    if ((bytes[0] & 0x80) == 0x00)
    {
        if (outCodepointWidth)
        {
            *outCodepointWidth = 1;
        }

        return bytes[0];
    }

    if ((bytes[0] & 0xE0) == 0xC0)
    {
        if (outCodepointWidth)
        {
            *outCodepointWidth = 2;
        }

        UTF8Codepoint codepoint = 0;
        codepoint += (bytes[0] & 0x1F) << 6;
        codepoint += (bytes[1] & 0x3F) << 0;
        return codepoint;
    }

    if ((bytes[0] & 0xF0) == 0xE0)
    {
        if (outCodepointWidth)
        {
            *outCodepointWidth = 3;
        }

        UTF8Codepoint codepoint = 0;
        codepoint += (bytes[0] & 0x1F) << 12;
        codepoint += (bytes[1] & 0x3F) << 6;
        codepoint += (bytes[2] & 0x3F) << 0;
        return codepoint;
    }

    if ((bytes[0] & 0xF8) == 0xF0)
    {
        if (outCodepointWidth)
        {
            *outCodepointWidth = 4;
        }

        UTF8Codepoint codepoint = 0;
        codepoint += (bytes[0] & 0x1F) << 18;
        codepoint += (bytes[1] & 0x3F) << 12;
        codepoint += (bytes[2] & 0x3F) << 6;
        codepoint += (bytes[3] & 0x3F) << 0;
        return codepoint;
    }

    if (outCodepointWidth)
    {
        *outCodepointWidth = 0;
    }
    return InvalidUTF8Codepoint;
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

} // namespace Basalt
