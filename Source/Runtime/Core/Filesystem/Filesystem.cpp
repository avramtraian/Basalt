// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "Filesystem.h"

namespace Basalt
{

Unique<IFilesystem> IFilesystem::Create(const FilesystemDescription& description)
{
    return {};
}

} // namespace Basalt
