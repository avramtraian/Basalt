// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/Containers/Strings/NullStringView.h"
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
public:
    /** The native file handle. */
    void* native_handle = (void*)(-1);

    /** The size of the file, expressed in bytes. */
    U64 file_size = 0;

public:
    /** @return The native handle. */
    FORCEINLINE void* operator*() const { return native_handle; }
};

enum class EFilesystemType : U8
{
    Physical,
    Virtual
};

/**
 * Enumeration of error codes that the filesystem uses to propagate errors.
 */
enum class EFilesystemError : U32
{
    /** The filesystem operation was performed successfully. */
    Success = 0,

    /** An unknown/unexpected error occurred. */
    Unknown,

    /** The provided file handle is invalid. */
    InvalidFileHandle,

    /** The filesystem couldn't locate the file. Most likely caused by an invalid path. */
    FileNotFound,

    /** The provided handle represents a directory, but a file was expected. */
    FileIsDirectory,
};

/**
 * Structure that describes how the filesystem should work.
 */
struct FilesystemDescription
{
    EFilesystemType type = EFilesystemType::Physical;
};

/**
 * Abstract interface that all filesystems, virtual or physical, must provide.
 */
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
    /**
     * Checks if the given handle represents a valid file in the filesystem.
     * 
     * @param file_handle The file handle to check.
     * @return True if the handle is valid and can be used; False otherwise.
     */
    virtual bool IsFileHandleValid(FileHandle file_handle) const = 0;

    /** @return The last error code posted by the filesystem implementation. */
    virtual EFilesystemError GetLastErrorCode() const = 0;

public:
    /**
     * Gets the size (in bytes) of a file.
     * 
     * Error codes:
     *  - FileNotFound      => The provided file handle is invalid.
     *  - FileIsDirectory   => The provided file handle represents a directory.
     * 
     * @param filepath The path to the file.
     * @return The size (in bytes) of the file, or -1 if an error occurs.
     */
    virtual U64 FileSize(NullStringView filepath) const = 0;

    /**
     * Opens a file for reading purposes.
     * 
     * Error codes:
     *  - FileNotFound  => The file doesn't exist. Most likely caused by providing a path
     *                     that doesn't represents a valid file in the filesystem.
     *  - Unknown       => An unknown error occurred.
     * 
     * @param filepath                  The path where the file is located in the filesystem.
     * @param allow_writing_while_open  Whether or not other processes can access the file
     *                                  and write to it while it is opened by this process.
     * 
     * @return The file handle that represents the opened file. If the handle is invalid,
     *         an error occurred. Inspect `GetLastErrorCode()` for more details.
     */
    virtual FileHandle OpenForReading(NullStringView filepath, bool allow_writing_while_open) = 0;

    /**
     * Opens a file for writing purposes.
     * 
     * Error codes:
     *  - Unknown => An unknown error occurred.
     * 
     * @param filepath                  The path where the file is located in the filesystem.
     * @param allow_reading_while_open  Whether or not other processes can access the file and
     *                                  read from it while it is opened by this process.
     * @param append                    Whether or not to append the bytes to the data that
     *                                  is currently stored in the file.
     * 
     * @return The file handle that represents the opened file. If this handle is invalid, an
     *         error occured. Inspect `GetLastErrorCode()` for more details.
     */
    virtual FileHandle OpenForWriting(NullStringView filepath, bool allow_reading_while_open, bool append) = 0;

    /**
     * Closes the given file handle.
     * 
     * @param file_handle Handle to the file to close. If this is an invalid file handle,
     *                    no action will be performed.
     */
    virtual void CloseFileHandle(FileHandle& file_handle) = 0;

    /**
     * Reads data from a file.
     * 
     * Error codes:
     *  - InvalidFileHandle => The provided file handle is invalid.
     *  - Unknown           => An unknown error occurred.
     * 
     * @param file_handle           The handle to the file to read from.
     * @param buffer                The memory buffer where the read bytes will be written to.
     * @param bytes_count_to_read   The maximum number of bytes to read from. The provided
     *                              buffer must be at least this bytes long.
     * @param file_offset           The offset (in bytes) where the file reading should begin from.
     *                              If this is greater or equal to the total file size, no action will
     *                              be performed - not an error.
     * 
     * @return The number of bytes read from the file. If this is not less or equal to `bytes_count_to_read`,
     *         or it is equal to -1, an error occurred. Inspect `GetLastErrorCode()` for more details.
     */
    virtual U64 ReadFromFile(FileHandle file_handle, void* buffer, U64 bytes_count_to_read, U64 file_offset) = 0;

    /**
     * Writes data to a file.
     * 
     * Error codes:
     *  - InvalidFileHandle => The provided file handle is invalid.
     *  - Unknown => An unknown error occurred.
     * 
     * @param file_handle   The handle to the file to write to.
     * @param buffer        The memory buffer where the bytes to write to the file are located.
     * @param bytes_count   The number of bytes to write to the file. The provided buffer
     *                      must be at least this bytes long. If this value is 0, an assert
     *                      will be issued.
     * 
     * @return The number of bytes written to the file. If this value is not equal to
     *         `bytes_count` an error occurred. Inspect `GetLastErrorCode()` for more details.
     */
    virtual U64 WriteToFile(FileHandle file_handle, const void* buffer, U64 bytes_count) = 0;
};

} // namespace Basalt
