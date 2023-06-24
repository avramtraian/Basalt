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

    filter "platforms:Win64"
        links { "d3d11", "dxgi" }
    filter {}

    filter "configurations:Game_Debug or Editor_Debug"
         links
         {
             Libaries["ShaderC_Debug"],
             Libaries["SPIRV_Cross_Debug"],
             Libaries["SPIRV_Cross_HLSL_Debug"]
         }

    filter "configurations:Game_Release or Editor_Release or Game_Shipping"
        links
        {
            Libaries["ShaderC"],
            Libaries["SPIRV_Cross"],
             Libaries["SPIRV_Cross_HLSL"]
        }

    filter {}
