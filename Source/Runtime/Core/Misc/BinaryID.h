// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/CoreDefines.h"
#include "Core/CoreTypes.h"

namespace Basalt
{

template<Usize C>
struct BASALT_S_API BinaryID
{
public:
    BinaryID() = default;

    FORCEINLINE BinaryID(const char* id_string)
    {
        for (Usize index = 0; index < C; ++index)
        {
            m_id[index] = id_string[index];
        }
    }

public:
    FORCEINLINE U8& operator[](Usize index) { return m_id[index]; }
    FORCEINLINE const U8& operator[](Usize index) const { return m_id[index]; }

    FORCEINLINE bool operator==(const char* id_string) const
    {
        for (Usize index = 0; index < C; ++index)
        {
            if (m_id[index] != id_string[index])
            {
                // The ID's are not equal.
                return false;
            }
        }

        // The ID's are the equal.
        return true;
    }

    FORCEINLINE bool operator!=(const char* id_string) const
    {
        for (Usize index = 0; index < C; ++index)
        {
            if (m_id[index] != id_string[index])
            {
                // The ID's are not equal.
                return true;
            }
        }

        // The ID's are the equal.
        return false;
    }

private:
    U8 m_id[C] = {};
};

} // namespace Basalt
