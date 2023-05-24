// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/CoreDefines.h"
#include "Core/CoreTypes.h"
#include "Core/AssertionMacros.h"
#include "Core/Memory/Memory.h"

namespace Basalt
{

/**
 * Container that stores the elements contiguous in memory.
 * Provides dynamically growing and shrinking capabilities, base on the
 * number of elements that are currently stored in the array.
 * 
 * @tparam ElemenType The type of the elements that are stored in the container.
 */
template<typename ElementType>
class BASALT_S_API Array
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
    FORCEINLINE Array()
        : m_elements(nullptr)
        , m_capacity(0)
        , m_count(0)
    {}

    /**
     * Copy constructor. It will allocate memory only if `other` is
     * not an empty array. The elements are constructed using their
     * copy constructor.
     * 
     * @param other The array to copy.
     */
    FORCEINLINE Array(const Array& other)
        : m_count(other.m_count)
    {
        if (m_count == 0)
        {
            m_elements = nullptr;
            m_capacity = 0;
            return;
        }

        m_capacity = m_count;
        m_elements = AllocateMemory(m_capacity);
        CopyElements(m_elements, other.m_elements, m_count);
    }

    /**
     * Move constructor. It doesn't allocate any memory, as the only
     * operation performed is swapping the control member variables.
     * 
     * @param other The array to move. After this operation, it will
     *              become an empty array.
     */
    FORCEINLINE Array(Array&& other) noexcept
        : m_elements(other.m_elements)
        , m_capacity(other.m_capacity)
        , m_count(other.m_count)
    {
        other.m_elements = nullptr;
        other.m_capacity = 0;
        other.m_count = 0;
    }

    /** Destructor. Clears the array and releases the memory. */
    FORCEINLINE ~Array()
    {
        Clear();
        ReleaseMemory(m_elements, m_capacity);
    }

    /**
     * Copy assignment operator.
     * 
     * @param other The array to copy.
     * @return Reference to this after the copy operation.
     */
    FORCEINLINE Array& operator=(const Array& other)
    {
        Clear();
        
        if (other.m_count > m_capacity)
        {
            ReAllocate(NextCapacity(other.m_count));
        }

        m_count = other.m_count;
        CopyElements(m_elements, other.m_elements, m_count);
        return *this;
    }

    /**
     * Move assignment operator.
     * Doesn't allocate any memory, as the arrays will just swap pointers.
     * 
     * @param other The array to move.
     * @return Reference to this after the move operation.
     */
    FORCEINLINE Array& operator=(Array&& other) noexcept
    {
        Clear();

        ElementType* elements = m_elements;
        const Usize capacity = m_capacity;

        m_elements = other.m_elements;
        m_capacity = other.m_capacity;
        m_count = other.m_count;

        other.m_elements = elements;
        other.m_capacity = capacity;
        other.m_count = 0;

        return *this;
    }

public:
    /** @return A pointer to the buffer where the elements are stored. */
    FORCEINLINE ElementType* Data() { return m_elements; }
    FORCEINLINE const ElementType* Data() const { return m_elements; }

    /** @return The number of elements that are stored in the array. */
    FORCEINLINE Usize Count() const { return m_count; }

    /** @return The number of elements that the array can store without reallocating. */
    FORCEINLINE Usize Capacity() const { return m_capacity; }
    
    /** @return True if there are no elements stored in the array; False otherwise. */
    FORCEINLINE bool IsEmpty() const { return (m_count == 0); }

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
        Check(index < m_count);
        return m_elements[index];
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
        Check(index < m_count);
        return m_elements[index];
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
        return m_elements[0];
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
        return m_elements[0];
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
        return m_elements[m_count - 1];
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
        return m_elements[m_count - 1];
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
        if (m_count >= m_capacity)
        {
            ReAllocate(NextCapacity(m_count + 1));
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
        Check(m_count > 0);
        m_elements[--m_count].~ElementType();
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

        for (Usize i = m_count - count; i < m_count; ++i)
        {
            m_elements[i].~ElementType();
        }

        m_count -= count;
    }

