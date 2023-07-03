// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "Version.h"

#include "Core/Containers/Hash.h"

namespace Basalt
{

template<>
BASALT_API U64 Hasher::Compute<Version>(const Version& value)
{
    return Hasher::Compute<U64>(Version::Encode(value));
}

} // namespace Basalt
