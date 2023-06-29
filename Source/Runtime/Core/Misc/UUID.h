// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/CoreDefines.h"
#include "Core/CoreTypes.h"

namespace Basalt
{

/**
 * Unique identifier represented by a 64-bit unsigned integer.
 */
struct BASALT_API UUID
{
public:
    /** Value that represents an invalid UUID. */
    static constexpr U64 Invalid = 0;

public:
    /**
     * Default constructor. Generates a random unique identifier.
     */
    UUID();

    /**
     * Copy constructor.
     * Copies the internal value of the other UUID.
     * 
     * @param other The UUID to copy.
     */
    FORCEINLINE constexpr UUID(const UUID& other)
        : m_uuid(other.m_uuid)
    {}

    /**
     * Constructs an UUID from a 64-bit unsigned integer.
     * The provided value will represent the internal UUID value.
     * 
     * @param uuid The 64-bit unsigned integer.
     */
    FORCEINLINE constexpr UUID(U64 uuid)
        : m_uuid(uuid)
    {}

    /**
     * Copy assignment operator.
     * Copes the internal value of the other UUID.
     * 
     * @param other The UUID to copy.
     * @return Reference to this, after the copy operation.
     */
    FORCEINLINE constexpr UUID& operator=(const UUID& other)
    {
        m_uuid = other.m_uuid;
        return *this;
    }

    /**
     * Sets the internal UUID value to the provided 64-bit unsigned integer.
     * 
     * @param uuid The 64-bit unsigned integer.
     * @return Reference to this, after the copy operation.
     */
    FORCEINLINE constexpr UUID& operator=(U64 uuid)
    {
        m_uuid = uuid;
        return *this;
    }

public:
    /** @return The internal UUID value, that is represented by a 64-bit unsigned integer. */
    FORCEINLINE operator U64() const { return m_uuid; }

private:
    /** The internal UUID value. */
    U64 m_uuid;
};

} // namespace Basalt
