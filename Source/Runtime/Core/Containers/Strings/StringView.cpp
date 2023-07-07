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

    if (count > 0)
    {
        for (Usize index = 0; index < count; ++index)
        {
            U32 codepoint_width;
            UTF8Calls::BytesToCodepoint(substring.m_view_data + substring.m_bytes_count, &codepoint_width);
            Check(codepoint_width > 0); // The view contains invalid UTF-8.
            substring.m_bytes_count += codepoint_width;
        }
    }
    else
    {
        Usize offset = substring.m_view_data - m_view_data;
        while (offset < m_bytes_count)
        {
            U32 codepoint_width;
            UTF8Calls::BytesToCodepoint(substring.m_view_data + substring.m_bytes_count, &codepoint_width);
            Check(codepoint_width > 0); // The view contains invalid UTF-8.
            substring.m_bytes_count += codepoint_width;
            offset += codepoint_width;
        }
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

Usize StringView::FindFirstOf(char character) const
{
    if (IsEmpty())
    {
        // The string view is empty.
        return InvalidPos;
    }

    for (Usize index = 0; index < m_bytes_count; ++index)
    {
        if (m_view_data[index] == character)
        {
            // The first occurrence of the given character was found.
            return index;
        }
    }

    // The character doesn't exist in the string view.
    return InvalidPos;
}

Usize StringView::FindLastOf(StringView substring) const
{
    Check(false); // Not implemented.
    return InvalidPos;
}

Usize StringView::FindLastOf(char character) const
{
    if (IsEmpty())
    {
        // The string view is empty.
        return InvalidPos;
    }

    for (Usize index = 1; index <= m_bytes_count; ++index)
    {
        if (m_view_data[m_bytes_count - index] == character)
        {
            // The last occurrence of the given character was found.
            return m_bytes_count - index;
        }
    }

    // The character doesn't exist in the string view.
    return InvalidPos;
}

bool StringView::BeginsWith(StringView substring) const
{
    if (substring.m_bytes_count > m_bytes_count)
    {
        // It is impossible for the string to begin with the given substring.
        return false;
    }

    for (Usize index = 0; index < substring.m_bytes_count; ++index)
    {
        if (m_view_data[index] != substring.m_view_data[index])
        {
            // The string doesn't begin with the given substring.
            return false;
        }
    }

    // The string begins with the given substring.
    return true;
}

bool StringView::BeginsWith(char character) const
{
    if (IsEmpty())
    {
        // The string view is empty.
        return false;
    }

    return (m_view_data[0] == character);
}

bool StringView::EndsWith(StringView substring) const
{
    if (substring.m_bytes_count > m_bytes_count)
    {
        // It is impossible for the string to end with the given substring.
        return false;
    }

    const Usize offset = m_bytes_count - substring.m_bytes_count;
    for (Usize index = 0; index < substring.m_bytes_count; ++index)
    {
        if (m_view_data[offset + index] != substring.m_view_data[index])
        {
            // The string doesn't end with the given substring.
            return false;
        }
    }

    // The string ends with the given substring.
    return true;
}

bool StringView::EndsWith(char character) const
{
    if (IsEmpty())
    {
        // The string view is empty.
        return false;
    }

    return (m_view_data[m_bytes_count - 1] == character);
}

StringView StringView::Stem() const
{
    Usize slash_position = FindLastOf('/');
    if (slash_position == InvalidPos)
    {
        // There is no slash in the filepath.
        slash_position = 0;
    }
    else
    {
        // The position of the first character that forms the filename is required.
        ++slash_position;
    }

    Usize dot_position = FindLastOf('.');
    if (dot_position == InvalidPos)
    {
        // There are isn't a dot in the filepath, meaning that the path represents a directory.
        dot_position = m_bytes_count;
    }

    // Construct the view towards the name of the file or directory using the found offsets.
    return StringView(m_view_data + slash_position, dot_position - slash_position);
}

StringView StringView::Extension() const
{
    Usize dot_position = FindLastOf('.');
    if (dot_position == InvalidPos)
    {
        // The path doesn't contain a file extension.
        return StringView();
    }

    // Construct a view towards the extension of the file using the dot offset.
    return StringView(m_view_data + dot_position, m_bytes_count - dot_position);
}

StringView StringView::Filename() const
{
    Usize slash_position = FindLastOf('/');
    if (slash_position == InvalidPos)
    {
        // The path doesn't contain a slash.
        slash_position = 0;
    }
    else
    {
        ++slash_position;
    }

    // Construct a view towards the name of the file using the slash offset.
    return StringView(m_view_data + slash_position, m_bytes_count - slash_position);
}

StringView StringView::ParentDirectory() const
{
    Usize slash_position = FindLastOf('/');
    if (slash_position == InvalidPos)
    {
        // The directory path doesn't exist in the filepath.
        return StringView();
    }

    // Construct a view towards the parent directory using the slash offset.
    return StringView(m_view_data, slash_position);
}

StringView StringView::ParentDirectory(U32 depth) const
{
    StringView result = *this;

    for (U32 index = 0; index < depth; ++index)
    {
        result = result.ParentDirectory();
    }

    return result;
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
