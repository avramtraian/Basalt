// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "StringView.h"

#include "AsciiString.h"
#include "Utf8String.h"

namespace Basalt
{

StringView StringView::FromUTF8(const char* utf8_string)
{
    const Usize length = UTF8Calls::Length(utf8_string);
    Check(length > 0); // The provided string contains invalid UTF-8.
    return StringView(utf8_string, length);
}

StringView StringView::FromASCII(const char* ascii_string)
{
    return StringView(ascii_string, ASCIICalls::Length(ascii_string));
}

StringView StringView::Substring(Usize offset, Usize count /*= 0*/) const
{
    StringView substring;
    substring.m_view_data = m_view_data;

    for (Usize index = 0; index < offset; ++index)
    {
        U32 codepoint_width;
        UTF8Calls::BytesToCodepoint(substring.m_view_data, &codepoint_width);
        Check(codepoint_width > 0); // The view contains invalid UTF-8.
        substring.m_view_data += codepoint_width;
    }

    for (Usize index = 0; index < count; ++index)
    {
        U32 codepoint_width;
        UTF8Calls::BytesToCodepoint(substring.m_view_data + substring.m_bytes_count, &codepoint_width);
        Check(codepoint_width > 0); // The view contains invalid UTF-8.
        substring.m_bytes_count += codepoint_width;
    }

    return substring;
}

Usize StringView::FindFirstOf(StringView substring) const
{
    if (substring.m_bytes_count >= m_bytes_count || substring.IsEmpty())
    {
        return InvalidPos;
    }

    Usize offset_length = 0;
    for (Usize offset_bytes = 0; offset_bytes < m_bytes_count - substring.m_bytes_count; ++offset_length)
    {
        bool are_equal = true;
        for (Usize i = 0; i < substring.m_bytes_count; ++i)
        {
            if (m_view_data[offset_bytes + i] != substring.m_view_data[i])
            {
                are_equal = false;
                break;
            }
        }

        if (are_equal)
        {
            return offset_length;
        }

        const U32 width = UTF8Calls::BytesToCodepointWidth(m_view_data + offset_bytes);
        Check(width > 0); // The view contains invalid UTF-8.
        offset_bytes += width;
    }

    return InvalidPos;
}

Usize StringView::FindLastOf(StringView substring) const
{
    Check(false); // Not implemented.
    return InvalidPos;
}

bool StringView::operator==(const StringView& other) const
{
    if (m_bytes_count != other.m_bytes_count)
    {
        return false;
    }

    for (Usize i = 0; i < m_bytes_count; ++i)
    {
        if (m_view_data[i] != other.m_view_data[i])
        {
            return false;
        }
    }

    return true;
}

bool StringView::operator!=(const StringView& other) const
{
    return !(*this == other);
}

} // namespace Basalt
