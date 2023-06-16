// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "Filesystem.h"

#if BASALT_PLATFORM_WINDOWS
    #include "Windows/WindowsFilesystem.h"
#endif // BASALT_PLATFORM_WINDOWS

namespace Basalt
{

Unique<IFilesystem> IFilesystem::Create(const FilesystemDescription& description)
{
    if (description.type == EFilesystemType::Physical)
    {
#if BASALT_PLATFORM_WINDOWS
        return Unique<WindowsFilesystem>::Create(description).As<IFilesystem>();
#endif // BASALT_PLATFORM_WINDOWS
    }
    else if (description.type == EFilesystemType::Virtual)
    {
        Check(false); // The engine currently doesn't support virtual filesystems.
        return {};
    }

    Check(false); // Invalid filesystem type.
    return {};
}

} // namespace Basalt
