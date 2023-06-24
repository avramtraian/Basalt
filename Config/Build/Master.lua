-- Copyright (c) 2023 Basalt Technology Group. All rights reserved.

workspace "Basalt"
    location "../../"

    configurations
    {
        "Game_Debug", "Game_Release", "Game_Shipping",

        "Editor_Debug", "Editor_Release"
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
Libaries = {}

IncludeDirectories["Runtime"]       = "%{wks.location}Source/Runtime"
IncludeDirectories["Editor"]        = "%{wks.location}Source/Editor"

vulkan_sdk_path = os.getenv("VULKAN_SDK")
IncludeDirectories["VulkanSDK"]     = (vulkan_sdk_path .. "/Include")

Libaries["ShaderC"]                 = (vulkan_sdk_path .. "/Lib/shaderc_shared")
Libaries["ShaderC_Debug"]           = (vulkan_sdk_path .. "/Lib/shaderc_sharedd")
Libaries["SPIRV_Cross"]             = (vulkan_sdk_path .. "/Lib/spirv-cross-core")
Libaries["SPIRV_Cross_Debug"]       = (vulkan_sdk_path .. "/Lib/spirv-cross-cored")
Libaries["SPIRV_Cross_HLSL"]        = (vulkan_sdk_path .. "/Lib/spirv-cross-hlsl")
Libaries["SPIRV_Cross_HLSL_Debug"]  = (vulkan_sdk_path .. "/Lib/spirv-cross-hlsld")

function BuildDefaultConfiguration()
    filter "platforms:Win64"
        systemversion "latest"
        defines "BASALT_PLATFORM_WINDOWS=1"

    filter "platforms:Linux"
        defines "BASALT_PLATFORM_LINUX=1"

    filter "platforms:MacOS"
        defines "BASALT_PLATFORM_MACOS=1"

    filter "configurations:Game_Debug or Editor_Debug"
        runtime "Debug"
        optimize "Off"
        symbols "On"
        defines "BASALT_BUILD_DEBUG=1"

    filter "configurations:Game_Release or Editor_Release"
        runtime "Release"
        optimize "On"
        symbols "On"
        defines "BASALT_BUILD_RELEASE=1"

    filter "configurations:Game_Shipping"
        runtime "Release"
        optimize "Speed"
        symbols "Off"
        defines "BASALT_BUILD_SHIPPING=1"

    filter "configurations:Game_Debug or Game_Release or Game_Shipping"
        defines "BASALT_BUILD_GAME=1"

    filter "configurations:Editor_Debug or Editor_Release"
        defines "BASALT_BUILD_EDITOR=1"

    filter {}

    objdir "%{wks.location}Intermediate"
    targetdir "%{wks.location}Binaries/%{cfg.platform}/%{cfg.buildcfg}"

    flags { "MultiProcessorCompile" }
end

include "Basalt-Runtime"
include "Basalt-Editor"
include "Basalt-Content"
