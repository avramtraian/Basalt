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
        "BT_EXPORT_API"
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

    links
    {
        Libraries["Vulkan"]
    }

    filter "platforms:Win64"
        links
        {
            "d3d11",
            "d3d12", 
            "dxgi"
        }

    filter {}
