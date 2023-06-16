// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/CoreDefines.h"
#include "Core/CoreTypes.h"
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
    /** The file located at the given path wasn't found in the filesystem. */
    FileNotFound,
    /** No filesystem was initialized at the time of calling the function. */
    NoFilesystem,
    /** The file reader or writer was already initialized. */
    AlreadyInitialized,

    MaxEnumValue
};

/**
 * Flags that describe how the file should be written to.
 */
struct EFileWrite
{
    enum Type : U32
    {
        /** No flags specified. */
        None = 0,

        /** If the file doesn't exist, it will be created */
        AlwaysOpen,
        /** Allows other processes to read this file while it is opened for writing by this process. */
        AllowReadingWhileOpen,
        /** Append the data to the previously existing data. */
        Append,
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
    void Close();

private:
    FileHandle m_file_handle = {};

    String m_path;

    bool m_is_valid = false;

private:
    friend class FileManager;
};

/**
 * Flags that describe how the file should be read from.
 */
struct EFileRead
{
    enum Type : U32
    {
        /** No flags specified. */
        None = 0,
        /** Allows other processes to write to this file while it is opened for reading by this process. */
        AllowWritingWhileOpen,
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
    void Close();

private:
    FileHandle m_file_handle = {};

    String m_path;

    bool m_is_valid = false;

private:
    friend class FileManager;
};

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
    virtual IterationDecision Visit(StringView filepath, bool is_directory) = 0;
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
    using PFN_DirectoryVisit = IterationDecision(*)(StringView, bool is_directory);

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
    static EFileError CreateReader(FileReader* out_reader, StringView filepath, U32 read_flags = EFileRead::None);

    /**
     * Opens a file for writing and creates a file writer which can be used to write to it.
     * 
     * @param out_writer Pointer to the file writer to initialize. Must not be a valid file writer.
     * @param filepath The path to the file to open.
     * @param write_flags Bit combination of `EFileWrite` flags.
     * 
     * @return The operation returned error code.
     */
    static EFileError CreateWriter(FileWriter* out_writer, StringView filepath, U32 write_flags = EFileWrite::None);

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
    static EFileError IterateDirectory(StringView directory_path, PFN_DirectoryVisit visitor);

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
    static EFileError IterateDirectory(StringView directory_path, DirectoryVisitor& visitor);

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
    static EFileError IterateDirectoryRecursively(StringView directory_path, PFN_DirectoryVisit visitor);

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
    static EFileError IterateDirectoryRecursively(StringView directory_path, DirectoryVisitor& visitor);
};

} // namespace Basalt
