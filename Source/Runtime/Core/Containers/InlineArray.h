// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/CoreDefines.h"
#include "Core/CoreTypes.h"
#include "Core/Misc/AssertionMacros.h"

#include <initializer_list>

namespace Basalt
{

/**
 * Container that stores the elements contiguous in memory. The elements
 * are stored either inline with the array class itself, either on a
 * heap allocated memory buffer. The placement decision is made at runtime,
 * depending on the number of elements stored.
 * This container is ideal when you have a small number of elements 90%
 * of the time, but there are cases when more space is required.
 * 
 * @tparam ElementType The type of elements stored in the array.
 * @tparam InlineCapacity The number of elements that the inline
 *                        buffer can store.
 */
template<typename ElementType, Usize InlineCapacity>
class BASALT_S_API InlineArray
{
public:
    /**
     * Iterator typedefs.
     * Because the elements are stored contiguous in memory, a simple
     * pointer has all the capabilities and requirements of an iterator.
     */
    using Iterator              = ElementType*;
    using ConstIterator         = const ElementType*;
    using ReverseIterator       = ElementType*;
    using ReverseConstIterator  = const ElementType*;

public:
    /** Default constructor. It doesn't allocate any memory. */
    FORCEINLINE InlineArray()
        : m_count(0)
    {}

    template<Usize OtherInlineCapacity>
    FORCEINLINE InlineArray(const InlineArray<ElementType, OtherInlineCapacity>& other)
        : m_count(other.m_count)
    {
        ElementType* destinationBuffer;

        if (m_count <= InlineCapacity)
        {
            destinationBuffer = m_inlineElements;
        }
        else
        {
            m_heapCapacity = m_count;
            m_heapElements = AllocateMemory(m_heapCapacity);
            destinationBuffer = m_heapElements;
        }

        CopyElements(destinationBuffer, other.Data(), m_count);
    }

    template<Usize OtherInlineCapacity>
    FORCEINLINE InlineArray(InlineArray<ElementType, OtherInlineCapacity>&& other) noexcept
        : m_count(other.m_count)
    {
        if (IsInlined())
        {
            if (other.IsInlined())
            {
                MoveElements(m_inlineElements, other.m_inlineElements, m_count);
            }
            else
            {
                MoveElements(m_inlineElements, other.m_heapElements, m_count);
                ReleaseMemory(other.m_heapElements, other.m_heapCapacity);
            }
        }
        else
        {
            if (other.IsInlined())
            {
                m_heapCapacity = m_count;
                m_heapElements = AllocateMemory(m_heapCapacity);
                MoveElements(m_heapElements, other.m_inlineElements, m_count);
            }
            else
            {
                m_heapElements = other.m_heapElements;
                m_heapCapacity = other.m_heapCapacity;

                other.m_heapElements = nullptr;
                other.m_heapCapacity = 0;
            }
        }

        other.m_count = 0;
    }

    FORCEINLINE InlineArray(std::initializer_list<ElementType> init_list)
        : m_count(init_list.size())
    {
        ElementType* destinationBuffer = m_inlineElements;

        if (m_count > InlineCapacity)
        {
            m_heapCapacity = m_count;
            m_heapElements = AllocateMemory(m_heapCapacity);
            destinationBuffer = m_heapElements;
        }

        CopyElements(destinationBuffer, init_list.begin(), m_count);
    }

    /**
     * Destructor. The elements are destroyed and if the internal buffer is allocated
     * on the heap, it will be released from memory.
     */
    FORCEINLINE ~InlineArray()
    {
        if (m_count <= InlineCapacity)
        {
            for (Usize i = 0; i < m_count; ++i)
            {
                m_inlineElements[i].~ElementType();
            }
        }
        else
        {
            for (Usize i = 0; i < m_count; ++i)
            {
                m_heapElements[i].~ElementType();
            }

            ReleaseMemory(m_heapElements, m_heapCapacity);
        }
    }

    template<Usize OtherInlineCapacity>
    FORCEINLINE InlineArray& operator=(const InlineArray<ElementType, OtherInlineCapacity>& other)
    {
        const Usize currentCount = m_count;
        Clear();
        ElementType* destinationBuffer;
        
        if (currentCount <= InlineCapacity)
        {
            if (other.m_count <= InlineCapacity)
            {
                destinationBuffer = m_inlineElements;
            }
            else
            {
                m_heapCapacity = other.m_count;
                m_heapElements = AllocateMemory(m_heapCapacity);
                destinationBuffer = m_heapElements;
            }
        }
        else
        {
            if (other.m_count <= InlineCapacity)
            {
                ReleaseMemory(m_heapElements, m_heapCapacity);
                destinationBuffer = m_inlineElements;
            }
            else
            {
                if (other.m_count > m_heapCapacity)
                {
                    ReAllocate(NextCapacity(other.m_count));
                }
                destinationBuffer = m_heapElements;
            }
        }

        m_count = other.m_count;
        CopyElements(destinationBuffer, other.Data(), m_count);

        return *this;
    }

