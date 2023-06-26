// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/CoreDefines.h"
#include "Core/CoreTypes.h"

namespace Basalt
{

/**
 * Structure that contains all of the utility functions that manipulate UTF-8
 * encoded strings and codepoints.
 */
struct BASALT_API UTF8Calls
{
public:
    /**
     * Gets the number of codepoints that the string is composed of.
     * 
     * @param string The UTF-8 encoded, null-terminated string.
     * @return The number of codepoints.
     */
    static Usize Length(const char* string);

    /**
     * Counts the number of bytes an UTF-8 encoded string occupies.
     * This function evaluates `length` characters, even if a null-terminated character is encountered.
     * If the provided string is not valid UTF-8, an assert will be issued.
     * 
     * @param string Pointer to the UTF-8 encoded string.
     * @param length The number of characters to evaluate.
     * 
     * @return The number of bytes the string occupies.
     */
    static Usize BytesCount(const char* string, Usize length);

    /**
     * Converts a codepoint to an UTF-8 encoded sequence of bytes.
     * 
     * @param codepoint The codepoint to convert.
     * @param buffer Pointer to the buffer where the bytes will be written.
     *               Unless the width of the codepoint is known, this buffer
     *               should be 4-bytes long.
     * 
     * @return The width (in bytes) of the codepoint. If zero is returned, the
     *         provided codepoint is not valid UTF-8.
     */
    static U32 CodepointToBytes(UnicodeCodepoint codepoint, void* buffer);

    /**
     * Converts an UTF-8 encoded byte sequence to a codepoint.
     * 
     * @param buffer Pointer to the buffer where the bytes are stored.
     * @param outCodepointWidth This will be filled with the width of the codepoint.
     *                          If the byte sequence is not valid UTF-8, this will be
     *                          set to zero.
     * 
     * @return The UTF-8 codepoint. If the byte sequence is not valid, `InvalidUnicodeCodepoint`
      *        will be returned.
     */
    static UnicodeCodepoint BytesToCodepoint(const void* buffer, U32* out_codepoint_width = nullptr);

    /**
     * Calculates the width of the codepoint that is represented by a given byte sequence.
     * If the byte sequence is not valid UTF-8, zero will be returned.
     * 
     * @param buffer The UTF-8 encoded byte sequence.
     * @return The width of the codepoint or zero if the byte sequence is not valid UTF-8.
     */
    static U32 BytesToCodepointWidth(const void* buffer);

    /**
     * Validates that the provided byte sequence is valid UTF-8.
     * 
     * @param string        Pointer to the buffer where the string is stored.
     * @param bytes_count   The number of bytes the string occupies.
     * 
     * @return True if the provided sequence is valid UTF-8; False otherwise.
     */
    static bool Validate(const char* string, Usize bytes_count);

    /**
     * Validates that the provided byte sequence is valid UTF-8.
     * The functions checks characters from the string until a null-termination character is encountered.
     *
     * @param null_terminated_string Pointer to the buffer where the null-terminated string is stored.
     * @return True if the provided sequence is valid UTF-8; False otherwise.
     */
    static bool Validate(const char* null_terminated_string);
};

} // namespace Basalt
