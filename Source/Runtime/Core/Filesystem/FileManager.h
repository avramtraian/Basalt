// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/CoreDefines.h"
#include "Core/Containers/Strings/StringView.h"
#include "Core/CoreTypes.h"
#include "Core/Memory/StackBuffer.h"
#include "Core/Misc/DateTime.h"
#include "Core/Misc/IterationDecision.h"
#include "Filesystem.h"

namespace Basalt
{

/**
 * Enumeration of the errors that can occur while working with the file API.
 */
enum class EFileError
{
    /** The operation was successful. */
    Success = 0,
    /** An unknown error occurred. */
    Unknown = 0,

    /** The file located at the given path wasn't found in the filesystem. */
    FileNotFound,
    /** No filesystem was initialized at the time of calling the function. */
    NoFilesystem,
    /** The file reader or writer was already initialized. */
    AlreadyInitialized,
    /** The internal/provided file handle is not valid. */
    InvalidFileHandle,

    MaxEnumValue
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////// FILE READING API ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Flags that describe how the file should be read from.
 * Each flag represents a bit, so the flags can be | together.
 */
struct EFileRead
{
    enum Type : U32
    {
        /** No flags specified. */
        None = 0,
        /** Allows other processes to write to this file while it is opened for reading by this process. */
        AllowWritingWhileOpen   = BIT(0),
    };
};

/**
 * Reads data from a file.
 */
class BASALT_API FileReader
{
public:
    FileReader() = default;
    ~FileReader();

public:
    /** @return True if the reader is valid and is ready to read; False otherwise. */
    FORCEINLINE bool IsValid() const { return m_is_valid; }

    /** @return True if the reader is valid and is ready to read; False otherwise. */
    FORCEINLINE operator bool() const { return m_is_valid; }

public:
    EFileError ReadBytes(void* buffer, Usize bytes_count_to_read, Usize* read_bytes_count);

    template<typename T>
    FORCEINLINE EFileError Read(T* destination_object_instance)
    {
        return ReadBytes(destination_object_instance, sizeof(T), nullptr);
    }

    template<typename T>
    FORCEINLINE EFileError ReadArray(T* destination_array, Usize count)
    {
        return ReadBytes(destination_array, count * sizeof(T), nullptr);
    }

    FORCEINLINE EFileError ReadAll(Buffer& buffer)
    {
        buffer.Resize(m_file_handle.file_size);
        return ReadBytes(buffer.data, m_file_handle.file_size, nullptr);
    }

    FORCEINLINE EFileError ReadAllAsString(Buffer& buffer)
    {
        // Resize the buffer to accommodate the file data and the null-termination character.
        buffer.Resize(m_file_handle.file_size + sizeof(char));

        // Read the file as a string.
        EFileError result = ReadBytes(buffer.data, m_file_handle.file_size, nullptr);
        if (result != EFileError::Success)
        {
            return result;
        }

        // Set the null-termination character.
        buffer.data[buffer.size - 1] = 0;
        return result;
    }

    void Close();

private:
    FileHandle m_file_handle = {};

    String m_path;

    bool m_is_valid = false;

    U64 m_file_offset = 0;

private:
    friend class FileManager;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////// FILE WRITING API ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Flags that describe how the file should be written to.
 * Each flag represents a bit, so the flags can be | together.
 */
struct EFileWrite
{
    enum Type : U32
    {
        /** No flags specified. */
        None = 0,

        /** If the file doesn't exist, it will be created */
        AlwaysOpen              = BIT(0),
        /** Allows other processes to read this file while it is opened for writing by this process. */
        AllowReadingWhileOpen   = BIT(1),
        /** Append the data to the previously existing data. */
        Append                  = BIT(2),
    };
};

/**
 * Writes data to a file.
 */
class BASALT_API FileWriter
{
public:
    FileWriter() = default;
    ~FileWriter();

public:
    /** @return True if the reader is valid and is ready to read; False otherwise. */
    FORCEINLINE bool IsValid() const { return m_is_valid; }

    /** @return True if the reader is valid and is ready to read; False otherwise. */
    FORCEINLINE operator bool() const { return m_is_valid; }

public:
    EFileError WriteBytes(const void* buffer, U64 bytes_count);
    
    template<typename T>
    FORCEINLINE EFileError Write(const T& object_instance)
    {
        return WriteBytes(&object_instance, sizeof (T));
    }

    template<typename T>
    FORCEINLINE EFileError WriteArray(const T* elements, Usize count)
    {
        return WriteBytes(elements, count * sizeof(T));
    }

    FORCEINLINE EFileError Write(StringView utf8_string)
    {
        return WriteBytes(utf8_string.Data(), utf8_string.BytesCount());
    }

    void Flush();

