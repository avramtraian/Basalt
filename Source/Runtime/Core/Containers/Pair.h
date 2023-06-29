// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/CoreDefines.h"
#include "Core/CoreTypes.h"

namespace Basalt
{

template<typename T, typename Q>
class BASALT_S_API Pair
{
public:
    Pair() = default;
    ~Pair() = default;

    FORCEINLINE Pair(const Pair& other)
        : a(other.a)
        , b(other.b)
    {}

    FORCEINLINE Pair(Pair&& other)
        : a(Move(other.a))
        , b(Move(other.b))
    {}

    FORCEINLINE Pair& operator=(const Pair& other)
    {
        a = other.a;
        b = other.b;
        return *this;
    }

    FORCEINLINE Pair& operator=(Pair&& other)
    {
        a = Move(other.a);
        b = Move(other.b);
        return *this;
    }

public:
    FORCEINLINE Pair(const T& in_a, const Q& in_b)
        : a(in_a)
        , b(in_b)
    {}

    FORCEINLINE Pair(const T& in_a, Q&& in_b)
        : a(in_a)
        , b(Move(in_b))
    {}

    FORCEINLINE Pair(T&& in_a, const Q& in_b)
        : a(Move(in_a))
        , b(in_b)
    {}

    FORCEINLINE Pair(T&& in_a, Q&& in_b)
        : a(Move(in_a))
        , b(Move(in_b))
    {}

public:
    T a;
    Q b;
};

} // namespace Basalt
