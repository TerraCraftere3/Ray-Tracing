require('vstudio')
local vs = premake.vstudio.vc2010

workspace "Ray-Tracer"
    architecture "x64"
    configurations { "Debug", "Release", "Dist" }

local function disableVcpkg(prj)
    premake.w('<VcpkgEnabled>false</VcpkgEnabled>')
    premake.w('<VcpkgManifestInstall>false</VcpkgManifestInstall>')
    premake.w('<VcpkgAutoLink>false</VcpkgAutoLink>')
end

project "Editor"
    location "Editor"
    kind "ConsoleApp"
    architecture "x64"
    language "C++"
    cppdialect "C++latest"
    targetdir ("bin/%{cfg.buildcfg}-%{cfg.architecture}")
    objdir ("bin-int/%{cfg.buildcfg}-%{cfg.architecture}")
    files { "Editor/src/Core/**.h", "Editor/src/Core/**.cpp", "Editor/src/Core/**.c",
        "Editor/src/Platform/**.h", "Editor/src/Platform/**.cpp", "Editor/src/Platform/**.c",
        "Editor/src/Rendering/**.h", "Editor/src/Rendering/**.cpp", "Editor/src/Rendering/**.c",
        "Editor/src/Vendor/glad/**.h", "Editor/src/Vendor/glad/**.cpp", "Editor/src/Vendor/glad/**.c",
        "Editor/src/Vendor/glm/**.h", "Editor/src/Vendor/glm/**.cpp", "Editor/src/Vendor/glm/**.c",
        "Editor/src/Vendor/spdlog/**.h", "Editor/src/Vendor/spdlog/**.cpp", "Editor/src/Vendor/spdlog/**.c",
        "Editor/src/Main.cpp",

        -- GLFW --
        "Editor/src/vendor/glfw/src/context.c",
        "Editor/src/vendor/glfw/src/init.c",
        "Editor/src/vendor/glfw/src/input.c",
        "Editor/src/vendor/glfw/src/monitor.c",
        "Editor/src/vendor/glfw/src/vulkan.c",
        "Editor/src/vendor/glfw/src/window.c",
        "Editor/src/vendor/glfw/src/platform.c",
        "Editor/src/vendor/glfw/src/null*",
    }
    includedirs { "Editor/src",
        "Editor/src/vendor",
        "Editor/src/vendor/spdlog/include",
        "Editor/src/vendor/glad/include",
        "Editor/src/vendor/glfw/include"
    }

    defines { "GLM_ENABLE_EXPERIMENTAL", "SPDLOG_COMPILED_LIB"}

    filter "action:vs*"
        buildoptions { "/utf-8" }
        defines { "FMT_UNICODE=0" }

    filter "system:windows"
        defines { "PLATFORM_WINDOWS" }
        -- GLFW --
        defines { "_GLFW_WIN32"}
        files {
            "Editor/src/vendor/glfw/src/win32_*.c",
            "Editor/src/vendor/glfw/src/wgl_context.c",
            "Editor/src/vendor/glfw/src/egl_context.c",
            "Editor/src/vendor/glfw/src/osmesa_context.c"
        }
        links { "opengl32" }

    filter "system:linux"
        defines { "PLATFORM_LINUX" }
        -- GLFW --
        defines { "_GLFW_X11"}
        files {
            "Editor/src/vendor/glfw/src/x11_*.c",
            "Editor/src/vendor/glfw/src/glx_context.c",
            "Editor/src/vendor/glfw/src/posix_*.c"
        }
        links { "GL", "X11", "pthread", "dl" }

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

premake.override(premake.vstudio.vc2010.elements, "globals", function(base, prj)
    local calls = base(prj)
    table.insertafter(calls, vs.globals, disableVcpkg)
    return calls
end)
