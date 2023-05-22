// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Memory.h"

void* operator new(Basalt::Usize bytesCount)
{
    return Basalt::Memory::Allocate(bytesCount);
}

void* operator new(Basalt::Usize bytesCount, const char* fileName, Basalt::U32 lineNumber) noexcept
{
    return Basalt::Memory::AllocateTagged(bytesCount, fileName, lineNumber);
}

void operator delete(void* memoryBlock)
{
    Basalt::Memory::Free(memoryBlock);
}
