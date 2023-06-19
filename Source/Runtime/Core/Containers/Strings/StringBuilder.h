// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "StringView.h"
#include "Core/Memory/Buffer.h"

namespace Basalt
{

class BASALT_API StringBuilder
{
public:
    /**
     * Converts an UTF-8 encoded string to an UTF-16 encoded string.
     * The conversion doesn't allocate any memory. If the provided buffer is not sufficient to store
     * the UTF-16 string, this function will return `InvalidSize`.
     * 
     * @param utf8_view                     The view towards the UTF-8 string to convert. If this string
     *                                      is not valid UTF-8 an assert will be issued.
     * @param utf16_buffer                  The buffer where the UTF-16 string will be written to.
     * @param include_null_termination_char Whether or not to also write a null-termination
     *                                      character to the UTF-16 string.
     * 
     * @return The number of bytes the UTF-16 string occupies. If the buffer is not sufficient to store
     * the string, `InvalidSize` will be returned.
     */
    static Usize ToUTF16(StringView utf8_view, Buffer utf16_buffer, bool include_null_termination_char);

    /**
     * Converts an UTF-8 encoded string to an UTF-16 encoded string.
     * The conversion can allocate memory if the provided buffer is not sufficient to store the UTF-16 string.
     * 
     * @param utf8_view                     The view towards the UTF-8 string to convert. If this string
     *                                      is not valid UTF-8 an assert will be issued.
     * @param utf16_buffer                  Reference to the buffer where the UTF-16 string will be written
     *                                      to. If the provided buffer is not sufficient to store the string,
     *                                      it will dynamically grow until it is big enough.
     * @param include_null_termination_char Whether or not to also write a null-termination
     *                                      character to the UTF-16 string.
     * 
     * @return The number of bytes the UTF-16 string occupies.
     */
    static Usize ToUTF16Dynamic(StringView utf8_view, Buffer& utf16_buffer, bool include_null_termination_char);

    /**
     * Converts an UTF-16 encoded string to an UTF-8 encoded string.
     * The conversion doesn't allocate any memory. If the provided buffer is not
     * sufficient to store the UTF-8 string, the function will return `InvalidSize`.
     * 
     * @param utf16_string                  The UTF-16 encoded, null-terminated string.
     * @param utf8_buffer                   The buffer where the UTF-8 string will be written to.
     * @param include_null_termination_char Whether or not to also write a null-termination
     *                                      character to the UTF-8 string.
     * 
     * @return The number of bytes the UTF-8 string occupies. If the buffer is not sufficient to
     *         store the string, `InvalidSize` will be returned.
     */
    static Usize FromUTF16(const wchar_t* utf16_string, Buffer utf8_buffer, bool include_null_termination_char);
    
    /**
     * Converts an UTF-16 encoded string to an UTF-8 encoded string.
     * The conversion can allocate memory if the provided buffer is not sufficient to store the UTF-16 string.
     * 
     * @param utf16_string                  The UTF-16 encoded, null-terminated string.
     * @param utf8_buffer                   The buffer where the UTF-8 string will be written to.
     * @param include_null_termination_char Whether or not to also write a null-termination
     *                                      character to the UTF-8 string.
     * 
     * @return The number of bytes the UTF-8 string occupies.
     */
    static Usize FromUTF16Dynamic(const wchar_t* utf16_string, Buffer& utf8_buffer, bool include_null_termination_char);

public:
    StringBuilder() = default;
};

} // namespace Basalt
