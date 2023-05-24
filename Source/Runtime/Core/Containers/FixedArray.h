// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/CoreDefines.h"
#include "Core/CoreTypes.h"
#include "Core/AssertionMacros.h"

namespace Basalt
{

/**
 * Container that stores the elements contiguous in memory. The internal memory buffer
 * is static, being allocated on the stack. So, this array has no reallocation capabilities.
 * 
 * @tparam ElementType The type of elements stored in the array.
 * @tparam ElementsCount The number of elements that are stored in the array.
 */
template<typename ElementType, Usize ElementsCount>
class BASALT_S_API FixedArray
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
    FixedArray() = default;

    FixedArray(const FixedArray&) = default;
    FixedArray(FixedArray&&) noexcept = default;

    FixedArray& operator=(const FixedArray&) = default;
    FixedArray& operator=(FixedArray&&) noexcept = default;

public:
    /** @return A pointer to the buffer where the elements are stored. */
    FORCEINLINE ElementType* Data() { return &m_elements[0]; }
    FORCEINLINE const ElementType* Data() const { return &m_elements[0]; }

    /** @return The number of elements that are stored in the array. */
    FORCEINLINE static constexpr Usize Count() { return ElementsCount; }

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
        Check(index < Count());
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
        Check(index < Count());
        return m_elements[index];
    }

    /** Wrappers around `At()`. */
    FORCEINLINE ElementType& operator[](Usize index) { return At(index); }
    FORCEINLINE const ElementType& operator[](Usize index) const { return At(index); }

    /** @return The first element stored in the array. */
    FORCEINLINE ElementType& Front() { return m_elements[0]; }
    FORCEINLINE const ElementType& Front() const { return m_elements[0]; }

    /** @return The last element stored in the array. */
    FORCEINLINE ElementType& Back() { return m_elements[Count() - 1]; }
    FORCEINLINE const ElementType& Back() const { return m_elements[Count() - 1]; }

public:
    /** @return A forward iterator that points to the first element in the array. */
    FORCEINLINE Iterator begin() { return Iterator(Data()); }
    FORCEINLINE ConstIterator begin() const { return ConstIterator(Data()); }

    /** @return A forward iterator that points to the slot after the last element in the array. */
    FORCEINLINE Iterator end() { return Iterator(Data() + m_count); }
    FORCEINLINE ConstIterator end() const { return ConstIterator(Data() + Count()); }

    /** @return A reverse iterator that points to the last element in the array. */
    FORCEINLINE ReverseIterator rbegin() { return ReverseIterator(Data() + Count() - 1); }
    FORCEINLINE ReverseConstIterator rbegin() const { return ReverseConstIterator(Data() + Count() - 1); }

    /** @return A reverse iterator that points to the slot before the first element in the array. */
    FORCEINLINE ReverseIterator rend() { return ReverseIterator(Data() - 1); }
    FORCEINLINE ReverseConstIterator rend() const { return ReverseConstIterator(Data() - 1); }


private:
    /** The buffer where the elements are stored. */
    ElementType m_elements[ElementsCount];
};

} // namespace Basalt
