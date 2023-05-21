-- Copyright (c) 2023 Basalt Technology Group. All rights reserved.

group "Tools"
project "Basalt-Editor"
    location "%{wks.location}Source/Editor"
    BuildDefaultConfiguration()

    kind "WindowedApp"
    language "C++"
    cppdialect "C++17"

    staticruntime "Off"
    rtti "Off"
    exceptionhandling "Off"

    links
    {
        "Basalt-Runtime"
    }

    files
    {
        "%{prj.location}/**.h",
        "%{prj.location}/**.cpp"
    }

    includedirs
    {
        IncludeDirectories["Runtime"],
        IncludeDirectories["Editor"]
    }
