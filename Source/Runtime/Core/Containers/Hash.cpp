// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "Hash.h"

#include "Strings/String.h"
#include "Strings/StringView.h"
#include "Strings/Utf16String.h"
#include "Strings/Utf8String.h"

namespace Basalt
{

template<typename T>
U64 Hasher::Compute(const T& value)
{
    static_assert(false, "Override Hasher::Compute(const T& value) in order to hash T!");
    return 0;
}

U64 Hasher::ComputeByteSequence(const void* buffer, Usize bytes_count)
{
    return 0;
}

////////////////////////////////////////////////////////////////
////////////////////// UNSIGNED INTEGERS ///////////////////////
////////////////////////////////////////////////////////////////

template<>
BASALT_API U64 Hasher::Compute<U64>(const U64& value)
{
    return value;
}

template<>
BASALT_API U64 Hasher::Compute<U8>(const U8& value)
{
    return Hasher::Compute<U64>((U64)value);
}

template<>
BASALT_API U64 Hasher::Compute<U16>(const U16& value)
{
    return Hasher::Compute<U64>((U64)value);
}

template<>
BASALT_API U64 Hasher::Compute<U32>(const U32& value)
{
    return Hasher::Compute<U64>((U64)value);
}

////////////////////////////////////////////////////////////////
/////////////////////// SIGNED INTEGERS ////////////////////////
////////////////////////////////////////////////////////////////

template<>
BASALT_API U64 Hasher::Compute<I8>(const I8& value)
{
    return Hasher::Compute<U64>((U64)value);
}

template<>
BASALT_API U64 Hasher::Compute<I16>(const I16& value)
{
    return Hasher::Compute<U64>((U64)value);
}

template<>
BASALT_API U64 Hasher::Compute<I32>(const I32& value)
{
    return Hasher::Compute<U64>((U64)value);
}

template<>
BASALT_API U64 Hasher::Compute<I64>(const I64& value)
{
    return Hasher::Compute<U64>((U64)value);
}

////////////////////////////////////////////////////////////////
/////////////////// FLOATING POINT NUMBERS /////////////////////
////////////////////////////////////////////////////////////////

template<>
BASALT_API U64 Hasher::Compute<F32>(const F32& value)
{
    const U64 uinteger = (U64)(*(U32*)&value);
    return Hasher::Compute<U64>(uinteger);
}

template<>
BASALT_API U64 Hasher::Compute<F64>(const F64& value)
{
    const U64 uinteger = (U64)(*(U64*)&value);
    return Hasher::Compute<U64>(uinteger);
}

////////////////////////////////////////////////////////////////
/////////////////////////// STRINGS ////////////////////////////
////////////////////////////////////////////////////////////////

template<>
BASALT_API U64 Hasher::Compute<String>(const String& value)
{
    return 0;
}

} // namespace Basalt
