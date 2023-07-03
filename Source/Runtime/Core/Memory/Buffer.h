// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/Math/MathUtilities.h"
#include "Core/Misc/AssertionMacros.h"
#include "Memory.h"

namespace Basalt
{

class BASALT_API Buffer
{
public:
    FORCEINLINE static Buffer Copy(Buffer source_buffer)
    {
        Buffer result;
        result.Invalidate(source_buffer.size);
        Memory::Copy(result.data, source_buffer.data, source_buffer.size);
        return result;
    }

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
    template<typename T>
    FORCEINLINE T* As() const { return (T*)data; }

    template<typename T>
    FORCEINLINE Usize CountOf() const { return (size / sizeof(T)); }

    /** @return Pointer to the buffer memory, expressed as an UTF-8 encoded string. */
    FORCEINLINE char* AsUTF8() { return As<char>(); }
    FORCEINLINE const char* AsUTF8() const { return As<const char>(); }

    /** @return Pointer to the buffer memory, expressed as an UTF-16 encoded string. */
    FORCEINLINE wchar_t* AsUTF16() { return As<wchar_t>(); }
    FORCEINLINE const wchar_t* AsUTF16() const { return As<const wchar_t>(); }

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
    BT_NON_COPYABLE(ScopedBuffer);

public:
    ScopedBuffer() = default;

    FORCEINLINE ScopedBuffer(ScopedBuffer&& other) noexcept
        : m_buffer(other.m_buffer)
    {
        other.m_buffer.data = nullptr;
        other.m_buffer.size = 0;
    }

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

    FORCEINLINE ScopedBuffer& operator=(ScopedBuffer&& other) noexcept
    {
        Release();
        m_buffer = other.m_buffer;
        
        other.m_buffer.data = nullptr;
        other.m_buffer.size = 0;

        return *this;
    }

public:
    FORCEINLINE U8* Data() const { return m_buffer.data; }

    FORCEINLINE Usize Size() const { return m_buffer.size; }

    FORCEINLINE Buffer& RawBuffer() { return m_buffer; }
    FORCEINLINE const Buffer& RawBuffer() const { return m_buffer; }

public:
    template<typename T>
    FORCEINLINE T* As() const { return m_buffer.As<T>(); }

    template<typename T>
    FORCEINLINE Usize CountOf() const { return m_buffer.CountOf<T>(); }

    /** @return Pointer to the buffer memory, expressed as an UTF-8 encoded string. */
    FORCEINLINE char* AsUTF8() { return As<char>(); }
    FORCEINLINE const char* AsUTF8() const { return As<const char>(); }

    /** @return Pointer to the buffer memory, expressed as an UTF-16 encoded string. */
    FORCEINLINE wchar_t* AsUTF16() { return As<wchar_t>(); }
    FORCEINLINE const wchar_t* AsUTF16() const { return As<const wchar_t>(); }

public:
    FORCEINLINE void Invalidate(Usize in_size)
    {
        m_buffer.Invalidate(in_size);
    }

    FORCEINLINE void Release()
    {
        m_buffer.Release();
    }

    FORCEINLINE void Resize(Usize new_size)
    {
        m_buffer.Resize(new_size);
    }

private:
    Buffer m_buffer;
};

} // namespace Basalt
