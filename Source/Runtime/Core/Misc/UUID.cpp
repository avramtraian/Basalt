// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "UUID.h"

#include "Core/Containers/Hash.h"

#include <random>

namespace Basalt
{

static std::random_device s_random_device;
static std::mt19937_64 s_engine(s_random_device());
static std::uniform_int_distribution<U64> s_uniform_distribution;

UUID::UUID()
    : m_uuid(s_uniform_distribution(s_engine))
{
}

template<>
BASALT_API U64 Hasher::Compute<UUID>(const UUID& value)
{
    return Hasher::Compute<U64>((U64)value);
}

} // namespace Basalt
