// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "StringView.h"
#include "Core/Memory/Buffer.h"

#include <cstdarg>

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
    /**
     * Formats an UTF-8 encoded string.
     * It follows the C format specification.
     * The resulted string is null-terminated.
     * 
     * @param buffer    The buffer where the formatted string will be written to.
     * @param format    The string to be formatted.
     * @param arg_list  The list of arguments that will be formatted into the string.
     * 
     * @return The number of bytes the formatted string occupies, including the null
     *         termination character. If the provided buffer is not sufficient to store
     *         the string, `InvalidSize` will be returned instead.
     */
    static Usize FormatList(Buffer buffer, StringView format, va_list arg_list);

    /**
     * Formats an UTF-8 encoded string. This version of the function doesn't allocate any memory.
     * It follows the C format specification.
     * The resulted string is null-terminated.
     *
     * @param buffer    The buffer where the formatted string will be written to.
     * @param format    The string to be formatted.
     * @param ...       The list of arguments that will be formatted into the string.
     *
     * @return The number of bytes the formatted string occupies, including the null
     *         termination character. If the provided buffer is not sufficient to store
     *         the string, `InvalidSize` will be returned instead.
     */
    FORCEINLINE static Usize Format(Buffer buffer, StringView format, ...)
    {
        va_list arg_list;
        va_start(arg_list, format);
        Usize written = FormatList(buffer, format, arg_list);
        va_end(arg_list);
        return written;
    }

    /**
     * Formats an UTF-8 encoded string. This version of the function might allocate memory.
     * It follows the C format specification.
     * The resulted string is null-terminated.
     * If the buffer is not sufficient to store the formatted string, it will dynamically
     * allocate memory until it is large enough.
     *
     * @param buffer    The buffer where the formatted string will be written to.
     * @param format    The string to be formatted.
     * @param arg_list  The list of arguments that will be formatted into the string.
     * 
     * @return The number of bytes the formatted string occupies, including the null
     *         termination character.
     */
    static Usize FormatDynamicList(Buffer& buffer, StringView format, va_list arg_list);

    /**
     * Formats an UTF-8 encoded string. This version of the function might allocate memory.
     * It follows the C format specification.
     * The resulted string is null-terminated.
     * If the buffer is not sufficient to store the formatted string, it will dynamically
     * allocate memory until it is large enough.
     *
     * @param buffer    The buffer where the formatted string will be written to.
     * @param format    The string to be formatted.
     * @param arg_list  The list of arguments that will be formatted into the string.
     *
     * @return The number of bytes the formatted string occupies, including the null
     *         termination character.
     */
    FORCEINLINE static Usize FormatDynamic(Buffer& buffer, StringView format, ...)
    {
        va_list arg_list;
        va_start(arg_list, format);
        Usize written = FormatDynamicList(buffer, format, arg_list);
        va_end(arg_list);
        return written;
    }

public:
    StringBuilder() = default;
};

} // namespace Basalt
