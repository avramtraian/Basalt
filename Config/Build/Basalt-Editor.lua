-- Copyright (c) 2023 Basalt Technology Group. All rights reserved.

group "Tools"
project "Basalt-Editor"
    location "%{wks.location}Source/Editor"
    BuildDefaultConfiguration()

    -- This value will get overwritten in shipping configurations.
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"

    staticruntime "Off"
    rtti "Off"
    exceptionhandling "Off"

    filter "configurations:Game_Shipping"
        kind "WindowedApp"
    filter {}

    links
    {
        "Basalt-Runtime"
    }

    files
    {
        "%{prj.location}/**.h",
        "%{prj.location}/**.cpp",
        "%{prj.location}/**.inl"
    }

    includedirs
    {
        IncludeDirectories["Runtime"],
        IncludeDirectories["Editor"],
        IncludeDirectories["VulkanSDK"]
    }

    libdirs
    {
        LibraryDirectories["VulkanSDK"]
    }

    filter "configurations:Editor_Debug"
        links
        {
            Libraries["DXC"],
            Libraries["SPIRV_Cross_Debug"],
            Libraries["SPIRV_Cross_Reflect_Debug"],
            Libraries["SPIRV_Cross_MSL_Debug"],
            Libraries["SPIRV_Cross_GLSL_Debug"]
        }

    filter "configurations:Editor_Release"
        links
        {
            Libraries["DXC"],
            Libraries["SPIRV_Cross"],
            Libraries["SPIRV_Cross_Reflect"],
            Libraries["SPIRV_Cross_MSL"],
            Libraries["SPIRV_Cross_GLSL"]
        }

    filter {}
