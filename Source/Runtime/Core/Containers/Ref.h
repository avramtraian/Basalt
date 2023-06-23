// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/CoreDefines.h"
#include "Core/CoreTypes.h"
#include "Core/Misc/AssertionMacros.h"
#include "Core/Memory/Memory.h"

namespace Basalt
{

/**
 * Base class for all reference counted objects.
 * All types that are reference counted should inherit from this class.
 */
class BASALT_API RefCounted
{
public:
    virtual ~RefCounted() = default;

private:
    /** Increments the object's reference count. */
    void IncrementReferenceCount__() { ++m_reference_count__; }
    
    /**
     * Decrements the object's reference count.
     * @return True if the object's reference count after the decrement operations hits 0; False otherwise.
     */
    bool DecrementReferenceCount__() { return (m_reference_count__--) <= 1; }

private:
    /** The object's reference count. */
    U64 m_reference_count__ = 0;

    // The ref pointer is the single system that can mutate the object ref counter.
    template<typename T>
    friend class Ref;
};

/**
 * An intrusive reference-counted object pointer, that manages the lifetime of an object instance.
 * By default, this object pointer is thread-safe, as reference count increment and decrement
 * operations are performed using atomic calls.
 */
template<typename T>
class BASALT_S_API Ref
{
public:
    template<typename... Args>
    FORCEINLINE static Ref<T> Create(Args&&... args)
    {
        Ref<T> result;
        result.m_instance = btnew T(Forward<Args>(args)...);
        result.IncrementRefCount();
        return result;
    }

public:
    FORCEINLINE Ref()
        : m_instance(nullptr)
    {}

    FORCEINLINE Ref(const Ref& other)
        : m_instance(other.m_instance)
    {
        if (m_instance)
        {
            IncrementRefCount();
        }
    }

    FORCEINLINE Ref(Ref&& other) noexcept
        : m_instance(other.m_instance)
    {
        other.m_instance = nullptr;
    }

    FORCEINLINE explicit Ref(T* instance)
        : m_instance(instance)
    {
        if (m_instance)
        {
            IncrementRefCount();
        }
    }

    FORCEINLINE Ref(std::nullptr_t)
        : m_instance(nullptr)
    {}

    FORCEINLINE ~Ref()
    {
        Release();
    }

    FORCEINLINE Ref& operator=(const Ref& other)
    {
        Release();
        m_instance = other.m_instance;

        if (m_instance)
        {
            IncrementRefCount();
        }

        return *this;
    }

    FORCEINLINE Ref& operator=(Ref&& other) noexcept
    {
        Release();
        m_instance = other.m_instance;
        other.m_instance = nullptr;
        return *this;
    }

    FORCEINLINE Ref& operator=(std::nullptr_t)
    {
        Release();
        return *this;
    }

public:
    /** @return True if the ref pointer is valid (it stores an object instance) and can be used; False otherwise. */
    FORCEINLINE bool IsValid() const { return (m_instance != nullptr); }

    /** @return Pointer to the stored object instance. If the ref pointer is invalid, nullptr will be returned. */
    FORCEINLINE T* Raw() { return m_instance; }
    FORCEINLINE const T* Raw() const { return m_instance; }

    /** @return Pointer to the stored object instance. If the ref pointer is invalid, an assert will be issued. */
    FORCEINLINE T* Get()
    {
        Check(IsValid());
        return m_instance;
    }

    /** @return Const pointer to the stored object instance. If the ref pointer is invalid, an assert will be issued. */
    FORCEINLINE const T* Get() const
    {
        Check(IsValid());
        return m_instance;
    }

    /** @return Reference to the stored object instance. If the ref pointer is invalid, an assert will be issued. */
    FORCEINLINE T& GetRef() { return *Get(); }
    FORCEINLINE const T& GetRef() const { return *Get(); }

public:
    /** @return Pointer to the stored object instance. If the ref pointer is invalid, an assert will be issued. */
    FORCEINLINE T* operator->() { return Get(); }
    FORCEINLINE const T* operator->() const { return Get(); }

    /** @return Reference to the stored object instance. If the ref pointer is invalid, an assert will be issued. */
    FORCEINLINE T& operator*() { return GetRef(); }
    FORCEINLINE const T& operator*() const { return GetRef(); }

    /** @return True if the ref pointer is valid (it stores an object instance) and can be used; False otherwise. */
    FORCEINLINE operator bool() const { return IsValid(); }

    /**
     * Equality check operator.
     * This function compares the raw pointers, not the object instances.
     * 
     * @param other The ref pointer to compare against.
     * @return True if the ref pointers store the same object instance; False otherwise.
     */
    FORCEINLINE bool operator==(const Ref& other) const { return (m_instance == other.m_instance); }

    /**
     * Inequality check operator.
     * This function compares the raw pointers, not the object instances.
     *
     * @param other The ref pointer to compare against.
     * @return True if the ref pointers don't store the same object instance; False otherwise.
     */
    FORCEINLINE bool operator!=(const Ref& other) const { return (m_instance != other.m_instance); }

public:
    /**
     * Releases the object instance stored in this ref pointer.
     * If the object's reference count hits 0 after this release operation, it will be destroyed and
     * its memory footprint deallocated.
     */
    FORCEINLINE void Release()
    {
        if (m_instance)
        {
            if (DecrementRefCount())
            {
                btdelete (RefCounted*)m_instance;
            }

            m_instance = nullptr;
        }
    }

    /**
     * Casts this ref pointer to another type.
     * A new ref pointer will be constructed, increasing the object's reference count.
     * 
     * @return The casted ref pointer.
     */
    template<typename Q>
    FORCEINLINE Ref<Q> As() const
    {
        Ref<Q> result = Ref<Q>((Q*)m_instance);
        return result;
    }

private:
    FORCEINLINE void IncrementRefCount()
    {
        // All types used with the ref pointer system must derive from `RefCounted`.
        static_assert(std::is_base_of_v<RefCounted, T>, "T must be derived from `RefCounted`!");
        ((RefCounted*)m_instance)->IncrementReferenceCount__();
    }

    FORCEINLINE bool DecrementRefCount()
    {
        // All types used with the ref pointer system must derive from `RefCounted`.
        static_assert(std::is_base_of_v<RefCounted, T>, "T must be derived from `RefCounted`!");
        return ((RefCounted*)m_instance)->DecrementReferenceCount__();
    }

private:
    /** The pointer to the object instance that this ref pointer manages. */
    T* m_instance;
};

} // namespace Basalt
