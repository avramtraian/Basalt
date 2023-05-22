// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "Memory.h"
#include "MemoryOperators.h"

#include <cstring>
#include <cstdlib>

namespace Basalt
{

bool Memory::Initialize(const MemoryDescription& description)
{
    return true;
}

void Memory::Shutdown()
{
}

void Memory::Copy(void* destination, void* source, Usize bytesCount)
{
    std::memcpy(destination, source, bytesCount);
}

void Memory::Set(void* destination, U8 value, Usize bytesCount)
{
    std::memset(destination, value, bytesCount);
}

void Memory::Zero(void* destination, Usize bytesCount)
{
    std::memset(destination, 0, bytesCount);
}

void* Memory::AllocateRaw(Usize bytesCount)
{
    if (bytesCount == 0)
    {
        return nullptr;
    }

    return malloc(bytesCount);
}

void* Memory::Allocate(Usize bytesCount)
{
    return AllocateRaw(bytesCount);
}

void* Memory::AllocateTagged(Usize bytesCount, const char* fileName, U32 lineNumber)
{
    return Allocate(bytesCount);
}

void Memory::FreeRaw(void* memoryBlock)
{
    if (memoryBlock == nullptr)
    {
        return;
    }

    free(memoryBlock);
}

void Memory::Free(void* memoryBlock)
{
    FreeRaw(memoryBlock);
}

} // namespace Basalt