    template<Usize OtherInlineCapacity>
    FORCEINLINE InlineArray& operator=(InlineArray<ElementType, OtherInlineCapacity>&& other) noexcept
    {
        const Usize currentCount = m_count;
        Clear();

        if (other.m_count <= InlineCapacity)
        {
            // Elements should end up on the inline buffer.

            if (currentCount > InlineCapacity)
            {
                ReleaseMemory(m_heapElements, m_heapCapacity);
            }
            
            if (other.IsInlined())
            {
                MoveElements(m_inlineElements, other.m_inlineElements, other.m_count);
            }
            else
            {
                MoveElements(m_inlineElements, other.m_heapElements, other.m_count);

                ReleaseMemory(other.m_heapElements, other.m_heapCapacity);
                other.m_heapElements = nullptr;
                other.m_heapCapacity = 0;
            }
        }
        else
        {
            // Elements should end up on the heap buffer.

            if (other.IsInlined())
            {
                if (currentCount <= InlineCapacity)
                {
                    m_heapCapacity = other.m_count;
                    m_heapElements = AllocateMemory(m_heapCapacity);
                }
                else if (other.m_count > m_heapCapacity)
                {
                    ReAllocate(NextCapacity(other.m_count));
                }

                MoveElements(m_heapElements, other.m_inlineElements, other.m_count);
            }
            else
            {
                if (currentCount >= InlineCapacity)
                {
                    ReleaseMemory(m_heapElements, m_heapCapacity);
                }

                m_heapElements = other.m_heapElements;
                m_heapCapacity = other.m_heapCapacity;

                other.m_heapElements = nullptr;
                other.m_heapCapacity = 0;
            }
        }

        m_count = other.m_count;
        other.m_count = 0;
        return *this;
    }

public:
    /** @return A pointer to the buffer where the elements are stored. */
    FORCEINLINE ElementType* Data() { return (m_count <= InlineCapacity) ? m_inlineElements : m_heapElements; }
    FORCEINLINE const ElementType* Data() const { return (m_count <= InlineCapacity) ? m_inlineElements : m_heapElements; }

    /** @return A pointer to the buffer where the elements are stored. */
    FORCEINLINE ElementType* operator*() { return Data(); }
    FORCEINLINE const ElementType* operator*() const { return Data(); }

    /** @return The number of elements that are stored in the array. */ 
    FORCEINLINE Usize Count() const { return m_count; }

    /** @return The number of elements that the array can store without reallocating. */
    FORCEINLINE Usize Capacity() const { return (m_count <= InlineCapacity) ? InlineCapacity : m_heapCapacity; }

    /** @return True if there are no elements stored in the array; False otherwise. */
    FORCEINLINE bool IsEmpty() const { return (m_count == 0); }

    /** @return True if the elements are stored inline; False otherwise. */
    FORCEINLINE bool IsInlined() const { return (m_count <= InlineCapacity); }

public:
    /*
     * Gets the element stored at the given index.
     * If the index is out of range, an assert will be issued.
     *
     * @param index The index where the element is stored.
     * @return Reference to the element stored at the given index.
     */
    FORCEINLINE ElementType& At(Usize index)
    {
        Check(index < m_count); // Index out of range.
        return Data()[index];
    }

    /*
     * Gets the element stored at the given index.
     * If the index is out of range, an assert will be issued.
     * Const version of the above.
     *
     * @param index The index where the element is stored.
     * @return Const reference to the element stored at the given index.
     */
    FORCEINLINE const ElementType& At(Usize index) const
    {
        Check(index < m_count); // Index out of range.
        return Data()[index];
    }

    /** Wrappers around `At()`. */
    FORCEINLINE ElementType& operator[](Usize index) { return At(index); }
    FORCEINLINE const ElementType& operator[](Usize index) const { return At(index); }

    /**
     * Gets the first element stored in the array.
     * If the array is empty, an assert will be issued.
     *
     * @return Reference to the first element in the array.
     */
    FORCEINLINE ElementType& Front()
    {
        Check(m_count > 0);
        return Data()[0];
    }

    /**
     * Gets the first element stored in the array.
     * If the array is empty, an assert will be issued.
     *
     * @return Const reference to the first element in the array.
     */
    FORCEINLINE const ElementType& Front() const
    {
        Check(m_count > 0);
        return Data()[0];
    }

