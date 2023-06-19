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
        IncludeDirectories["Editor"]
    }
