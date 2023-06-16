// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "WindowsFilesystem.h"

#include "Core/Containers/Strings/StringBuilder.h"

#include <Windows.h>

namespace Basalt
{

WindowsFilesystem::WindowsFilesystem(const FilesystemDescription& description)
    : m_description(description)
{
}

bool WindowsFilesystem::Initialize()
{
    if (m_is_initialized)
    {
        // The filesystem was already initialized.
        return false;
    }

    m_is_initialized = true;
    return true;
}

void WindowsFilesystem::Shutdown()
{
    if (!m_is_initialized)
    {
        // The filesystem was already shut down.
        return;
    }

    m_is_initialized = false;
}

bool WindowsFilesystem::IsFileHandleValid(FileHandle file_handle) const
{
    return (file_handle.native_handle != INVALID_HANDLE_VALUE);
}

U64 WindowsFilesystem::FileSize(StringView filepath) const
{
    const TCHAR* win32_filepath = AllocatePath(filepath);

    WIN32_FILE_ATTRIBUTE_DATA attributes = {};
    BOOL result = GetFileAttributesEx(win32_filepath, GetFileExInfoStandard, &attributes);
    ReleasePath(win32_filepath);
    
    if (result == FALSE || attributes.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        return -1;
    }

    return (U64)attributes.nFileSizeLow | ((U64)attributes.nFileSizeHigh << 32);
}

U64 WindowsFilesystem::FileSize(FileHandle file_handle) const
{
    LARGE_INTEGER file_size;
    BOOL result = GetFileSizeEx(file_handle.native_handle, &file_size);
    return result ? file_size.QuadPart : -1;
}

FileHandle WindowsFilesystem::OpenForReading(StringView filepath, bool allow_writing_while_open)
{
    m_last_error_code = EFilesystemError::Unknown;
    FileHandle file_handle = {};

    DWORD access_flags = GENERIC_READ;
    DWORD share_mode = FILE_SHARE_READ | (allow_writing_while_open ? FILE_SHARE_WRITE : 0);
    DWORD create_mode = OPEN_EXISTING;
    DWORD attributes = FILE_ATTRIBUTE_NORMAL;

    const TCHAR* win32_filepath = AllocatePath(filepath);
    file_handle.native_handle = CreateFile(win32_filepath, access_flags, share_mode, NULL, create_mode, attributes, NULL);
    ReleasePath(win32_filepath);

    if (file_handle.native_handle == INVALID_HANDLE_VALUE)
    {
        m_last_error_code = EFilesystemError::FileNotFound;
        return file_handle;
    }

    m_last_error_code = EFilesystemError::Success;
    return file_handle;
}

FileHandle WindowsFilesystem::OpenForWriting(StringView filepath, bool allow_reading_while_open, bool append)
{
    m_last_error_code = EFilesystemError::Unknown;
    FileHandle file_handle = {};

    DWORD access_flags = GENERIC_READ;
    DWORD share_mode = allow_reading_while_open ? FILE_SHARE_READ : 0;
    DWORD create_mode = append ? OPEN_ALWAYS : CREATE_ALWAYS;
    DWORD attributes = FILE_ATTRIBUTE_NORMAL;

    const TCHAR* win32_filepath = AllocatePath(filepath);
    file_handle.native_handle = CreateFile(win32_filepath, access_flags, share_mode, NULL, create_mode, attributes, NULL);
    ReleasePath(win32_filepath);

    if (file_handle.native_handle == INVALID_HANDLE_VALUE)
    {
        m_last_error_code = EFilesystemError::FileNotFound;
        return file_handle;
    }

    m_last_error_code = EFilesystemError::Success;
    return file_handle;
}

const wchar_t* WindowsFilesystem::AllocatePath(StringView filepath) const
{
    // NOTE(traian): The memory is not deallocated by the Buffer that stores it.
    Buffer win32_path_buffer = Buffer(filepath.BytesCount() * 2);
    Usize bytes_count = StringBuilder::DynamicToUTF16(filepath, win32_path_buffer);

    if (bytes_count == win32_path_buffer.size)
    {
        win32_path_buffer.Resize(bytes_count + sizeof(wchar_t));
    }

    win32_path_buffer.As<wchar_t>()[bytes_count / sizeof(wchar_t)] = 0;
    return win32_path_buffer.As<const wchar_t>();
}

void WindowsFilesystem::ReleasePath(const wchar_t* filepath) const
{
    // TODO(traian): Because the path is allocated using the Buffer API, this deallocation
    //               will not be properly tracked by the memory profiler. Maybe `AllocatePath`
    //               should return a buffer object instead of the raw pointer?
    delete[] filepath;
}

} // namespace Basalt
