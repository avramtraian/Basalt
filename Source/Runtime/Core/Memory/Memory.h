// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/CoreDefines.h"
#include "Core/CoreTypes.h"
#include "Core/Misc/CommandLine.h"

#include <new>

namespace Basalt
{

/**
 * Structure that describes the behavior of the memory system.
 */
struct MemoryDescription
{
};

/**
 * The memory class is responsible for management of the memory system.
 * It controls all memory allocation and deallocation, as well as generating
 * the raw memory debugging information during runtime. It is tightly tied
 * with the memory profiler, as this provides the source data.
 */
class BASALT_API Memory
{
public:
    /**
     * Initializes the memory system. No memory allocations or deallocations
     * could be performed before this function is invoked.
     * 
     * @param description The description of the memory system.
     */
    static bool Initialize(const MemoryDescription& description);

    /**
     * Shuts down the memory system. No memory allocations or deallocations
     * could be performed after this function is invoked.
     */
    static void Shutdown();

public:
    /**
     * Copies a block of memory from one buffer to another.
     * 
     * @param destination Pointer to the buffer where the bytes are copied to.
     * @param source Pointer to the buffer where the bytes are copied from.
     * @param bytesCount The number of bytes to copy.
     */
    static void Copy(void* destination, const void* source, Usize bytesCount);

    /**
     * Sets each byte from a memory block to a specified value.
     * 
     * @param destination Pointer to the buffer where the bytes will be set.
     * @param value The value of the bytes.
     * @param bytesCount The number of bytes to set.
     */
    static void Set(void* destination, U8 value, Usize bytesCount);

    /**
     * Sets all bytes from a memory block to zero.
     * 
     * @param destination Pointer to the buffer where the bytes will be set.
     * @param bytesCount The number of bytes to set.
     */
    static void Zero(void* destination, Usize bytesCount);

public:
    NODISCARD static void* AllocateRaw(Usize bytesCount);

    NODISCARD static void* Allocate(Usize bytesCount);

    NODISCARD static void* AllocateTagged(Usize bytesCount, const char* fileName, U32 lineNumber);

    /**
     * Frees a memory block from the heap. This function can only release
     * memory that was allocated via `AllocateRaw()`. If `Allocate()` or
     * `AllocateTagged()` was used instead, the memory debug information
     * will not be accurate.
     * 
     * @param memoryBlock Pointer to the memory block to free.
     */
    static void FreeRaw(void* memoryBlock);

    /**
     * Frees a memory block from the heap. It will be determined during
     * runtime whether or not the block was tag allocated or raw allocated.
     * 
     * @param memoryBlock Pointer to the memory block to free.
     */
    static void Free(void* memoryBlock);
};

} // namespace Basalt


/**
 * Wrapper around `AllocateTagged()` that expands the file and line number
 * where the macro, and function implicitly, is invoked.
 */
#define AllocateTaggedI(BYTES_COUNT)    AllocateTagged(BYTES_COUNT, __FILE__, __LINE__)

/**
 * Wrapper around operator new, that expands the file and line number where
 * the macro, and operator implicitly, is invoked.
 * This macro should always be used instead of the default operator new, as
 * it provides memory tracking capabilities.
 */
#define btnew                           new(__FILE__, __LINE__)

/**
 * Wrapper around operator btdelete.
 */
#define btdelete                        delete

/** Default new operator. Allows third-party code to use the Basalt memory system. */
NODISCARD void* operator new(Basalt::Usize bytesCount);

/** New operator that provides information about the file and line number from where the allocation is called. */
NODISCARD void* operator new(Basalt::Usize bytesCount, const char* fileName, Basalt::U32 lineNumber) noexcept;

/** Default btdelete operator. */
void operator delete(void* memoryBlock);
