-- Copyright (c) 2023 Basalt Technology Group. All rights reserved.

group "Core"
project "Basalt-Runtime"
    location "%{wks.location}Source/Runtime"
    BuildDefaultConfiguration()

    kind "SharedLib"
    language "C++"
    cppdialect "C++17"

    staticruntime "Off"
    rtti "Off"
    exceptionhandling "Off"

    files
    {
        "%{prj.location}/**.h",
        "%{prj.location}/**.cpp",
        "%{prj.location}/**.inl"
    }

    defines
    {
        "BASALT_EXPORT_API"
    }

    includedirs
    {
        IncludeDirectories["Runtime"],
        IncludeDirectories["VulkanSDK"]
    }

    libdirs
    {
        LibraryDirectories["VulkanSDK"]
    }

    filter "platforms:Win64"
        links
        {
            "d3d11",
            "dxgi"
        }

    filter {}

    filter "configurations:Editor_Debug"
        links
        {
            Libraries["DXC"],
            Libraries["SPIRV_Cross_Debug"],
            Libraries["SPIRV_Cross_MSL_Debug"]
        }

    filter "configurations:Editor_Release"
        links
        {
            Libraries["DXC"],
            Libraries["SPIRV_Cross"],
            Libraries["SPIRV_Cross_MSL"]
        }

    filter {}
