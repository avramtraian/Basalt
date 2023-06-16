// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/Containers/Strings/StringView.h"
#include "Core/Containers/Unique.h"
#include "Core/CoreTypes.h"
#include "Core/CoreTypes.h"
#include "Core/Memory/Buffer.h"

namespace Basalt
{

/**
 * Structure that represents the handle of a filesystem file in an opaque way.
 */
struct FileHandle
{
    // NOTE(traian): The default value represents the Win32 invalid handle value.

    /** The native file handle. */
    void* native_handle = (void*)(-1);
};

enum class EFilesystemType : U8
{
    Physical,
    Virtual
};

enum class EFilesystemError : U32
{
    /** The filesystem operation was performed successfully. */
    Success = 0,
    /** An unknown/unexpected error occurred. */
    Unknown,
    /** The filesystem couldn't locate the file. Most likely caused by an invalid path. */
    FileNotFound,
};

struct FilesystemDescription
{
    EFilesystemType type = EFilesystemType::Physical;
};

class IFilesystem
{
public:
    static Unique<IFilesystem> Create(const FilesystemDescription& description);

public:
    virtual ~IFilesystem() = default;

    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;

    virtual bool IsInitialized() const = 0;

public:
    virtual bool IsFileHandleValid(FileHandle file_handle) const = 0;

    virtual EFilesystemError GetLastErrorCode() const = 0;

public:
    virtual U64 FileSize(StringView filepath) const = 0;

    virtual U64 FileSize(FileHandle file_handle) const = 0;

    virtual FileHandle OpenForReading(StringView filepath, bool allow_writing_while_open) = 0;

    virtual FileHandle OpenForWriting(StringView filepath, bool allow_reading_while_open, bool append) = 0;
};

} // namespace Basalt
