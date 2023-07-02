// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/CoreDefines.h"
#include "Core/CoreTypes.h"

namespace Basalt
{

/**
 * Container that refers to a contiguous sequence of objects.
 */
template<typename ElementType>
class BASALT_S_API ArrayView
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
    /** Default constructor. */
    FORCEINLINE constexpr ArrayView()
        : m_elements(nullptr)
        , m_count(0)
    {}

    /**
     * Copy constructor.
     * 
     * @param other The array view to copy.
     */
    FORCEINLINE constexpr ArrayView(const ArrayView& other)
        : m_elements(other.m_elements)
        , m_count(other.m_count)
    {}

    /**
     * Move constructor. The array view to move will be left in an invalidated state.
     *
     * @param other The array view to move.
     */
    FORCEINLINE constexpr ArrayView(ArrayView&& other) noexcept
        : m_elements(other.m_elements)
        , m_count(other.m_count)
    {
        other.m_elements = nullptr;
        other.m_count = 0;
    }

    /**
     * Constructor that takes a raw array.
     * 
     * @param elements Pointer to the elements where the elements are stored.
     * @param count The number of elements stored in the raw array.
     */
    FORCEINLINE constexpr ArrayView(ElementType* elements, Usize count)
        : m_elements(elements)
        , m_count(count)
    {}

    /**
     * Copy assignment operator.
     * 
     * @param other The array view to copy.
     * @return Reference to this, after the copy operation.
     */
    FORCEINLINE ArrayView& operator=(const ArrayView& other)
    {
        m_elements = other.m_elements;
        m_count = other.m_count;

        return *this;
    }

    /**
     * Move assignment operator. The provided array view will be left in an invalidated state.
     *
     * @param other The array view to move.
     * @return Reference to this, after the move operation.
     */
    FORCEINLINE ArrayView& operator=(ArrayView&& other) noexcept
    {
        m_elements = other.m_elements;
        m_count = other.m_count;

        other.m_elements = nullptr;
        other.m_count = 0;

        return *this;
    }

public:
    /** @return A pointer to the buffer where the elements are stored. */
    FORCEINLINE ElementType* Data() { return m_elements; }
    FORCEINLINE const ElementType* Data() const { return m_elements; }

    /** @return A pointer to the buffer where the elements are stored. */
    FORCEINLINE ElementType* operator*() { return Data(); }
    FORCEINLINE const ElementType* operator*() const { return Data(); }

    /** @return The number of elements that are stored in the array view. */
    FORCEINLINE Usize Count() const { return m_count; }

    /** @return True if there are no elements stored in the sparray view; False otherwise. */
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
    * Gets the first element stored in the array view.
    * If the array view is empty, an assert will be issued.
    *
    * @return Reference to the first element in the array view.
    */
    FORCEINLINE ElementType& Front(Usize index)
    {
        Check(m_count > 0);
        return m_elements[0];
    }

    /**
     * Gets the first element stored in the array view.
     * If the array view is empty, an assert will be issued.
     *
     * @return Const reference to the first element in the array view.
     */
    FORCEINLINE const ElementType& Front(Usize index) const
    {
        Check(m_count > 0);
        return m_elements[0];
    }

    /**
     * Gets the last element stored in the array view.
     * If the array view is empty, an assert will be issued.
     *
     * @return Reference to the last element in the array view.
     */
    FORCEINLINE ElementType& Back(Usize index)
    {
        Check(m_count > 0);
        return m_elements[m_count - 1];
    }

    /**
     * Gets the last element stored in the array view.
     * If the array view is empty, an assert will be issued.
     *
     * @return Const reference to the last element in the array view.
     */
    FORCEINLINE const ElementType& Back(Usize index) const
    {
        Check(m_count > 0);
        return m_elements[m_count - 1];
    }

public:
    /** @return A forward iterator that points to the first element in the array view. */
    FORCEINLINE Iterator begin() { return Iterator(m_elements); }
    FORCEINLINE ConstIterator begin() const { return ConstIterator(m_elements); }

    /** @return A forward iterator that points to the slot after the last element in the array view. */
    FORCEINLINE Iterator end() { return Iterator(m_elements + m_count); }
    FORCEINLINE ConstIterator end() const { return ConstIterator(m_elements + m_count); }

    /** @return A reverse iterator that points to the last element in the array view. */
    FORCEINLINE ReverseIterator rbegin() { return ReverseIterator(m_elements + m_count - 1); }
    FORCEINLINE ReverseConstIterator rbegin() const { return ReverseConstIterator(m_elements + m_count - 1); }

    /** @return A reverse iterator that points to the slot before the first element in the array view. */
    FORCEINLINE ReverseIterator rend() { return ReverseIterator(m_elements - 1); }
    FORCEINLINE ReverseConstIterator rend() const { return ReverseConstIterator(m_elements - 1); }

private:
    /** Pointer to the buffer where the elements are stored. */
    ElementType* m_elements;

    /** The number of elements that are stored in the array view. */
    Usize m_count;
};

} // namespace Basalt
