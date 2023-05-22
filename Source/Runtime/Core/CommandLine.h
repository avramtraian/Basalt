// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

namespace Basalt
{

/**
 * Structure that contains the command line arguments passed
 * by the user when the application is run.
 */
struct BASALT_S_API CommandLineArguments
{
    /** The command line arguments. */
    char** arguments = nullptr;

    /** The number of command line arguments. */
    U32 argumentsCount = 0;
};

} // namespace Basalt
