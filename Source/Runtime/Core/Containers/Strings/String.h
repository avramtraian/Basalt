// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/CoreDefines.h"
#include "Core/CoreTypes.h"

#include "StringView.h"

namespace Basalt
{

/**
 * UTF-8 encoded, null-terminated string implementation.
 * Uses small string optimizations.
 */
class BASALT_API String
{
public:
    /** The number of bytes (including the null-terminating byte) the string can store inline. */
    static constexpr Usize InlineCapacity = sizeof(char*) / sizeof(char);

public:
    /** Default constructor. A string in the default state only has the null-terminating byte. */
    String();
    
    /** Copy constructor. */
    String(const String& other);

    /** Move constructor. Leaves `other` in the default state. */
    String(String&& other) noexcept;

    /** View constructor. Creates a string from a view. */
    String(StringView view);

    /** Destructor. If the string is stored on the heap, the memory block will be released. */
    ~String();

    /**
     * Copy assignment operator.
     * 
     * @param other The string to copy.
     * @return Reference to this, after the copy operation.
     */
    String& operator=(const String& other);
    
    /**
     * Move assignment operator.
     *
     * @param other The string to move.
     * @return Reference to this, after the move operation.
     */
    String& operator=(String&& other) noexcept;
    
    /**
     * View assignment operator. Copies the view into this string.
     * 
     * @param view The view to copy.
     * @return Reference to this, after the copy operator.
     */
    String& operator=(StringView view);

public:
    /** @return Pointer to the buffer where the elements are stored. The memory could be on the heap or on the stack. */
    FORCEINLINE char* Data() { return (m_bytes_count <= InlineCapacity) ? m_inline_data : m_heap_data; }
    FORCEINLINE const char* Data() const { return (m_bytes_count <= InlineCapacity) ? m_inline_data : m_heap_data; }

    /** Wrappers around `Data()`. */
    FORCEINLINE char* operator*() { return Data(); }
    FORCEINLINE const char* operator*() const { return Data(); }

    /** @return Pointer to the buffer where the elements are stored. Used for compatibility with various C-style APIs. */
    FORCEINLINE const char* c_str() const { return Data(); }
    
    /** @return True if the string is empty (there are no characters except the null-terminating one); False otherwise. */
    FORCEINLINE bool IsEmpty() const { return (m_bytes_count == 1); }

    /** @return The number of bytes, including the null-terminating byte, the string occupies. */
    FORCEINLINE Usize BytesCount() const { return m_bytes_count; }

public:
    /** @return A view towards this string. */
    FORCEINLINE StringView ToView() const { return StringView(Data(), m_bytes_count - 1); }

    /**
     * Function that creates a view towards a specified subregion of the original view.
     * The provided parameters are expressed in UTF-8 encoded codepoints, not in bytes.
     * Wrapper around `StringView::Substring()`.
     *
     * @param offset The index of the first character that will be included in the view.
     * @param count The number of characters that will be included in the view.
     */
    FORCEINLINE StringView Substring(Usize offset, Usize count = 0) const { return ToView().Substring(offset, count); }

    /**
     * Finds the index of the first, from left to right, appearance of the given substring.
     * If the substring doesn't appear in the view, `InvalidPos` will be returned.
     * Wrapper around `StringView::FindFirstOf()`.
     *
     * @param substring The string view to find.
     * @return The offset (in UTF-8 encoded codepoints) of the first appearance, or
     *         `InvalidPos` if the substring doesn't appear in the string view.
     */
    FORCEINLINE Usize FindFirstOf(StringView substring) const { return ToView().FindFirstOf(substring); }
    FORCEINLINE Usize FindFirstOf(StringView substring, Usize offset, Usize count = 0) const { return ToView().FindFirstOf(substring, offset, count); }
    FORCEINLINE Usize FindFirstOf(char character) const { return ToView().FindFirstOf(character); }
    FORCEINLINE Usize FindFirstOf(char character, Usize offset, Usize count = 0) const { return ToView().FindFirstOf(character, offset, count); }

