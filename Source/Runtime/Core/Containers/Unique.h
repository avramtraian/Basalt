// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/CoreDefines.h"
#include "Core/CoreTypes.h"
#include "Core/Memory/Memory.h"
#include "Core/Misc/AssertionMacros.h"

namespace Basalt
{

/**
 * Container that allows a single pointer to an object instance to be present
 * at any given point in time. It can't be copied, only moved.
 * 
 * @param T The type of the object instance.
 */
template<typename T>
class BASALT_S_API Unique
{
public:
    template<typename... Args>
    FORCEINLINE static Unique<T> Create(Args&&... args)
    {
        Unique<T> result;
        result.m_instance = bnew T(Forward<Args>(args)...);
        return result;
    }

    FORCEINLINE static Unique<T> Adopt(T* instance)
    {
        Unique<T> result;
        result.m_instance = instance;
        return result;
    }

public:
    FORCEINLINE Unique()
        : m_instance(nullptr)
    {}

    FORCEINLINE Unique(Unique&& other) noexcept
        : m_instance(other.m_instance)
    {
        other.m_instance = nullptr;
    }

    FORCEINLINE Unique(std::nullptr_t)
        : m_instance(nullptr)
    {}

    FORCEINLINE ~Unique()
    {
        Release();
    }

    // Unique pointers can't be copied.
    Unique(const Unique&) = delete;
    Unique& operator=(const Unique&) = delete;

    FORCEINLINE Unique& operator=(Unique&& other) noexcept
    {
        Release();

        m_instance = other.m_instance;
        other.m_instance = nullptr;

        return *this;
    }

    FORCEINLINE Unique& operator=(std::nullptr_t)
    {
        Release();
        return *this;
    }

public:
    /** @return True if the unique pointer is valid (it stores an object instance); False otherwise. */
    FORCEINLINE bool IsValid() const { return (m_instance != nullptr); }

    /** @return Pointer to the stored object instance. If the unique pointer is invalid, nullptr will be returned. */
    FORCEINLINE T* Raw() { return m_instance; }
    FORCEINLINE const T* Raw() const { return m_instance; }

    /** @return Pointer to the stored object instance. If the unique pointer is invalid, an assert will be issued. */
    FORCEINLINE T* Get()
    {
        Check(IsValid()); // Trying to get an invalid pointer.
        return m_instance;
    }

    /** @return Const pointer to the stored object instance. If the unique pointer is invalid, an assert will be issued. */
    FORCEINLINE const T* Get() const
    {
        Check(IsValid()); // Trying to get an invalid pointer.
        return m_instance;
    }

    /** @return Reference to the stored object instance. If the unique pointer is invalid, an assert will be issued. */
    FORCEINLINE T& GetRef() { return *Get(); }
    FORCEINLINE const T& GetRef() const { return *Get(); }

public:
    /** @return Pointer to the stored object instance. If the unique pointer is invalid, an assert will be issued. */
    FORCEINLINE T* operator->() { return Get(); }
    FORCEINLINE const T* operator->() const { return Get(); }

    /** @return Reference to the stored object instance. If the unique pointer is invalid, an assert will be issued. */
    FORCEINLINE T& operator*() { return GetRef(); }
    FORCEINLINE const T& operator*() const { return GetRef(); }

    /** @return True if the unique pointer is valid (it stores an object instance); False otherwise. */
    FORCEINLINE operator bool() const { return IsValid(); }

public:
    /**
     * Releases the object instance stored in this unique pointer.
     * If the instance is valid, it will be destructed and its memory will be released.
     */
    FORCEINLINE void Release()
    {
        if (m_instance)
        {
            delete m_instance;
            m_instance = nullptr;
        }
    }

    /**
     * Casts this unique pointer to another type.
     * A new unique pointer will be created, stealing the instance from this.
     * After the cast, this unique pointer will be left in an invalid state.
     * 
     * @return The casted unique pointer.
     */
    template<typename Q>
    FORCEINLINE Unique<Q> As()
    {
        Unique<Q> result = Unique<Q>::Adopt((Q*)m_instance);
        m_instance = nullptr;
        return result;
    }

private:
    /** The internal object instance. */
    T* m_instance;
};

} // namespace Basalt
