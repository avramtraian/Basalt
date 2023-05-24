// Copyright(c) 2023 Basalt Technology Group.All rights reserved.

#pragma once

#include "Core/CoreDefines.h"
#include "Core/CoreTypes.h"

namespace Basalt
{

/**
 * Structure that contains all of the utility functions that manipulate ASCII
 * encoded strings and codepoints.
 */
struct BASALT_API ASCIICalls
{
public:
    /**
     * Gets the length of the given string.
     * 
     * @param string An ASCII, null-terminated C-style string.
     * @return The length of the string, excluding the null-terminating character.
     */
    static Usize Length(const char* string);
};

} // namespace Basalt