    /**
     * Gets the last element stored in the array.
     * If the array is empty, an assert will be issued.
     *
     * @return Reference to the last element in the array.
     */
    FORCEINLINE ElementType& Back()
    {
        Check(m_count > 0);
        return Data()[m_count - 1];
    }

    /**
     * Gets the last element stored in the array.
     * If the array is empty, an assert will be issued.
     *
     * @return Const reference to the last element in the array.
     */
    FORCEINLINE const ElementType& Back() const
    {
        Check(m_count > 0);
        return Data()[m_count - 1];
    }

public:
    /**
     * Adds an uninitialized element to the array. It will be placed at the
     * end of the array.
     * A memory reallocation might occur, if the internal buffer is not
     * sufficient to store the new element.
     *
     * @return Reference to the newly added element.
     */
    FORCEINLINE ElementType& AddUninitialized()
    {
        if (m_count == InlineCapacity)
        {
            const Usize heapCapacity = m_count + 1;
            ElementType* heapElements = AllocateMemory(heapCapacity);
            MoveElements(heapElements, m_inlineElements, m_count);

            m_heapElements = heapElements;
            m_heapCapacity = heapCapacity;
        }
        else if (!IsInlined())
        {
            if (m_count >= m_heapCapacity)
            {
                ReAllocate(NextCapacity(m_count + 1));
            }
        }

        ++m_count;
    }

    /**
     * Adds an element to the array. It will be placed
     * at the end of the array.
     * The element is constructed in-place, using its copy constructor.
     * A memory reallocation might occur, if the internal buffer is not
     * sufficient to store the new element.
     *
     * @param newElement The element to copy in-place.
     * @return Reference to the newly added element.
     */
    FORCEINLINE ElementType& Add(const ElementType& newElement)
    {
        ElementType& slot = AddUninitialized();
        new (&slot) ElementType(newElement);
        return slot;
    }

    /**
     * Adds an element to the array. It will be placed at the end of the array.
     * The element is constructed in-place, using its move constructor.
     * A memory reallocation might occur, if the internal buffer is not
     * sufficient to store the new element.
     *
     * @param newElement The element to move in-place.
     * @return Reference to the newly added element.
     */
    FORCEINLINE ElementType& Add(ElementType&& newElement)
    {
        ElementType& slot = AddUninitialized();
        new (&slot) ElementType(Move(newElement));
        return slot;
    }

    /**
     * Adds an element to the array. It will be placed at the end of the array.
     * The element is constructed in-place, using its default constructor.
     * A memory reallocation might occur, if the internal buffer is not
     * sufficient to store the new element.
     *
     * @return Reference to the newly added element.
     */
    FORCEINLINE ElementType& AddDefaulted()
    {
        ElementType& slot = AddUninitialized();
        new (&slot) ElementType();
        return slot;
    }

    /**
     * Adds a new element to the array. It will be placed at the end of the array.
     * The element is initialized by setting its memory to 0. No constructor will
     * be invoked.
     * A memory reallocation might occur, if the internal buffer is not
     * sufficient to store the new element.
     *
     * @return Reference to the newly added element.
     */
    FORCEINLINE ElementType& AddZeroed()
    {
        ElementType& slot = AddUninitialized();
        Memory::Zero(&slot, sizeof(ElementType));
        return slot;
    }

public:
    /**
     * Removes the last element from the array.
     * If the array is empty, an assert will be issued.
     */
    FORCEINLINE void PopBack()
    {
        Check(m_count > 0); // An empty array can't be popped.

        if (m_count == (InlineCapacity + 1))
        {
            ElementType* elements = m_heapElements;
            const Usize capacity = m_heapCapacity;

            MoveElements(m_inlineElements, elements, m_count - 1);
            elements[--m_count].~ElementType();
            ReleaseMemory(elements, capacity);
        }
        else
        {
            Data()[--m_count].~ElementType();
        }
    }

    /**
     * Removes the last `count` elements from the array.
     * If the array doesn't have at least `count` elements, an assert will be issued.
     *
     * @param count The number of elements to pop from the array.
     */
    FORCEINLINE void PopBack(Usize count)
    {
        Check(m_count >= count);

        if (!IsInlined() && (m_count - count) <= InlineCapacity)
        {
            ElementType* elements = m_heapElements;
            const Usize capacity = m_heapCapacity;

            MoveElements(m_inlineElements, elements, m_count - count);
            for (Usize i = m_count - count; i < m_count; ++i)
            {
                elements[i].~ElementType();
            }
            m_count -= count;
            ReleaseMemory(elements, capacity);
        }
        else
        {
            ElementType* elements = Data();
            for (Usize i = m_count - count; i < m_count; ++i)
            {
                elements[i].~ElementType();
            }

            m_count -= count;
        }
    }