    void Close();

private:
    FileHandle m_file_handle = {};

    String m_path;

    bool m_is_valid = false;

    StackBuffer m_buffer;

private:
    friend class FileManager;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// FILE INFORMATION API /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Structure that contains the information obtained by reading the file properties.
 */
struct BASALT_S_API FileInformation
{
    /** The date when the file or directory was originally created, or `InvalidDate` if unknown. */
    DateTime creation_time;

    /** The date when the file or directory was last accessed, or `InvalidDate` if unknown. */
    DateTime access_time;

    /** The date when the file or directory was last modified, or `InvalidDate` if unknown. */
    DateTime modification_time;

    /** The size (in bytes) of the file, or -1 if unknown. */
    U64 file_size;

    /** Whether or not the file is actually a directory. */
    bool is_directory;

    /** Whether or not the file or directory is read-only. */
    bool is_readonly;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////// FILE MANAGER API ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Base class for all directory visitors.
 */
class BASALT_API DirectoryVisitor
{
public:
    DirectoryVisitor() = default;
    virtual ~DirectoryVisitor() = default;

    /**
     * Callback that will be invoked for every file or directory in a directory iteration.
     * 
     * @param filepath The path of the file or directory that is iterated over.
     * @param is_directory Whether or not the `filepath` represents a directory.
     * 
     * @return Whether or not the directory iteration should continue.
     */
    virtual IterationDecision Visit(NullStringView filepath, bool is_directory) = 0;
};

/**
 * Structure that describes the file manager.
 */
struct FileManagerDescription
{
};

/**
 * The file manager class is responsible for management of the filesystem and file I/O.
 */
class BASALT_API FileManager
{
public:
    using PFN_DirectoryVisit = IterationDecision(*)(NullStringView, bool is_directory);

public:
    static bool Initialize(const FileManagerDescription& description);

    static void Shutdown();

    static bool IsInitialized();

public:
    /**
     * Opens a file for reading and creates a file reader which can be used to read from it.
     *
     * @param out_reader Pointer to the file reader to initialize. Must not be a valid file reader.
     * @param filepath The path to the file to open.
     * @param read_flags Bit combination of `EFileRead` flags.
     *
     * @return The operation returned error code.
     */
    static EFileError CreateReader(FileReader* out_reader, NullStringView filepath, U32 read_flags = EFileRead::None);

    /**
     * Opens a file for writing and creates a file writer which can be used to write to it.
     * 
     * @param out_writer Pointer to the file writer to initialize. Must not be a valid file writer.
     * @param filepath The path to the file to open.
     * @param write_flags Bit combination of `EFileWrite` flags.
     * 
     * @return The operation returned error code.
     */
    static EFileError CreateWriter(FileWriter* out_writer, NullStringView filepath, U32 write_flags = EFileWrite::None);

public:
    /**
     * Invokes the provided function for every file or directory present in the provided directory. This function
     * doesn't explore the subdirectories.
     * 
     * @param directory_path The path to the directory to iterate over.
     * @param visitor The function to invoke for each file or directory.
     * 
     * @return The operation error code. Can only be `FileNotFound` if the given directory path is not valid
     *         or it isn't a directory.
     */
    static EFileError IterateDirectory(NullStringView directory_path, PFN_DirectoryVisit visitor);

    /**
     * Calls the provided directory visitor for every file or directory present in the provided directory. This function
     * doesn't explore the subdirectories.
     *
     * @param directory_path The path to the directory to iterate over.
     * @param visitor The visitor to call for each file or directory.
     *
     * @return The operation error code. Can only be `FileNotFound` if the given directory path is not valid
     *         or it isn't a directory.
     */
    static EFileError IterateDirectory(NullStringView directory_path, DirectoryVisitor& visitor);

    /**
     * Invokes the provided function for every file or directory present in the provided directory, recursively
     * This function explores the subdirectories.
     *
     * @param directory_path The path to the directory to iterate over.
     * @param visitor The function to invoke for each file or directory.
     *
     * @return The operation error code. Can only be `FileNotFound` if the given directory path is not valid
     *         or it isn't a directory.
     */
    static EFileError IterateDirectoryRecursively(NullStringView directory_path, PFN_DirectoryVisit visitor);

    /**
     * Calls the provided directory visitor for every file or directory present in the provided directory, recursively
     * This function explores the subdirectories.
     *
     * @param directory_path The path to the directory to iterate over.
     * @param visitor The visitor to call for each file or directory.
     *
     * @return The operation error code. Can only be `FileNotFound` if the given directory path is not valid
     *         or it isn't a directory.
     */
    static EFileError IterateDirectoryRecursively(NullStringView directory_path, DirectoryVisitor& visitor);
};

} // namespace Basalt
