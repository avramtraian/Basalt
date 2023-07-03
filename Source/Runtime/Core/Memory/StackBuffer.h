// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Buffer.h"

namespace Basalt
{

/**
 * 
 */
struct BASALT_API StackBuffer
{
public:
    BT_NON_COPYABLE(StackBuffer);
    BT_NON_MOVABLE(StackBuffer);

public:
    static bool Copy(StackBuffer& destination, const StackBuffer& source)
    {
        // The destination buffer must be empty.
        Check(destination.m_offset == 0);

        if (destination.m_buffer.size < source.m_offset)
        {
            return false;
        }

        Memory::Copy(destination.Data(), source.Data(), source.m_offset);
        destination.m_offset = source.m_offset;
        return true;
    }

public:
    FORCEINLINE StackBuffer()
        : m_offset(0)
    {}

    FORCEINLINE explicit StackBuffer(Usize in_size)
        : m_offset(0)
    {
        m_buffer.Invalidate(in_size);
    }

    FORCEINLINE ~StackBuffer()
    {
        m_buffer.Release();
    }

public:
    /** @return Pointer to the internal memory buffer. */
    FORCEINLINE U8* Data() { return m_buffer.data; }
    FORCEINLINE const U8* Data() const { return m_buffer.data; }

    /** @return The size (in bytes) of the internal memory buffer. */
    FORCEINLINE Usize Size() const { return m_buffer.size; }

    /** @return The number of bytes that are currently occupied. Subsequent pushes will increment this value. */
    FORCEINLINE Usize Offset() const { return m_offset; }

    /** @return The number of bytes that are currently not occupied. Same as `Size() - Offset()`. */
    FORCEINLINE Usize Available() const { return m_buffer.size - m_offset; }

    /** @return True if there are no bytes occupied; False otherwise. */
    FORCEINLINE bool IsEmpty() const { return (m_offset == 0); }

public:
    /**
     * Pushes a number of raw bytes to the buffer.
     * If the internal memory buffer is not sufficient to store them, this function
     * will return nullptr.
     * 
     * @param bytes The bytes to copy to the buffer.
     * @param bytes_count The number of bytes to copy/push.
     * 
     * @return Address where the first pushed byte lives. If the buffer is not
     *         sufficient, nullptr will be returned.
     */
    U8* PushBytes(const void* bytes, Usize bytes_count)
    {
        if (m_offset + bytes_count < m_buffer.size)
        {
            // Buffer overflow.
            return nullptr;
        }

        U8* memory = m_buffer.data + m_offset;
        Memory::Copy(memory, bytes, bytes_count);
        m_offset += bytes_count;
        return memory;
    }

    /**
     * Pushes the raw bytes that form the given object instance to the buffer.
     * Wrapper around `PushBytes()`.
     * 
     * @tparam T The type of the object instance.
     * @param object_instance The object instance to push to the buffer.
     * 
     * @return Address where the first pushed byte lives. If the buffer is not
     *         sufficient to store the bytes, nullptr will be returned.
     */
    template<typename T>
    FORCEINLINE T* Push(const T& object_instance)
    {
        return PushBytes(&object_instance, sizeof(T));
    }

    /**
     * Pushes the raw bytes that form an array of the given type to the buffer.
     * Wrapper around `PushBytes()`.
     * 
     * @tparam T The type of the objects stored in the raw array.
     * @param elements Pointer to the first element in the array.
     * @param count The number of elements the array contains.
     * 
     * @return Address where the first pushed byte lives. If the buffer is not
     *         sufficient to store the bytes, nullptr will be returned.
     */
    template<typename T>
    FORCEINLINE T* PushArray(T* elements, Usize count)
    {
        return PushBytes(elements, count * sizeof(T));
    }

public:
    void PopBytes(Usize bytes_count)
    {
        // Trying to pop more bytes than the buffer currently stores.
        Check(bytes_count >= m_offset);
        m_offset -= bytes_count;
    }

    template<typename T>
    FORCEINLINE void Pop()
    {
        PopBytes(sizeof(T));
    }

    template<typename T>
    FORCEINLINE void PopArray(Usize count)
    {
        PopBytes(count * sizeof(T));
    }

public:
    void Reset()
    {
        m_offset = 0;
    }

    void Release()
    {
        m_buffer.Release();
        m_offset = 0;
    }

    void Invalidate(Usize bytes_count)
    {
        m_buffer.Invalidate(bytes_count);
        m_offset = 0;
    }

    void Resize(Usize new_size)
    {
        // The new size must be at least sufficient to store the currently stored data.
        Check(new_size >= m_offset);

        if (new_size == m_buffer.size)
        {
            return;
        }

        Buffer new_buffer = Buffer(new_size);
        Memory::Copy(new_buffer.data, m_buffer.data, m_offset);

        m_buffer.Release();
        m_buffer = new_buffer;
    }

private:
    Buffer m_buffer;
    Usize m_offset;
};

} // namespace Basalt
