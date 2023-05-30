// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "StringView.h"

#include "AsciiString.h"
#include "Utf8String.h"

namespace Basalt
{

StringView StringView::FromUTF8(const char* utf8String)
{
    const Usize length = UTF8Calls::Length(utf8String);
    Check(length > 0); // The provided string contains invalid UTF-8.
    return StringView(utf8String, length);
}

StringView StringView::FromASCII(const char* asciiString)
{
    return StringView(asciiString, ASCIICalls::Length(asciiString));
}

StringView StringView::Substring(Usize offset, Usize count /*= 0*/) const
{
    StringView substring;
    substring.m_viewData = m_viewData;

    for (Usize i = 0; i < offset; ++i)
    {
        U32 codepointWidth;
        UTF8Calls::BytesToCodepoint(substring.m_viewData, &codepointWidth);
        Check(codepointWidth > 0); // The view contains invalid UTF-8.
        substring.m_viewData += codepointWidth;
    }

    for (Usize i = 0; i < count; ++i)
    {
        U32 codepointWidth;
        UTF8Calls::BytesToCodepoint(substring.m_viewData + substring.m_bytesCount, &codepointWidth);
        Check(codepointWidth > 0); // The view contains invalid UTF-8.
        substring.m_bytesCount += codepointWidth;
    }

    return substring;
}

Usize StringView::FindFirstOf(StringView substring) const
{
    if (substring.m_bytesCount >= m_bytesCount || substring.IsEmpty())
    {
        return InvalidPos;
    }

    Usize offsetLength = 0;
    for (Usize offset = 0; offset < m_bytesCount - substring.m_bytesCount; ++offsetLength)
    {
        bool areEqual = true;
        for (Usize i = 0; i < substring.m_bytesCount; ++i)
        {
            if (m_viewData[offset + i] != substring.m_viewData[i])
            {
                areEqual = false;
                break;
            }
        }

        if (areEqual)
        {
            return offsetLength;
        }

        const U32 width = UTF8Calls::BytesToCodepointWidth(m_viewData + offset);
        Check(width > 0); // The view contains invalid UTF-8.
        offset += width;
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
    if (m_bytesCount != other.m_bytesCount)
    {
        return false;
    }

    for (Usize i = 0; i < m_bytesCount; ++i)
    {
        if (m_viewData[i] != other.m_viewData[i])
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
