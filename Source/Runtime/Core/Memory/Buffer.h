// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Memory.h"

namespace Basalt
{

class BASALT_API Buffer
{
public:
    FORCEINLINE Buffer()
        : data(nullptr)
        , size(0)
    {}

    FORCEINLINE Buffer(const Buffer& other)
        : data(other.data)
        , size(other.size)
    {}

    FORCEINLINE Buffer& operator=(const Buffer& other)
    {
        data = other.data;
        size = other.size;
        return *this;
    }

    FORCEINLINE explicit Buffer(Usize initial_size)
        : data(nullptr)
        , size(0)
    {
        Invalidate(initial_size);
    }

public:
    FORCEINLINE void Invalidate(Usize in_size)
    {
        Release();

        data = bnew U8[in_size];
        size = in_size;
    }

    FORCEINLINE void Release()
    {
        delete[] data;
        size = 0;
    }

    template<typename T>
    FORCEINLINE T* As() const { return (T*)data; }

    template<typename T>
    FORCEINLINE Usize CountOf() const { return (size / sizeof(T)); }

public:
    U8* data;
    Usize size;
};

} // namespace Basalt
