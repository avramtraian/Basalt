// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/CoreDefines.h"
#include "Core/CoreTypes.h"
#include "Core/Misc/AssertionMacros.h"

namespace Basalt
{

/**
 * A view towards an UTF-8 encoded, non null-terminated string.
 */
class BASALT_API StringView
{
public:
    /** Value that indicates an invalid position in the string view. */
    static constexpr Usize InvalidPos = static_cast<Usize>(-1);

    /**
     * Creates a string view from an UTF-8 encoded, null-terminated C-style string.
     * If the provided string is not valid UTF-8, an assert will be issued.
     * 
     * @param asciiString The to string to transform into a view.
     * @return The view.
     */
    static StringView FromUTF8(const char* utf8_string);

    /**
     * Creates a string view from an ASCII encoded, null-terminated C-style string.
     *
     * @param asciiString The to string to transform into a view.
     * @return The view.
     */
    static StringView FromASCII(const char* ascii_string);

public:
    /** Default constructor. Creates an empty (or invalid) view. */
    FORCEINLINE constexpr StringView()
        : m_view_data(nullptr)
        , m_bytes_count(0)
    {}

    /** Copy constructor. */
    FORCEINLINE constexpr StringView(const StringView& other)
        : m_view_data(other.m_view_data)
        , m_bytes_count(other.m_bytes_count)
    {}

    /** Move constructor. Leaves `other` as an empty (or invalid) view. */
    FORCEINLINE constexpr StringView(StringView&& other) noexcept
        : m_view_data(other.m_view_data)
        , m_bytes_count(other.m_bytes_count)
    {
        other.m_view_data = nullptr;
        other.m_bytes_count = 0;
    }

    /**
     * Creates a view from an UTF-8 encoded C-style string.
     * 
     * @param string The C-style string.
     * @param bytesCount The number of bytes the string occupies.
     */
    FORCEINLINE constexpr StringView(const char* string, Usize bytes_count)
        : m_view_data(string)
        , m_bytes_count(bytes_count)
    {}

    /**
     * Copy/Assignment operator.
     * 
     * @param other The view to copy.
     * @return Reference to this, after the copy operation.
     */
    FORCEINLINE StringView& operator=(const StringView& other)
    {
        m_view_data = other.m_view_data;
        m_bytes_count = other.m_bytes_count;

        return *this;
    }

    /**
     * Move operator.
     *
     * @param other The view to move.
     * @return Reference to this, after the move operation.
     */
    FORCEINLINE StringView& operator=(StringView&& other) noexcept
    {
        m_view_data = other.m_view_data;
        m_bytes_count = other.m_bytes_count;
        other.m_view_data = nullptr;
        other.m_bytes_count = 0;

        return *this;
    }

public:
    /** @return Const pointer to the buffer where the view data is stored. */
    FORCEINLINE const char* Data() const { return m_view_data; }

    /** @return Const pointer to the buffer where the view data is stored. Wrapper around `Data()`. */
    FORCEINLINE const char* operator*() const { return Data(); }

    /** @return Const pointer to the buffer where the view data is stored. Used for compatibility with various C-style APIs. */
    FORCEINLINE const char* c_str() const { return Data(); }

    /** @return True if the view doesn't contain any characters; False otherwise. */
    FORCEINLINE bool IsEmpty() const { return (m_bytes_count == 0); }

    /** @return The number of bytes the string view occupies. */
    FORCEINLINE Usize BytesCount() const { return m_bytes_count; }

public:
    /**
     * Function that creates a view towards a specified subregion of the original view.
     * The provided parameters are expressed in UTF-8 encoded codepoints, not in bytes.
     * 
     * @param offset The index of the first character that will be included in the view.
     * @param count The number of characters that will be included in the view.
     */
    StringView Substring(Usize offset, Usize count = 0) const;

