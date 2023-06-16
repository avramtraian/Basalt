// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/Filesystem/Filesystem.h"
#include "Core/Memory/Buffer.h"

namespace Basalt
{

class WindowsFilesystem : public IFilesystem
{
public:
    WindowsFilesystem(const FilesystemDescription& description);
    virtual ~WindowsFilesystem() override = default;

    virtual bool Initialize() override;
    virtual void Shutdown() override;

    virtual bool IsInitialized() const override { return m_is_initialized; }

public:
    virtual bool IsFileHandleValid(FileHandle file_handle) const override;

    virtual EFilesystemError GetLastErrorCode() const override { return m_last_error_code; }

public:
    virtual U64 FileSize(StringView filepath) const override;
    virtual U64 FileSize(FileHandle file_handle) const override;

    virtual FileHandle OpenForReading(StringView filepath, bool allow_writing_while_open) override;

    virtual FileHandle OpenForWriting(StringView filepath, bool allow_reading_while_open, bool append) override;

private:
    /**
     * Allocates memory to store the given filepath in Win32 API compatible mode.
     * Usually, this means that the path is converted into UTF-16. Remember that
     * all engine and game paths are UTF-8 encoded.
     * The returned pointer must be manually released from memory, using `ReleasePath()`.
     * 
     * @param filepath The path to allocate memory for and convert. Must be valid UTF-8.
     * @return Pointer to the newly allocated path. The pointer must be manually released.
     */
    const wchar_t* AllocatePath(StringView filepath) const;

    /**
     * Releases a path from memory.
     * 
     * @param filepath The filepath to release.
     */
    void ReleasePath(const wchar_t* filepath) const;

private:
    bool m_is_initialized = false;

    FilesystemDescription m_description;

    EFilesystemError m_last_error_code = EFilesystemError::Success;
};

} // namespace Basalt