    /**
     * Clears the array, destroying all the elements stored in it.
     * If the elements are stored on the heap, the internal memory buffer will be released.
     */
    FORCEINLINE void Clear()
    {
        ElementType* elements = Data();
        for (Usize i = 0; i < m_count; ++i)
        {
            elements[i].~ElementType();
        }

        if (!IsInlined())
        {
            ReleaseMemory(m_heapElements, m_heapCapacity);
        }

        m_count = 0;
    }

public:
    /** @return A forward iterator that points to the first element in the array. */
    FORCEINLINE Iterator begin() { return Iterator(m_elements); }
    FORCEINLINE ConstIterator begin() const { return ConstIterator(m_elements); }

    /** @return A forward iterator that points to the slot after the last element in the array. */
    FORCEINLINE Iterator end() { return Iterator(m_elements + m_count); }
    FORCEINLINE ConstIterator end() const { return ConstIterator(m_elements + m_count); }

    /** @return A reverse iterator that points to the last element in the array. */
    FORCEINLINE ReverseIterator rbegin() { return ReverseIterator(m_elements + m_count - 1); }
    FORCEINLINE ReverseConstIterator rbegin() const { return ReverseConstIterator(m_elements + m_count - 1); }

    /** @return A reverse iterator that points to the slot before the first element in the array. */
    FORCEINLINE ReverseIterator rend() { return ReverseIterator(m_elements - 1); }
    FORCEINLINE ReverseConstIterator rend() const { return ReverseConstIterator(m_elements - 1); }


private:
    /**
     * Allocates a block of memory from the heap. The buffer will be
     * big enough to store exactly `capacity` elements.
     * All allocations must be done using this function, in order
     * to provide better memory debugging.
     *
     * @param capacity The capacity (in elements) of the memory block.
     * @return Pointer to the newly allocated memory buffer.
     */
    FORCEINLINE ElementType* AllocateMemory(Usize capacity)
    {
        return (ElementType*)Memory::AllocateTaggedI(capacity * sizeof(ElementType));
    }

    /**
     * Releases a memory block.
     * All deallocations must be done using this function, in order
     * to provide better memory debugging.
     *
     * @param elements Pointer to the memory buffer to release.
     * @param capacity The capacity (in elements) of the memory block.
     */
    FORCEINLINE void ReleaseMemory(ElementType* elements, Usize capacity)
    {
        (void)capacity;
        Memory::Free(elements);
    }

    /**
     * Copies the elements from one buffer to another.
     * The elements are constructed in the destination buffer using their copy constructor.
     * The destination buffer should not contain any valid data, as it will be overwritten.
     *
     * @param destination The buffer where the elements will be copied to.
     * @param source The buffer where the elements will be copied from.
     * @param count The number of elements to copy.
     */
    FORCEINLINE void CopyElements(ElementType* destination, const ElementType* source, Usize count)
    {
        for (Usize i = 0; i < count; ++i)
        {
            new (destination + i) ElementType(source[i]);
        }
    }

    /**
     * Moves the elements from one buffer to another.
     * The elements are constructed in the destination buffer using their move constructor.
     * After each movement operation, the element in the source buffer will be destroyed,
     * leaving the memory in an undefined state.
     * The destination buffer should not contain any valid data, as it will be overwritten.
     *
     * @param destination The buffer where the elements will be moved to.
     * @param source The buffer where the elements will be moved from.
     * @param count The number of elements to move.
     */
    FORCEINLINE void MoveElements(ElementType* destination, ElementType* source, Usize count)
    {
        for (Usize i = 0; i < count; ++i)
        {
            new (destination + i) ElementType(Move(source[i]));
            source[i].~ElementType();
        }
    }

private:
    union
    {
        struct
        {
            /**
             * Pointer to the memory block allocated on the heap where the
             * elements are stored.
             * If the inline buffer is used this variable contains invalid data.
             */
            ElementType* m_heapElements;

            /**
             * The number of elements that the heap buffer can store
             * without reallocating.
             * If the inline buffer is used this variable contains invalid data.
             */
            Usize m_heapCapacity;
        };

        /**
         * Inline buffer of elements.
         * This buffer contains invalid data if the heap buffer is
         * used instead.
         */
        ElementType m_inlineElements[InlineCapacity];
    };

    /** The number of elements currently stored in the array. */
    Usize m_count;

private:
    friend class InlineArray;
};

} // namespace Basalt
