// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/CoreDefines.h"
#include "Core/CoreTypes.h"
#include "Core/Misc/AssertionMacros.h"

namespace Basalt
{

/**
 * Container that manages an optional contained value, meaning that the value
 * may or may not be present. A common use case is the return value of a function
 * that may fail.
 */
template<typename T>
class BASALT_S_API Optional
{
public:
    /** Default constructor. The optional doesn't contain a value. */
    FORCEINLINE Optional()
        : m_has_value(false)
    {}

    /**
     * Copy constructor. If `other` has a value stored in it, the managed
     * object will be copied to this optional, making it valid.
     * The instance is constructed via its copy constructor.
     * 
     * @param other The optional to copy.
     */
    FORCEINLINE Optional(const Optional& other)
        : m_has_value(other.m_has_value)
    {
        if (m_has_value)
        {
            new (m_instance) T(other.GetRaw());
        }
    }

    /**
     * Move constructor. If `other` has a value stored in it, the managed
     * object will be copied to this optional, making it valid. After the
     * move operation, `other`'s instance will be released.
     * The instance is constructed via its move constructor.
     * 
     * @param other The optional to copy.
     */
    FORCEINLINE Optional(Optional&& other) noexcept
        : m_has_value(other.m_has_value)
    {
        if (m_has_value)
        {
            new (m_instance) T(Move(other.GetRaw()));
            other.GetRaw().~T();
            other.m_has_value = false;
        }
    }

    /**
     * Constructor that initializes the optional with a valid value.
     * The instance is constructed via its copy constructor.
     * 
     * @param instance The raw object to initialize the optional with.
     */
    FORCEINLINE Optional(const T& instance)
        : m_has_value(true)
    {
        new (m_instance) T(instance);
    }

    /**
     * Move semantics version of the above.
     */
    FORCEINLINE Optional(T&& instance)
        : m_has_value(true)
    {
        new (m_instance) T(Move(instance));
    }

    /** Destructor. If the optional contains a value, it will be destroyed. */
    FORCEINLINE ~Optional()
    {
        Release();
    }

    /**
     * Copy assignment operator.
     * If this optional currently contains a value, it will be released.
     * The instance will be constructed using its copy constructor.
     * 
     * @param other The optional to copy.
     * @return Reference to this after the copy operation.
     */
    FORCEINLINE Optional& operator=(const Optional& other)
    {
        Release();

        if (other.m_has_value)
        {
            new (m_instance) T(other.GetRaw());
            m_has_value = true;
        }

        return *this;
    }
    
    /**
     * Move assignment operator.
     * If this optional currently contains a value, it will be released.
     * The instance will be constructed using its move constructor.
     *
     * @param other The optional to move.
     * @return Reference to this after the move operation.
     */
    FORCEINLINE Optional& operator=(Optional&& other) noexcept
    {
        Release();

        if (other.m_has_value)
        {
            new (m_instance) T(Move(other.GetRaw()));
            m_has_value = true;
            other.GetRaw().~T();
            other.m_has_value = false;
        }

        return *this;
    }

public:
    /** @return True if the optional contains a value; False otherwise. */
    FORCEINLINE bool HasValue() const { return m_has_value; }

    /** @return True if the optional contains a value; False otherwise. */
    FORCEINLINE operator bool() const { return HasValue(); }

    /** @return Reference to the stored object. If the optional is empty, an assert will be issued. */
    FORCEINLINE T& Get() { Check(m_has_value); return GetRaw(); }
    FORCEINLINE const T& Get() const { Check(m_has_value); return GetRaw(); }

    /** @return A pointer to the stored object. If the optional is empty, nullptr will be returned. */
    FORCEINLINE T* GetPtr() { return m_has_value ? GetPtrRaw() : nullptr; }
    FORCEINLINE const T* GetPtr() const { return m_has_value ? GetPtrRaw() : nullptr; }

    /** @return A pointer to the stored object. If the optional is empty, nullptr will be returned. */
    FORCEINLINE T* operator->() { return GetPtr(); }
    FORCEINLINE const T* operator->() const { return GetPtr(); }

    /** @return Reference to the stored object. If the optional is empty, an assert will be issued. */
    FORCEINLINE T& operator*() { return Get(); }
    FORCEINLINE const T& operator*() const { return Get(); }

    /**
     * Releases the value stored in this optional.
     * If the optional doesn't contain a value, nothing will be executed.
     */
    FORCEINLINE void Release()
    {
        if (m_has_value)
        {
            GetRaw().~T();
            m_has_value = false;
        }
    }

    /**
     * @param errorValue The value to return if the optional is empty.
     * @return The value stored in this optional if any, or `errorValue` otherwise.
     */
    FORCEINLINE T& ValueOr(T& error_value) { return m_has_value ? Get() : error_value; }
    FORCEINLINE const T& ValueOr(const T& error_value) const { return m_has_value ? Get() : error_value; }

public:
    /** @return A pointer to the stored object. No check that it actually exists will be performed. */
    FORCEINLINE T* GetPtrRaw() { return (T*)m_instance; }
    FORCEINLINE const T* GetPtrRaw() const { return (const T*)m_instance; }

    /** @return A pointer to the stored object. No check that it actually exists will be performed. */
    FORCEINLINE T& GetRaw() { return *GetPtrRaw(); }
    FORCEINLINE const T& GetRaw() const { return *GetPtrRaw(); }

public:
    /**
     * Equality check operator.
     * Two optionals that don't have a value stored in them are always not equal.
     * 
     * @param other The optional to compare against.
     * @return True if the optional are equivalent; False otheriwse.
     */
    FORCEINLINE bool operator==(const Optional& other) const
    {
        if (m_has_value && other.m_has_value)
        {
            return (GetRaw() == other.GetRaw());
        }

        return false;
    }

    /**
     * Inequality check operator.
     * Two optionals that don't have a value stored in them are always not equal.
     *
     * @param other The optional to compare against.
     * @return True if the optional are not equivalent; False otheriwse.
     */
    FORCEINLINE bool operator!=(const Optional& other) const
    {
        return !(*this == other);
    }

private:
    /** The memory buffer where the object is stored. If the optional is empty, this contains invalid data. */
    alignas(T) U8 m_instance[sizeof(T)];

    /** Whether or not the optional contains a value. */
    bool m_has_value;
};

} // namespace Basalt
