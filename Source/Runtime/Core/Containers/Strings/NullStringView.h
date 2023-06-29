// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/CoreDefines.h"
#include "Core/CoreTypes.h"
#include "Core/Misc/AssertionMacros.h"
#include "StringView.h"

namespace Basalt
{

class BASALT_API NullStringView
{
public:
    BT_NON_MOVABLE(NullStringView);

public:
    /** Copy constructor. */
    FORCEINLINE constexpr NullStringView(const NullStringView& other)
        : m_view_data(other.m_view_data)
        , m_bytes_count(other.m_bytes_count)
    {}

    /**
     * Creates a view from an UTF-8 encoded C-style string.
     *
     * @param string The C-style string.
     * @param bytesCount The number of bytes the string occupies.
     */
    FORCEINLINE constexpr NullStringView(const char* string, Usize bytes_count)
        : m_view_data(string)
        , m_bytes_count(bytes_count)
    {}

    /**
     * Copy/Assignment operator.
     *
     * @param other The view to copy.
     * @return Reference to this, after the copy operation.
     */
    FORCEINLINE constexpr NullStringView& operator=(const NullStringView& other)
    {
        m_view_data = other.m_view_data;
        m_bytes_count = other.m_bytes_count;

        return *this;
    }

public:
    /** @return Const pointer to the buffer where the view data is stored. */
    FORCEINLINE const char* Data() const { return m_view_data; }

    /** @return Const pointer to the buffer where the view data is stored. Wrapper around `Data()`. */
    FORCEINLINE const char* operator*() const { return Data(); }

    /** @return Const pointer to the buffer where the view data is stored. Used for compatibility with various C-style APIs. */
    FORCEINLINE const char* c_str() const { return Data(); }

    /** @return True if the view doesn't contain any characters, excluding the null-termination character; False otherwise. */
    FORCEINLINE bool IsEmpty() const { return (m_bytes_count <= 1); }

    /** @return The number of bytes the string view occupies, including the null-termination byte. */
    FORCEINLINE Usize BytesCount() const { return m_bytes_count; }

public:
    FORCEINLINE StringView AsStringView() const { return StringView(m_view_data, m_bytes_count - 1); }

private:
    /** Pointer to the buffer that contains the string data. */
    const char* m_view_data;

    /** The number of bytes the view ranges across. */
    Usize m_bytes_count;
};

// Disable: literal suffix identifiers that do not start with an underscore are reserved.
#if BASALT_COMPILER_MSVC
    #pragma warning(push)
    #pragma warning(disable : 4455)
#endif // BASALT_COMPILER_MSVC

/**
 * Literal operator that transforms an ASCII string literal to an UTF-8 encoded string view.
 * 
 * @param asciiString The ASCII string literal.
 * @param stringLength The length (in characters) of the string.
 * 
 * @return A view towards the string literal.
 */
FORCEINLINE constexpr NullStringView operator""nsv(const char* ascii_string, Usize string_length)
{
    // Because the string literal contains only ASCII characters, the string has as many bytes as characters.
    return NullStringView(ascii_string, string_length + 1);
}

#if BASALT_COMPILER_MSVC
    #pragma warning(pop)
#endif // BASALT_COMPILER_MSVC

} // namespace Basalt
