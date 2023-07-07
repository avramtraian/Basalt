// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "WindowsFilesystem.h"

#include "Core/Containers/Strings/String.h"
#include "Core/Containers/Strings/StringBuilder.h"
#include "Core/Containers/Strings/Utf16String.h"

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
    return (*file_handle != INVALID_HANDLE_VALUE);
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
    file_handle.native_handle = CreateFileW(win32_filepath, access_flags, share_mode, NULL, create_mode, attributes, NULL);
    ReleasePath(win32_filepath);

    if (*file_handle == INVALID_HANDLE_VALUE)
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

    Usize bytes_count;
    wchar_t* win32_filepath = AllocatePath(filepath, &bytes_count);

    Usize offset = 0;
    while (offset < bytes_count)
    {
        U32 codepoint_width;
        UnicodeCodepoint codepoint = UTF16Calls::BytesToCodepoint(win32_filepath + (offset / 2), &codepoint_width);
        Check(codepoint_width > 0); // Invalid UTF-16.

        if (codepoint == '/')
        {
            Check(codepoint_width == 2);
            char character = win32_filepath[offset / 2];

            win32_filepath[offset / 2] = 0;
            CreateDirectory(win32_filepath, NULL);
            win32_filepath[offset / 2] = character;
        }
        else if (codepoint == '.')
        {
            // The path is now a file, not a directory.
            break;
        }

        offset += codepoint_width;
    }

    file_handle.native_handle = CreateFileW(win32_filepath, access_flags, share_mode, NULL, create_mode, attributes, NULL);
    ReleasePath(win32_filepath);

    if (*file_handle == INVALID_HANDLE_VALUE)
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

void WindowsFilesystem::CloseFileHandle(FileHandle& file_handle)
{
    if (!IsFileHandleValid(file_handle))
    {
        // If the file handle is invalid, no action should be performed.
        return;
    }

    CloseHandle(*file_handle);
    file_handle.native_handle = INVALID_HANDLE_VALUE;
    m_last_error_code = EFilesystemError::Success;
}

U64 WindowsFilesystem::ReadFromFile(FileHandle file_handle, void* buffer, U64 bytes_count_to_read, U64 file_offset)
{
    if (!IsFileHandleValid(file_handle))
    {
        m_last_error_code = EFilesystemError::InvalidFileHandle;
        return -1;
    }

    if (file_offset >= file_handle.file_size)
    {
        m_last_error_code = EFilesystemError::Success;
        return 0;
    }

    constexpr U64 MaxReadCount = 0xFFFFFFFF;

    U64 total_bytes_to_read = Math::Min(file_handle.file_size - file_offset, bytes_count_to_read);
    if (total_bytes_to_read <= MaxReadCount)
    {
        // The read operation can be performed with a single call to the Win32 API.

        OVERLAPPED overlapped = {};
        overlapped.Offset = file_offset & LOW_32BIT;
        overlapped.OffsetHigh = file_offset >> 32;

        DWORD read;
        BOOL result = ReadFile(*file_handle, buffer, (DWORD)total_bytes_to_read, &read, &overlapped);
        Check(read == total_bytes_to_read);

        if (!result)
        {
            m_last_error_code = EFilesystemError::Unknown;
            return -1;
        }
    }
    else
    {
        U64 offset = 0;
        while (offset < total_bytes_to_read)
        {
            OVERLAPPED overlapped = {};
            overlapped.Offset = (file_offset + offset) & LOW_32BIT;
            overlapped.OffsetHigh = (file_offset + offset) >> 32;

            U64 bytes_to_read = Math::Min(total_bytes_to_read - offset, MaxReadCount);
            DWORD read;
            BOOL result = ReadFile(*file_handle, buffer, (DWORD)total_bytes_to_read, &read, &overlapped);
            offset += read;

            if (!result)
            {
                m_last_error_code = EFilesystemError::Unknown;
                return -1;
            }
        }
    }

    m_last_error_code = EFilesystemError::Success;
    return total_bytes_to_read;
}

U64 WindowsFilesystem::WriteToFile(FileHandle file_handle, const void* buffer, U64 bytes_count)
{
    Check(bytes_count > 0);

    if (!IsFileHandleValid(file_handle))
    {
        m_last_error_code = EFilesystemError::InvalidFileHandle;
        return 0;
    }

    constexpr U64 MaxWriteCount = 0xFFFFFFFF;

    U64 offset = 0;
    while (offset < bytes_count)
    {
        DWORD bytes_to_write = (DWORD)Math::Min(bytes_count - offset, MaxWriteCount);
        DWORD bytes_written;
        BOOL result = WriteFile(*file_handle, (const U8*)buffer + offset, bytes_to_write, &bytes_written, NULL);
        offset += bytes_written;

        if (!result)
        {
            // TODO(traian): Set the proper error code.
            m_last_error_code = EFilesystemError::Unknown;
            return 0;
        }
    }

    m_last_error_code = EFilesystemError::Success;
    return bytes_count;
}

void WindowsFilesystem::IterateDirectory(const String& directory_path, DirectoryVisitor& visitor)
{
    WIN32_FIND_DATA find_data;

    String wildcard = directory_path + "\\*"sv;
    const wchar_t* win32_path = AllocatePath(wildcard.ToView());
    HANDLE find_handle = FindFirstFile(win32_path, &find_data);
    ReleasePath(win32_path);

    if (find_handle == INVALID_HANDLE_VALUE)
    {
        // Failed to open the find handle.
        m_last_error_code = EFilesystemError::Unknown;
        return;
    }
    
    do
    {
        Buffer filepath_buffer;
        Usize bytes_count = StringBuilder::FromUTF16Dynamic(find_data.cFileName, filepath_buffer, false);
        String filepath = directory_path + "/"sv + StringView(filepath_buffer.As<const char>(), bytes_count);

        if (!filepath.EndsWith("/."sv) && !filepath.EndsWith("/.."sv))
        {
            const bool is_directory = (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
            if (visitor.Visit(filepath, is_directory) == IterationDecision::Break)
            {
                break;
            }
        }
    }
    while (FindNextFile(find_handle, &find_data) != 0);

    FindClose(find_handle);
    m_last_error_code = EFilesystemError::Success;
}

wchar_t* WindowsFilesystem::AllocatePath(StringView filepath, Usize* out_bytes_count /*= nullptr*/) const
{
    Buffer utf16_buffer = Buffer(m_filepath_buffer, sizeof(m_filepath_buffer));
    Usize written_count = StringBuilder::ToUTF16(filepath, utf16_buffer, true);
    wchar_t* win32_filepath = utf16_buffer.As<wchar_t>();

    if (written_count == InvalidSize)
    {
        // The stack buffer is not sufficient to store the filepath.
        Buffer heap_buffer;
        written_count = StringBuilder::ToUTF16Dynamic(filepath, heap_buffer, true);
        win32_filepath = heap_buffer.As<wchar_t>();
    }

    if (out_bytes_count)
    {
        *out_bytes_count = written_count;
    }
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
