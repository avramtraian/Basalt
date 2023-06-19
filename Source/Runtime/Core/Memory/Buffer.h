// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/Math/MathUtilities.h"
#include "Memory.h"

namespace Basalt
{

class BASALT_API Buffer
{
public:
    FORCEINLINE Buffer()
        : data(nullptr)
        , size(0)
    {}

    FORCEINLINE Buffer(const Buffer& other)
        : data(other.data)
        , size(other.size)
    {}

    FORCEINLINE Buffer& operator=(const Buffer& other)
    {
        data = other.data;
        size = other.size;
        return *this;
    }

    FORCEINLINE explicit Buffer(Usize initial_size)
        : data(nullptr)
        , size(0)
    {
        Invalidate(initial_size);
    }

    FORCEINLINE Buffer(void* in_data, Usize in_size)
        : data((U8*)in_data)
        , size(in_size)
    {}

public:
    FORCEINLINE void Invalidate(Usize in_size)
    {
        Release();

        data = btnew U8[in_size];
        size = in_size;
    }

    FORCEINLINE void Release()
    {
        btdelete[] data;
        data = nullptr;
        size = 0;
    }

    template<typename T>
    FORCEINLINE T* As() const { return (T*)data; }

    template<typename T>
    FORCEINLINE Usize CountOf() const { return (size / sizeof(T)); }

    FORCEINLINE void Resize(Usize new_size)
    {
        if (new_size == size)
        {
            return;
        }
        
        Buffer new_buffer = Buffer(new_size);
        Memory::Copy(new_buffer.data, data, Math::Min(size, new_size));
        
        Release();
        *this = new_buffer;
    }

public:
    U8* data;
    Usize size;
};

struct BASALT_API ScopedBuffer
{
public:
    ScopedBuffer() = default;

    FORCEINLINE explicit ScopedBuffer(Usize initial_size)
        : m_buffer(initial_size)
    {}

    FORCEINLINE explicit ScopedBuffer(Buffer buffer)
        : m_buffer(buffer)
    {}

    FORCEINLINE ~ScopedBuffer()
    {
        Release();
    }

public:
    FORCEINLINE U8* Data() const { return m_buffer.data; }

    FORCEINLINE Usize Size() const { return m_buffer.size; }

    FORCEINLINE Buffer& RawBuffer() { return m_buffer; }
    FORCEINLINE const Buffer& RawBuffer() const { return m_buffer; }

public:
    FORCEINLINE void Invalidate(Usize in_size)
    {
        m_buffer.Invalidate(in_size);
    }

    FORCEINLINE void Release()
    {
        m_buffer.Release();
    }

    template<typename T>
    FORCEINLINE T* As() const { return m_buffer.As<T>(); }

    template<typename T>
    FORCEINLINE Usize CountOf() const { return m_buffer.CountOf<T>(); }

private:
    Buffer m_buffer;
};

} // namespace Basalt
