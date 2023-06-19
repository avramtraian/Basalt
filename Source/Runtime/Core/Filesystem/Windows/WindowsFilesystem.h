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
    const wchar_t* AllocatePath(StringView filepath) const;
    void ReleasePath(const wchar_t* filepath) const;

private:
    bool m_is_initialized = false;

    FilesystemDescription m_description;

    EFilesystemError m_last_error_code = EFilesystemError::Success;

    mutable wchar_t m_filepath_buffer[256] = {};
};

} // namespace Basalt
