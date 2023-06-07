// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/CoreDefines.h"
#include "Core/CoreTypes.h"
#include "Core/Misc/AssertionMacros.h"

namespace Basalt
{

/**
 * Primitive type that represents an UTF-8 codepoint.
 * Because the UTF-8 encoding allows characters of 4-bytes the most, this is basically a `U32`.
 */
using UTF8Codepoint = U32;

constexpr UTF8Codepoint InvalidUTF8Codepoint = -1;

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
    static U32 CodepointToBytes(UTF8Codepoint codepoint, void* buffer);

    /**
     * Converts an UTF-8 encoded byte sequence to a codepoint.
     * 
     * @param buffer Pointer to the buffer where the bytes are stored.
     * @param outCodepointWidth This will be filled with the width of the codepoint.
     *                          If the byte sequence is not valid UTF-8, this will be
     *                          set to zero.
     * 
     * @return The UTF-8 codepoint. If the byte sequence is not valid, `InvalidUTF8Codepoint`
      *        will be returned.
     */
    static UTF8Codepoint BytesToCodepoint(const void* buffer, U32* outCodepointWidth = nullptr);

    /**
     * Calculates the width of the codepoint that is represented by a given byte sequence.
     * If the byte sequence is not valid UTF-8, zero will be returned.
     * 
     * @param buffer The UTF-8 encoded byte sequence.
     * @return The width if the codepoint or zero if the byte sequence is not valid UTF-8.
     */
    static U32 BytesToCodepointWidth(const void* buffer);
};

} // namespace Basalt
