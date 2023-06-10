// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "String.h"
#include "Core/Memory/Memory.h"

namespace Basalt
{

String::String()
    : m_bytesCount(1)
{
    // Set the null-terminating byte.
    m_heapData[0] = 0;
}
    
String::String(const String& other)
    : m_bytesCount(other.m_bytesCount)
{
    char* destinationBuffer;
    const char* sourceBuffer;

    if (m_bytesCount <= InlineCapacity)
    {
        destinationBuffer = m_inlineData;
        sourceBuffer = other.m_inlineData;
    }
    else
    {
        m_heapData = AllocateMemory(m_bytesCount);
        destinationBuffer = m_heapData;
        sourceBuffer = other.m_heapData;
    }

    Memory::Copy(destinationBuffer, sourceBuffer, m_bytesCount);
}

String::String(String&& other) noexcept
    : m_bytesCount(other.m_bytesCount)
    , m_heapData(other.m_heapData)
{
    other.m_bytesCount = 1;
    other.m_inlineData[0] = 0;
}

String::String(StringView view)
    : m_bytesCount(view.m_bytesCount + 1)
{
    char* destinationBuffer;

    if (m_bytesCount <= InlineCapacity)
    {
        destinationBuffer = m_inlineData;
    }
    else
    {
        m_heapData = AllocateMemory(m_bytesCount);
        destinationBuffer = m_heapData;
    }

    Memory::Copy(destinationBuffer, view.m_viewData, m_bytesCount - 1);
    destinationBuffer[m_bytesCount - 1] = 0;
}

String::~String()
{
    if (m_bytesCount > InlineCapacity)
    {
        ReleaseMemory(m_heapData, m_bytesCount);
    }
}

String& String::operator=(const String& other)
{
    StringView view;
    view.m_viewData = other.Data();
    view.m_bytesCount = other.m_bytesCount;

    AssignView(view);
    return *this;
}

String& String::operator=(String&& other) noexcept
{
    if (m_bytesCount > InlineCapacity)
    {
        ReleaseMemory(m_heapData, m_bytesCount);
    }

    m_bytesCount = other.m_bytesCount;
    m_heapData = other.m_heapData;

    other.m_bytesCount = 1;
    other.m_inlineData[0] = 0;

    return *this;
}

String& String::operator=(StringView view)
{
    AssignView(view);
    return *this;
}

char* String::AllocateMemory(Usize bytesCount)
{
    return (char*)Memory::AllocateTaggedI(bytesCount);
}

void String::ReleaseMemory(char* data, Usize bytesCount)
{
    Memory::Free(data);
}

void String::AssignView(StringView view)
{
    char* destinationBuffer;

    if (view.m_bytesCount < InlineCapacity)
    {
        destinationBuffer = m_inlineData;

        if (m_bytesCount > InlineCapacity)
        {
            ReleaseMemory(m_heapData, m_bytesCount);
        }
    }
    else
    {
        if (m_bytesCount <= InlineCapacity)
        {
            m_heapData = AllocateMemory(view.m_bytesCount + 1);
        }
        else if (m_bytesCount != view.m_bytesCount)
        {
            ReleaseMemory(m_heapData, m_bytesCount);
            m_heapData = AllocateMemory(view.m_bytesCount + 1);
        }

        destinationBuffer = m_heapData;
    }

    m_bytesCount = view.m_bytesCount + 1;
    Memory::Copy(destinationBuffer, view.m_viewData, view.m_bytesCount);
    destinationBuffer[m_bytesCount - 1] = 0;
}

} // namespace Basalt