    /**
     * Clears the array, destroying all the elements stored in it.
     * It doesn't release the internal memory buffer, so `Capacity()` will remain unchanged.
     */
    FORCEINLINE void Clear()
    {
        for (Usize i = 0; i < m_count; ++i)
        {
            m_elements[i].~ElementType();
        }

        m_count = 0;
    }

public:
    /**
     * Shrinks the internal memory buffer in order to reduce memory usage.
     * The newly allocated block will have no unused capacity.
     */
    FORCEINLINE void ShrinkToFit()
    {
        if (m_count == m_capacity)
        {
            return;
        }

        if (m_count == 0)
        {
            ReleaseMemory(m_elements, m_capacity);
            m_elements = nullptr;
            m_capacity = 0;

            return;
        }

        ElementType* newElements = AllocateMemory(m_count);
        MoveElements(newElements, m_elements, m_count);
        ReleaseMemory(m_elements, m_capacity);

        m_elements = newElements;
        m_capacity = m_count;
    }

    /**
     * Sets the capacity of the internal memory buffer.
     * If `newCapacity` is less than `Count()`, an assert will be issued.
     * Unless `newCapacity` is equal to `Capacity()`, this function will allocate.
     * 
     * @param newCapacity The new capacity. Must be greater or equal than `Count()`.
     */
    FORCEINLINE void SetCapacity(Usize newCapacity)
    {
        Check(newCapacity > m_count);
        
        if (newCapacity == m_capacity)
        {
            return;
        }

        ReAllocate(newCapacity);
    }

    /**
     * Sets the number of elements stored in the array.
     * If the new count is greater than the current count, the newly created
     * elements are not initialized in any way.
     * 
     * @param newCount The new number of elements that are stored in the array.
     */
    FORCEINLINE void SetCountUninitialized(Usize newCount)
    {
        if (newCount > m_capacity)
        {
            ReAllocate(NextCapacity(newCount));
        }
        else
        {
            for (Usize i = newCount; i < m_count; ++i)
            {
                m_elements[i].~ElementType();
            }
        }

        m_count = newCount;
    }

    /**
     * Sets the number of elements stored in the array.
     * If the new count is greater than the current count, the newly created
     * elements are initialized using their copy constructor.
     *
     * @param newCount The new number of elements that are stored in the array.
     * @param element The value used to initialize newly added elements, if any.
     */
    FORCEINLINE void SetCount(Usize newCount, const ElementType& element)
    {
        const Usize oldCount = m_count;
        SetCountUninitialized(newCount);

        for (Usize i = oldCount; i < m_count; ++i)
        {
            new (m_elements + i) ElementType(element);
        }
    }

    /**
     * Sets the number of elements stored in the array.
     * If the new count is greater than the current count, the newly created
     * elements are initialized using their default constructor.
     *
     * @param newCount The new number of elements that are stored in the array.
     * @param element The value used to initialize newly added elements, if any.
     */
    FORCEINLINE void SetCountDefaulted(Usize newCount)
    {
        const Usize oldCount = m_count;
        SetCountUninitialized(newCount);

        for (Usize i = oldCount; i < m_count; ++i)
        {
            new (m_elements + i) ElementType();
        }
    }

    /**
     * Sets the number of elements stored in the array.
     * If the new count is greater than the current count, the newly created
     * elements are initialized by setting their memory to 0.
     *
     * @param newCount The new number of elements that are stored in the array.
     * @param element The value used to initialize newly added elements, if any.
     */
    FORCEINLINE void SetCountZeroed(Usize newCount)
    {
        const Usize oldCount = m_count;
        SetCountUninitialized(newCount);
        
        if (m_count > oldCount)
        {
            Memory::Zero(m_elements + oldCount, (m_count - oldCount) * sizeof(ElementType));
        }
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

    FORCEINLINE Usize NextCapacity(Usize requiredCount) const
    {
        // The capacity is given by a geometric series. This growth rate
        // provides a good balance between memory efficiency and performance.
        const Usize geometric = m_capacity + m_capacity / 2;

        if (requiredCount > geometric)
        {
            // Geometric growth is not sufficient.
            return requiredCount;
        }

        // Geometric growth is sufficient.
        return geometric;
    }

    FORCEINLINE void ReAllocate(Usize newCapacity)
    {
        Check(newCapacity > m_capacity);

        ElementType* newElements = AllocateMemory(newCapacity);
        MoveElements(newElements, m_elements, m_count);
        ReleaseMemory(m_elements, m_capacity);

        m_elements = newElements;
        m_capacity = newCapacity;
    }

private:
    /** Pointer to the buffer where the elements are stored. */
    ElementType* m_elements;

    /** The number of elements that the array can store without reallocating. */
    Usize m_capacity;

    /** The number of elements that are currently stored in the container. */
    Usize m_count;
};

} // namespace Basalt
