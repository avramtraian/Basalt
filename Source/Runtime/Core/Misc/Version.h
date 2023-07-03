// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/CoreDefines.h"
#include "Core/CoreTypes.h"
#include "Core/Memory/Memory.h"

namespace Basalt
{

struct BASALT_S_API Version
{
public:
    FORCEINLINE static U64 Encode(Version version)
    {
        U64 result = 0;
        result |= (U64)version.m_versions[0] << 48;
        result |= (U64)version.m_versions[1] << 32;
        result |= (U64)version.m_versions[2] << 16;
        result |= (U64)version.m_versions[3];
        return result;
    }

    FORCEINLINE static Version Decode(U64 source)
    {
        Version result;
        result.m_versions[0] = (source >> 48) & 0xFFFF;
        result.m_versions[1] = (source >> 32) & 0xFFFF;
        result.m_versions[2] = (source >> 16) & 0xFFFF;
        result.m_versions[3] = (source >> 0)  & 0xFFFF;
        return result;
    }

public:
    FORCEINLINE Version()
        : m_versions{ 1, 0, 0, 0 }
    {}

    FORCEINLINE Version(U16 major, U16 minor, U16 patch, U16 revision)
        : m_versions{ major, minor, patch, revision }
    {}

public:
    /** @return The major number of the version. */
    FORCEINLINE U16 Major() const { return m_versions[0]; }
    /** @return The minor number of the version. */
    FORCEINLINE U16 Minor() const { return m_versions[1]; }
    /** @return The patch number of the version. */
    FORCEINLINE U16 Patch() const { return m_versions[2]; }
    /** @return The revision number of the version. */
    FORCEINLINE U16 Revision() const { return m_versions[3]; }

public:
    FORCEINLINE bool operator> (const Version& other) const { return Version::Encode(*this) >  Version::Encode(other); }
    FORCEINLINE bool operator>=(const Version& other) const { return Version::Encode(*this) >= Version::Encode(other); }
    FORCEINLINE bool operator< (const Version& other) const { return Version::Encode(*this) <  Version::Encode(other); }
    FORCEINLINE bool operator<=(const Version& other) const { return Version::Encode(*this) <= Version::Encode(other); }
    FORCEINLINE bool operator==(const Version& other) const { return Version::Encode(*this) == Version::Encode(other); }
    FORCEINLINE bool operator!=(const Version& other) const { return Version::Encode(*this) != Version::Encode(other); }

private:
    U16 m_versions[4];
};

} // namespace Basalt 
