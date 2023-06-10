// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "String.h"
#include "Core/Memory/Buffer.h"

namespace Basalt
{

class BASALT_API StringBuilder
{
public:
    /**
     * Converts an UTF-8 encoded string to an UTF-16 encoded string.
     * The conversion doesn't allocate any memory. If the provided buffer is not
     * sufficient to store the UTF-16 string, this function will return `InvalidSize`.
     * 
     * @param utf8_view The view towards the UTF-8 string to convert. If this string
     *                  is not valid UTF-8 an assert will be issued.
     * @param utf16_buffer The buffer where the UTF-16 string will be written to.
     * 
     * @return The number of bytes the UTF-16 string occupies. If the buffer is not
     *         sufficient to store the string, `InvalidSize` will be returned.
     */
    static Usize ToUTF16(StringView utf8_view, Buffer utf16_buffer);

    /**
     * Converts an UTF-8 encoded string to an UTF-16 encoded string.
     * The conversion can allocate memory if the provided buffer is not sufficient
     * to store the UTF-16 string.
     * 
     * @param utf8_view The view towards the UTF-8 string to convert. If this string
     *                  is not valid UTF-8 an assert will be issued.
     * @param utf16_buffer Reference to the buffer where the UTF-16 string will be written
     *                     to. If the provided buffer is not sufficient to store the string,
     *                     it will dynamically grow until it is big enough.
     * 
     * @return The number of bytes the UTF-16 string occupies.
     */
    static Usize DynamicToUTF16(StringView utf8_view, Buffer& utf16_buffer);

public:
    StringBuilder() = default;

    FORCEINLINE StringBuilder(const StringView& view)
        : m_string(view)
    {}

    FORCEINLINE StringBuilder(const String& string)
        : m_string(string)
    {}

public:
    void SetCapacity(Usize new_capacity);

private:
    String m_string;
};

} // namespace Basalt