    /**
     * Finds the index of the first, from left to right, appearance of the given substring.
     * If the substring doesn't appear in the view, `InvalidPos` will be returned.
     * 
     * @param substring The string view to find.
     * @return The offset (in UTF-8 encoded codepoints) of the first appearance, or
     *         `InvalidPos` if the substring doesn't appear in the string view.
     */
    Usize FindFirstOf(StringView substring) const;
    Usize FindFirstOf(char character) const;
    
    /**
     * Finds the index of the last, from left to right, appearance of the given substring.
     * If the substring doesn't appear in the view, `InvalidPos` will be returned.
     *
     * @param substring The string view to find.
     * @return The offset (in UTF-8 encoded codepoints) of the first appearance, or
     *         `InvalidPos` if the substring doesn't appear in the string view.
     */
    Usize FindLastOf(StringView substring) const;
    Usize FindLastOf(char character) const;

    /** Wrapper around `FindFirstOf()`. */
    FORCEINLINE Usize Find(StringView substring) const { return FindFirstOf(substring); }
    FORCEINLINE Usize Find(char character) const { return FindFirstOf(character); }

    /**
     * Checks whether or not the string view begins with the given substring.
     * 
     * @param substring The substring to check against.
     * @return True if the string view starts with the given substring; False otherwise.
     */
    bool BeginsWith(StringView substring) const;
    bool BeginsWith(char character) const;

    /**
     * Checks whether or not the string view ends with the given substring.
     *
     * @param substring The substring to check against.
     * @return True if the string view ends with the given substring; False otherwise.
     */
    bool EndsWith(StringView substring) const;
    bool EndsWith(char character) const;

    /**
     * Interprets the string view as a filepath and returns a view towards the name of the file
     * or directory the path leads to.
     * 
     * @return The name of the file or directory the path leads to.
     */
    StringView Filename() const;

    /**
     * Interprets the string view as a filepath and returns a view towards the extension of the file
     * that the path leads to.
     * If the filepath represents a directory or volume, an empty string view will be returned.
     * 
     * @return The extension of the file that the path leads to.
     */
    StringView Extension() const;

    /**
     * Interprets the string view as a filepath and returns a view towards the path, relative to the
     * original path root, where the file or directory that the path leads to is located.
     * If the path doesn't contain a parent directory, an empty string view will be returned.
     * 
     * @return The path to the parent directory, or an empty string if not existent.
     */
    StringView ParentDirectory() const;

public:
    /**
     * Equality check operator.
     * Two string views are equal if all characters are equal.
     * 
     * @param other The string view to compare against.
     * @return True if the string views are equal; False otherwise.
     */
    bool operator==(const StringView& other) const;
    
    /**
     * Inequality check operator.
     * Two string views are equal if all characters are equal.
     *
     * @param other The string view to compare against.
     * @return True if the string views are not equal; False otherwise.
     */
    bool operator!=(const StringView& other) const;

private:
    /** Pointer to the buffer that contains the string data. */
    const char* m_view_data;

    /** The number of bytes the view ranges across. */
    Usize m_bytes_count;

private:
    friend class String;
};

// Disable: literal suffix identifiers that do not start with an underscore are reserved.
#if BT_COMPILER_MSVC
    #pragma warning(push)
    #pragma warning(disable : 4455)
#endif // BT_COMPILER_MSVC

/**
 * Literal operator that transforms an ASCII string literal to an UTF-8 encoded string view.
 * 
 * @param asciiString The ASCII string literal.
 * @param stringLength The length (in characters) of the string.
 * 
 * @return A view towards the string literal.
 */
FORCEINLINE constexpr StringView operator""sv(const char* ascii_string, Usize string_length)
{
    // Because the string literal contains only ASCII characters, the string has as many bytes as characters.
    return StringView(ascii_string, string_length);
}

#if BT_COMPILER_MSVC
    #pragma warning(pop)
#endif // BT_COMPILER_MSVC

} // namespace Basalt
