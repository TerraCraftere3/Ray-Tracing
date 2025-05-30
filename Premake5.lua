workspace "Ray-Tracer"
    architecture "x64"
    configurations { "Debug", "Release", "Dist" }

project "Editor"
    location "Editor"
    kind "ConsoleApp"
    architecture "x64"
    language "C++"
    cppdialect "C++17"
    targetdir ("bin/%{cfg.buildcfg}-%{cfg.architecture}")
    objdir ("bin-int/%{cfg.buildcfg}-%{cfg.architecture}")
    files { "Editor/src/**.h", "Editor/src/**.cpp" }
    includedirs { "Editor/src" }

    defines { "GLM_ENABLE_EXPERIMENTAL"}

    filter "system:windows"
        defines { "PLATFORM_WINDOWS" }

    filter "system:linux"
        defines { "PLATFORM_LINUX" }

    filter "configurations:Debug"
        defines { "CONFIGURATION_DEBUG" }
        symbols "On"
        optimize "Off"

    filter "configurations:Release"
        defines { "CONFIGURATION_RELEASE" }
        optimize "On"
        symbols "On"

    filter "configurations:Dist"
        defines { "CONFIGURATION_DIST" }
        optimize "Full"
        symbols "Off"
