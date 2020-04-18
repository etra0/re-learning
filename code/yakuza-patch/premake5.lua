workspace "noSprint"
    platforms { "Win64" }

project "noSprint"
    kind "SharedLib"
    targetextension ".asi"
    language "C++"

    files { "**/MemoryMgr.h", "**/Patterns.*", "**/HookInit.hpp" }


workspace "*"
    configurations { "Debug", "Release", "Master" }
    location "build"

    vpaths { ["Headers/*"] = "./**.h",
            ["Sources/*"] = { "./**.c", "./**.cpp" }
    }

    files { "./*.h", "./*.cpp", "./re.s/*.rc" }

    cppdialect "C++17"
    staticruntime "on"
    buildoptions { "/sdl" }
    warnings "Extra"

    -- Automated defines for resources
    defines { "rsc_Extension=\"%{prj.targetextension}\"",
            "rsc_Name=\"%{prj.name}\"" }

filter "configurations:Debug"
    defines { "DEBUG" }
    runtime "Debug"

 filter "configurations:Master"
    defines { "NDEBUG" }
    symbols "Off"

filter "configurations:not Debug"
    optimize "Speed"
    functionlevellinking "on"
    flags { "LinkTimeOptimization" }

filter { "platforms:Win32" }
    system "Windows"
    architecture "x86"

filter { "platforms:Win64" }
    system "Windows"
    architecture "x86_64"

filter { "toolset:*_xp"}
    buildoptions { "/Zc:threadSafeInit-" }

filter { "toolset:not *_xp"}
    buildoptions { "/permissive-" }