    /**
     * Finds the index of the last, from left to right, appearance of the given substring.
     * If the substring doesn't appear in the view, `InvalidPos` will be returned.
     * Wrapper around `StringView::FindLastOf()`.
     *
     * @param substring The string view to find.
     * @return The offset (in UTF-8 encoded codepoints) of the first appearance, or
     *         `InvalidPos` if the substring doesn't appear in the string view.
     */
    FORCEINLINE Usize FindLastOf(StringView substring) const { return ToView().FindLastOf(substring); }
    FORCEINLINE Usize FindLastOf(StringView substring, Usize offset, Usize count = 0) const { return ToView().FindLastOf(substring, offset, count); }
    FORCEINLINE Usize FindLastOf(char character) const { return ToView().FindLastOf(character); }
    FORCEINLINE Usize FindLastOf(char character, Usize offset, Usize count = 0) const { return ToView().FindLastOf(character, offset, count); }

    /** Wrapper around `FindFirstOf()`. */
    FORCEINLINE Usize Find(StringView substring) const { return FindFirstOf(substring); }
    FORCEINLINE Usize Find(StringView substring, Usize offset, Usize count = 0) const { return FindFirstOf(substring, offset, count); }
    FORCEINLINE Usize Find(char character) const { return FindFirstOf(character); }
    FORCEINLINE Usize Find(char character, Usize offset, Usize count = 0) const { return FindFirstOf(character, offset, count); }
    
    FORCEINLINE bool BeginsWith(StringView substring) const { return ToView().BeginsWith(substring); }
    FORCEINLINE bool BeginsWith(char character) const { return ToView().BeginsWith(character); }

    FORCEINLINE bool EndsWith(StringView substring) const { return ToView().EndsWith(substring); }
    FORCEINLINE bool EndsWith(char character) const { return ToView().EndsWith(character); }

    FORCEINLINE StringView Stem() const { return ToView().Stem(); }
    FORCEINLINE StringView Extension() const { return ToView().Extension(); }
    FORCEINLINE StringView Filename() const { return ToView().Filename(); }
    FORCEINLINE StringView ParentDirectory() const { return ToView().ParentDirectory(); }
    FORCEINLINE StringView ParentDirectory(U32 depth) const { return ToView().ParentDirectory(depth); }

    String Append(StringView view) const;
    FORCEINLINE String Append(const String& other) const { return Append(other.ToView()); }

public:
    FORCEINLINE char operator[](Usize index) const { return Data()[index]; }

    FORCEINLINE String operator+(const String& other) const
    {
        return Append(other.ToView());
    }

    FORCEINLINE String operator+(StringView view) const
    {
        return Append(view);
    }

    FORCEINLINE bool operator==(const String& other) const
    {
        return (ToView() == other.ToView());
    }

    FORCEINLINE bool operator!=(const String& other) const
    {
        return (ToView() != other.ToView());
    }

public:
    void SetCount(Usize bytes_count);

private:
    /**
     * Allocates a block of memory from the heap.
     * All allocations must be done using this function, in order to provide better memory debugging.
     *
     * @param capacity The capacity (in bytes) of the memory block.
     * @return Pointer to the newly allocated memory buffer.
     */
    static char* AllocateMemory(Usize bytes_count);

    /**
     * Releases a memory block.
     * All deallocations must be done using this function, in order to provide better memory debugging.
     *
     * @param elements Pointer to the memory buffer to release.
     * @param capacity The capacity (in bytes) of the memory block.
     */
    static void ReleaseMemory(char* data, Usize bytes_count);

    /**
     * Assigns a view to this string.
     * 
     * @param view The view to assign.
     */
    void AssignView(StringView view);

private:
    union
    {
        /**
         * Pointer to the heap buffer where the elements are stored.
         * If the string stores the elements inline, this variable contains invalid data.
         */
        char* m_heap_data;

        /**
         * The inline buffer where the elements are stored.
         * If the string stores the elements on the heap, this buffer contains invalid data.
         */
        char m_inline_data[InlineCapacity];
    };

    /** The number of bytes the string occupies, including the null-terminating byte. */
    Usize m_bytes_count;

private:
    friend class StringBuilder;
};

} // namespace Basalt
