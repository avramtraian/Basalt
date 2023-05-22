-- Copyright (c) 2023 Basalt Technology Group. All rights reserved.

workspace "Basalt"
    location "../../"

    configurations
    {
        "Debug", "Release", "Shipping"
    }

    platforms
    {
        "Win64", "Linux", "MacOS"
    }

    startproject "Basalt-Editor"

    filter "platforms:Win64"
        system "windows"
        architecture "x86_64"

    filter "platforms:Linux"
        system "linux"
        architecture "x86_64"

    filter "platforms:MacOS"
        system "macosx"
        architecture "ARM64"

IncludeDirectories = {}
IncludeDirectories["Runtime"] = "%{wks.location}Source/Runtime"
IncludeDirectories["Editor"] = "%{wks.location}Source/Editor"

function BuildDefaultConfiguration()
    filter "platforms:Win64"
        systemversion "latest"
        defines "BASALT_PLATFORM_WINDOWS=1"

    filter "platforms:Linux"
        defines "BASALT_PLATFORM_LINUX=1"

    filter "platforms:MacOS"
        defines "BASALT_PLATFORM_MACOS=1"

    filter "configurations:Debug"
        runtime "Debug"
        optimize "Off"
        symbols "On"
        defines "BASALT_BUILD_DEBUG=1"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"
        symbols "On"
        defines "BASALT_BUILD_RELEASE=1"

    filter "configurations:Shipping"
        runtime "Release"
        optimize "Speed"
        symbols "Off"
        defines "BASALT_BUILD_SHIPPING=1"

    filter {}

    objdir "%{wks.location}Intermediate"
    targetdir "%{wks.location}Binaries/%{cfg.platform}"
end

include "Basalt-Runtime"
include "Basalt-Editor"
