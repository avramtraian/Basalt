// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "Utf16String.h"

#include "Core/Misc/AssertionMacros.h"

namespace Basalt
{

Usize UTF16Calls::Length(const wchar_t* string)
{
    Usize length = 0;

    while (*string)
    {
        const U32 codepoint_width = UTF16Calls::BytesToCodepointWidth(string);
        if (codepoint_width == 0)
        {
            return InvalidSize;
        }

        string += (codepoint_width / 2);
        ++length;
    }

    return length;
}

Usize UTF16Calls::BytesCount(const wchar_t* string, Usize length)
{
    Usize bytes_count = 0;

    for (Usize index = 0; index < length; ++index)
    {
        const U32 width = UTF16Calls::BytesToCodepointWidth((U8*)string + bytes_count);
        Check(width > 0); // Invalid UTF-16 string.
        bytes_count += width;
    }

    return bytes_count;
}

U32 UTF16Calls::CodepointToBytes(UnicodeCodepoint codepoint, void* buffer)
{
    U16* bytes = (U16*)buffer;

    if (0x0000 <= codepoint && codepoint <= 0xD7FF)
    {
        bytes[0] = (U16)codepoint;
        return 2;
    }
    if (0xE000 <= codepoint && codepoint <= 0xFFFF)
    {
        bytes[0] = (U16)codepoint;
        return 2;
    }

    if (0x010000 <= codepoint && codepoint <= 0x10FFFF)
    {
        const U32 prime = codepoint - 0x10000;
        bytes[0] = 0xD800 + (prime >> 10); // High 10-bits.
        bytes[1] = 0xDC00 + (prime & 0x3FF); // Low 10-bits.
        return 4;
    }

    // Invalid UTF-16 codepoint.
    return 0;
}

UnicodeCodepoint UTF16Calls::BytesToCodepoint(const void* buffer, U32* out_codepoint_width /*= nullptr*/)
{
    const U16* bytes = (const U16*)buffer;

    if (0x0000 <= bytes[0] && bytes[0] <= 0xD7FF)
    {
        if (out_codepoint_width)
        {
            *out_codepoint_width = 2;
        }

        return bytes[0];
    }
    if (0xE000 <= bytes[0] && bytes[0] <= 0xFFFF)
    {
        if (out_codepoint_width)
        {
            *out_codepoint_width = 2;
        }

        return bytes[0];
    }

    if ((bytes[0] & 0xFC00) == 0xD800)
    {
        if (out_codepoint_width)
        {
            *out_codepoint_width = 4;
        }

        UnicodeCodepoint codepoint = 0;
        codepoint += (bytes[0] & 0x3FF) << 10;
        codepoint += (bytes[1] & 0x3FF) << 0;
        return codepoint;
    }

    if (out_codepoint_width)
    {
        *out_codepoint_width = 0;
    }
    return InvalidUnicodeCodepoint;
}

U32 UTF16Calls::BytesToCodepointWidth(const void* buffer)
{
    const U16 byte = *(const U16*)buffer;

    if (0x0000 <= byte && byte <= 0xD7FF)
    {
        return 2;
    }
    if (0xE000 <= byte && byte <= 0xFFFF)
    {
        return 2;
    }

    if ((byte & 0xFC00) == 0xD800)
    {
        return 4;
    }

    // Invalid UTF-16 byte sequence.
    return 0;
}

bool UTF16Calls::Validate(const wchar_t* string, Usize bytes_count)
{
    Usize offset = 0;
    while (offset < bytes_count)
    {
        U32 codepoint_width = UTF16Calls::BytesToCodepointWidth(string + (bytes_count / 2));
        if (codepoint_width == 0)
        {
            return false;
        }

        offset += bytes_count;
    }

    return (offset == bytes_count);
}

bool UTF16Calls::Validate(const wchar_t* null_terminated_string)
{
    while (*null_terminated_string)
    {
        U32 codepoint_width = UTF16Calls::BytesToCodepointWidth(null_terminated_string);
        if (codepoint_width == 0)
        {
            return false;
        }

        null_terminated_string += (codepoint_width / 2);
    }

    return true;
}

} // namespace Basalt
