// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "String.h"
#include "Core/Memory/Memory.h"

namespace Basalt
{

String::String()
    : m_bytes_count(1)
{
    // Set the null-terminating byte.
    m_inline_data[0] = 0;
}
    
String::String(const String& other)
    : m_bytes_count(other.m_bytes_count)
{
    char* destination_buffer;
    const char* source_buffer;

    if (m_bytes_count <= InlineCapacity)
    {
        destination_buffer = m_inline_data;
        source_buffer = other.m_inline_data;
    }
    else
    {
        m_heap_data = AllocateMemory(m_bytes_count);
        destination_buffer = m_heap_data;
        source_buffer = other.m_heap_data;
    }

    Memory::Copy(destination_buffer, source_buffer, m_bytes_count);
}

String::String(String&& other) noexcept
    : m_bytes_count(other.m_bytes_count)
    , m_heap_data(other.m_heap_data)
{
    other.m_bytes_count = 1;
    other.m_inline_data[0] = 0;
}

String::String(StringView view)
    : m_bytes_count(view.m_bytesCount + 1)
{
    char* destinationBuffer;

    if (m_bytes_count <= InlineCapacity)
    {
        destinationBuffer = m_inline_data;
    }
    else
    {
        m_heap_data = AllocateMemory(m_bytes_count);
        destinationBuffer = m_heap_data;
    }

    Memory::Copy(destinationBuffer, view.m_viewData, m_bytes_count - 1);
    destinationBuffer[m_bytes_count - 1] = 0;
}

String::~String()
{
    if (m_bytes_count > InlineCapacity)
    {
        ReleaseMemory(m_heap_data, m_bytes_count);
    }
}

String& String::operator=(const String& other)
{
    StringView view;
    view.m_viewData = other.Data();
    view.m_bytesCount = other.m_bytes_count;

    AssignView(view);
    return *this;
}

String& String::operator=(String&& other) noexcept
{
    if (m_bytes_count > InlineCapacity)
    {
        ReleaseMemory(m_heap_data, m_bytes_count);
    }

    m_bytes_count = other.m_bytes_count;
    m_heap_data = other.m_heap_data;

    other.m_bytes_count = 1;
    other.m_inline_data[0] = 0;

    return *this;
}

String& String::operator=(StringView view)
{
    AssignView(view);
    return *this;
}

char* String::AllocateMemory(Usize bytes_count)
{
    return (char*)Memory::AllocateTaggedI(bytes_count);
}

void String::ReleaseMemory(char* data, Usize bytes_count)
{
    Memory::Free(data);
}

void String::AssignView(StringView view)
{
    char* destinationBuffer;

    if (view.m_bytesCount < InlineCapacity)
    {
        destinationBuffer = m_inline_data;

        if (m_bytes_count > InlineCapacity)
        {
            ReleaseMemory(m_heap_data, m_bytes_count);
        }
    }
    else
    {
        if (m_bytes_count <= InlineCapacity)
        {
            m_heap_data = AllocateMemory(view.m_bytesCount + 1);
        }
        else if (m_bytes_count != view.m_bytesCount)
        {
            ReleaseMemory(m_heap_data, m_bytes_count);
            m_heap_data = AllocateMemory(view.m_bytesCount + 1);
        }

        destinationBuffer = m_heap_data;
    }

    m_bytes_count = view.m_bytesCount + 1;
    Memory::Copy(destinationBuffer, view.m_viewData, view.m_bytesCount);
    destinationBuffer[m_bytes_count - 1] = 0;
}

} // namespace Basalt
