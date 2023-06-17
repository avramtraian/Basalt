// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "FileManager.h"

#include "Core/Logging/LogMacros.h"

namespace Basalt
{

struct FileManagerData
{
    Unique<IFilesystem> filesystem;
};

static FileManagerData* s_file_manager = nullptr;

bool FileManager::Initialize(const FileManagerDescription& description)
{
    if (s_file_manager)
    {
        // The file manager was already initialized.
        return false;
    }

    s_file_manager = btnew FileManagerData();
    if (!s_file_manager)
    {
        // Failed to allocate memory for the file manager data.
        return false;
    }

    FilesystemDescription filesystem_description = {};
    filesystem_description.type = EFilesystemType::Physical;

    s_file_manager->filesystem = IFilesystem::Create(filesystem_description);
    if (!s_file_manager->filesystem)
    {
        // Failed to allocate memory for the filesystem.
        return false;
    }

    if (!s_file_manager->filesystem->Initialize())
    {
        // Failed to initialize the filesystem.
        return false;
    }

    Check(FileManager::IsInitialized());
    return true;
}

void FileManager::Shutdown()
{
    if (!s_file_manager)
    {
        // The file manager was already shut down.
        return;
    }
    
    s_file_manager->filesystem->Shutdown();
    // Release the internal filesystem.
    s_file_manager->filesystem.Release();

    btdelete s_file_manager;
    s_file_manager = nullptr;
}

bool FileManager::IsInitialized()
{
    return s_file_manager != nullptr                    &&
           s_file_manager->filesystem.IsValid()         &&
           s_file_manager->filesystem->IsInitialized();
}

EFileError FileManager::CreateReader(FileReader* out_reader, StringView filepath, U32 read_flags /*= EFileRead::None*/)
{
    if (!FileManager::IsInitialized())
    {
        return EFileError::NoFilesystem;
    }

    if (out_reader->IsValid())
    {
        return EFileError::AlreadyInitialized;
    }

    FileHandle handle = s_file_manager->filesystem->OpenForReading(filepath, read_flags & EFileRead::AllowWritingWhileOpen);
    if (!s_file_manager->filesystem->IsFileHandleValid(handle))
    {
        EFilesystemError error_code = s_file_manager->filesystem->GetLastErrorCode();
        return EFileError::FileNotFound;
    }

    out_reader->m_file_handle = handle;
    out_reader->m_is_valid = true;
    out_reader->m_path = filepath;

    Check(out_reader->IsValid());
    return EFileError::Success;
}

EFileError FileManager::CreateWriter(FileWriter* out_writer, StringView filepath, U32 write_flags /*= EFileWrite::None*/)
{
    if (!FileManager::IsInitialized())
    {
        return EFileError::NoFilesystem;
    }

    if (out_writer->IsValid())
    {
        return EFileError::AlreadyInitialized;
    }

    FileHandle handle = s_file_manager->filesystem->OpenForWriting(filepath, write_flags & EFileWrite::AllowReadingWhileOpen, write_flags & EFileWrite::Append);
    if (!s_file_manager->filesystem->IsFileHandleValid(handle))
    {
        EFilesystemError error_code = s_file_manager->filesystem->GetLastErrorCode();
        return EFileError::FileNotFound;
    }

    out_writer->m_file_handle = handle;
    out_writer->m_is_valid = true;
    out_writer->m_path = filepath;
    
    Check(out_writer->IsValid());
    return EFileError::Success;
}

EFileError FileManager::IterateDirectory(StringView directory_path, DirectoryVisitor& visitor)
{
    return EFileError::Success;
}

EFileError FileManager::IterateDirectory(StringView directory_path, PFN_DirectoryVisit visitor)
{
    class WrapperDirectoryVisitor : public DirectoryVisitor
    {
    public:
        WrapperDirectoryVisitor(PFN_DirectoryVisit visitor)
            : m_visitor(visitor)
        {}

        virtual ~WrapperDirectoryVisitor() override = default;

        virtual IterationDecision Visit(StringView filepath, bool is_directory) override
        {
            return m_visitor(filepath, is_directory);
        }

    public:
        PFN_DirectoryVisit m_visitor;
    };

    WrapperDirectoryVisitor wrapper_visitor(visitor);
    return IterateDirectory(directory_path, wrapper_visitor);
}

EFileError FileManager::IterateDirectoryRecursively(StringView directory_path, DirectoryVisitor& visitor)
{
    class RecursiveDirectoryVisitor : public DirectoryVisitor
    {
    public:
        RecursiveDirectoryVisitor(DirectoryVisitor* visitor)
            : m_visitor(visitor)
        {}

        virtual ~RecursiveDirectoryVisitor() override = default;

        virtual IterationDecision Visit(StringView filepath, bool is_directory) override
        {
            if (is_directory)
            {
                EFileError error = IterateDirectory(filepath, *m_visitor);
                Check(error == EFileError::Success); // What?
            }

            return m_visitor->Visit(filepath, is_directory);
        }

    public:
        DirectoryVisitor* m_visitor;
    };

    RecursiveDirectoryVisitor recursive_visitor(&visitor);
    return IterateDirectory(directory_path, recursive_visitor);
}

EFileError FileManager::IterateDirectoryRecursively(StringView directory_path, PFN_DirectoryVisit visitor)
{
    class WrapperDirectoryVisitor : public DirectoryVisitor
    {
    public:
        WrapperDirectoryVisitor(PFN_DirectoryVisit visitor)
            : m_visitor(visitor)
        {}

        virtual ~WrapperDirectoryVisitor() override = default;

        virtual IterationDecision Visit(StringView filepath, bool is_directory) override
        {
            return m_visitor(filepath, is_directory);
        }

    public:
        PFN_DirectoryVisit m_visitor;
    };

    WrapperDirectoryVisitor wrapper_visitor(visitor);
    return IterateDirectoryRecursively(directory_path, wrapper_visitor);
}

FileReader::~FileReader()
{
    Close();
}

void FileReader::Close()
{
    if (!m_is_valid)
    {
        return;
    }

    m_is_valid = false;
}

FileWriter::~FileWriter()
{
    Close();
}

void FileWriter::Close()
{
    if (!m_is_valid)
    {
        return;
    }

    m_is_valid = false;
}

} // namespace Basalt
