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

local function getPlatformName()
    if os.host() == "windows" then
        return "Windows"
    elseif os.host() == "linux" then
        return "Linux"
    else
        return "Unknown"
    end
end

project "Editor"
    location "Editor"
    kind "ConsoleApp"
    architecture "x64"
    language "C++"
    cppdialect "C++latest"
    files {
        "Editor/src/Core/**.h",          "Editor/src/Core/**.cpp",          "Editor/src/Core/**.c",
        "Editor/src/Rendering/**.h",     "Editor/src/Rendering/**.cpp",     "Editor/src/Rendering/**.c",
        "Editor/src/Main.cpp",

        -- Platform Specific --
        "Editor/src/Platform/Common/*",
        "Editor/src/Platform/Image.h",
        "Editor/src/Platform/ImGui.h",
        "Editor/src/Platform/Input.h",
        "Editor/src/Platform/Platform.h",
        "Editor/src/Platform/stb_image.h",
        "Editor/src/Platform/Window.h",

        -- Libraries --
        "Editor/src/Vendor/glad/**.h",   "Editor/src/Vendor/glad/**.cpp",   "Editor/src/Vendor/glad/**.c",
        "Editor/src/Vendor/glm/**.h",    "Editor/src/Vendor/glm/**.cpp",    "Editor/src/Vendor/glm/**.c",
        "Editor/src/Vendor/spdlog/**.h", "Editor/src/Vendor/spdlog/**.cpp", "Editor/src/Vendor/spdlog/**.c",

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

    local platformName = getPlatformName()

    targetdir ("bin/".. platformName .."-%{cfg.buildcfg}-%{cfg.architecture}")
    debugdir ("bin/".. platformName .."-%{cfg.buildcfg}-%{cfg.architecture}")
    objdir ("bin-int/".. platformName .."-%{cfg.buildcfg}-%{cfg.architecture}")

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
            "Editor/src/vendor/glfw/src/osmesa_context.c",
            "Editor/src/Platform/Windows/**.h", "Editor/src/Platform/Windows/**.cpp", "Editor/src/Platform/Windows/**.c",
        }
        links { "opengl32" }
        postbuildcommands {
            ("{MKDIR} %{cfg.targetdir}"),
            ("xcopy /E /I /Y \"%{wks.location}Editor\\runtime\\*\" \"%{cfg.targetdir}\\\"")
        }

    filter "system:linux"
        defines { "PLATFORM_LINUX" }
        -- GLFW --
        defines { "_GLFW_X11"}
        files {
            "Editor/src/vendor/glfw/src/x11_*.c",
            "Editor/src/vendor/glfw/src/glx_context.c",
            "Editor/src/vendor/glfw/src/posix_*.c",
            "Editor/src/vendor/glfw/src/xkb_unicode.c",
            "Editor/src/vendor/glfw/src/linux_joystick.c",
            "Editor/src/vendor/glfw/src/egl_context.c",
            "Editor/src/vendor/glfw/src/osmesa_context.c",
            "Editor/src/Platform/Linux/**.h", "Editor/src/Platform/Linux/**.cpp", "Editor/src/Platform/Linux/**.c",
        }
        links { "GL", "X11", "pthread", "dl" }
        postbuildcommands {
            ("{MKDIR} %{cfg.targetdir}"),
            ("ls -la \"%{cfg.targetdir}/\"")
        }

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

    filter {}

    -- print("Runtime path: " .. path.getabsolute("%{wks.location}Editor/runtime/"))
    -- print("Target path: " .. path.getabsolute("%{cfg.targetdir}"))

premake.override(premake.vstudio.vc2010.elements, "globals", function(base, prj)
    local calls = base(prj)
    table.insertafter(calls, vs.globals, disableVcpkg)
    return calls
end)
