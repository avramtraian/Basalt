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
    const wchar_t* win32_filepath = AllocatePath(filepath);

    WIN32_FILE_ATTRIBUTE_DATA attributes = {};
    BOOL result = GetFileAttributesEx(win32_filepath, GetFileExInfoStandard, &attributes);
    ReleasePath(win32_filepath);
    
    if (!result)
    {
        m_last_error_code = EFilesystemError::FileNotFound;
        return -1;
    }

    if (attributes.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        m_last_error_code = EFilesystemError::FileIsDirectory;
        return -1;
    }

    m_last_error_code = EFilesystemError::Success;
    return (U64)attributes.nFileSizeLow | ((U64)attributes.nFileSizeHigh << 32);
}

FileHandle WindowsFilesystem::OpenForReading(StringView filepath, bool allow_writing_while_open)
{
    FileHandle file_handle;

    DWORD access_flags = GENERIC_READ;
    DWORD share_mode = FILE_SHARE_READ | (allow_writing_while_open ? FILE_SHARE_WRITE : 0);
    DWORD create_mode = OPEN_EXISTING;
    DWORD attributes = FILE_ATTRIBUTE_NORMAL;

    const wchar_t* win32_filepath = AllocatePath(filepath);
    file_handle.native_handle = CreateFile(win32_filepath, access_flags, share_mode, NULL, create_mode, attributes, NULL);
    ReleasePath(win32_filepath);

    if (file_handle.native_handle == INVALID_HANDLE_VALUE)
    {
        m_last_error_code = EFilesystemError::FileNotFound;
        return file_handle;
    }

    LARGE_INTEGER file_size;
    GetFileSizeEx(*file_handle, &file_size);
    file_handle.file_size = file_size.QuadPart;

    m_last_error_code = EFilesystemError::Success;
    return file_handle;
}

FileHandle WindowsFilesystem::OpenForWriting(StringView filepath, bool allow_reading_while_open, bool append)
{
    FileHandle file_handle;

    DWORD access_flags = GENERIC_WRITE;
    DWORD share_mode = allow_reading_while_open ? FILE_SHARE_READ : 0;
    DWORD create_mode = append ? OPEN_ALWAYS : CREATE_ALWAYS;
    DWORD attributes = FILE_ATTRIBUTE_NORMAL;

    const wchar_t* win32_filepath = AllocatePath(filepath);
    file_handle.native_handle = CreateFile(win32_filepath, access_flags, share_mode, NULL, create_mode, attributes, NULL);
    ReleasePath(win32_filepath);

    if (file_handle.native_handle == INVALID_HANDLE_VALUE)
    {
        m_last_error_code = EFilesystemError::FileNotFound;
        return file_handle;
    }

    LARGE_INTEGER file_size;
    GetFileSizeEx(*file_handle, &file_size);
    file_handle.file_size = file_size.QuadPart;

    m_last_error_code = EFilesystemError::Success;
    return file_handle;
}

const wchar_t* WindowsFilesystem::AllocatePath(StringView filepath) const
{
    Buffer utf16_buffer = Buffer(m_filepath_buffer, sizeof(m_filepath_buffer) - sizeof(wchar_t));
    Usize written_count = StringBuilder::ToUTF16(filepath, utf16_buffer);
    wchar_t* win32_filepath = utf16_buffer.As<wchar_t>();

    if (written_count == InvalidSize)
    {
        // The stack buffer is not sufficient to store the filepath.
        Buffer heap_buffer;
        written_count = StringBuilder::DynamicToUTF16(filepath, heap_buffer);
        win32_filepath = heap_buffer.As<wchar_t>();

        if (heap_buffer.size == written_count)
        {
            heap_buffer.Resize(heap_buffer.size + sizeof(wchar_t));
        }
    }

    win32_filepath[written_count / 2] = 0;
    return win32_filepath;
}

void WindowsFilesystem::ReleasePath(const wchar_t* filepath) const
{
    if (filepath != &m_filepath_buffer[0])
    {
        // TODO(traian): Because the path is allocated using the Buffer API, this deallocation
        //               will not be properly tracked by the memory profiler. Maybe `AllocatePath`
        //               should return a buffer object instead of the raw pointer?
        btdelete[] filepath;
    }
}

} // namespace Basalt
