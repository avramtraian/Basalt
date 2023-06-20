-- Copyright (c) 2023 Basalt Technology Group. All rights reserved.

group "Tools"
project "Basalt-Content"
    location "%{wks.location}Intermediate/ProjectFiles"
    kind "None"

    files
    {
        -- Build scripts
        "%{wks.location}Config/Build/**.lua",

        -- Shaders
        "%{wks.location}Content/Runtime/Shaders/**.hlsl",
        "%{wks.location}Content/Editor/Shaders/**.hlsl"
    }
