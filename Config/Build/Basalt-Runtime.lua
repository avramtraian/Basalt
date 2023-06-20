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
        IncludeDirectories["Runtime"]
    }

    filter "platforms:Win64"
        links "DXGI"
        links "d3d11"
    filter {}
