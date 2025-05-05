include "lzma/."
include "spmemvfs/."

project "YGOPro"
    kind "WindowedApp"
    cppdialect "C++14"
    rtti "Off"
    openmp "On"

    files { "*.cpp", "*.h" }
    includedirs { "../ocgcore" }
    links { "ocgcore", "clzma", "cspmemvfs", LUA_LIB_NAME, "sqlite3", "irrlicht", "freetype", "event" }

    if BUILD_IKPMP3 then
        links { "ikpmp3" }
    end

    if BUILD_EVENT then
        includedirs { "../event/include" }
    else
        includedirs { EVENT_INCLUDE_DIR }
        libdirs { EVENT_LIB_DIR }
    end

    if BUILD_IRRLICHT then
        includedirs { "../irrlicht/include" }
    else
        includedirs { IRRLICHT_INCLUDE_DIR }
        libdirs { IRRLICHT_LIB_DIR }
    end

    if BUILD_FREETYPE then
        includedirs { "../freetype/include" }
    else
        includedirs { FREETYPE_INCLUDE_DIR }
        libdirs { FREETYPE_LIB_DIR }
    end

    if BUILD_SQLITE then
        includedirs { "../sqlite3" }
    else
        includedirs { SQLITE_INCLUDE_DIR }
        libdirs { SQLITE_LIB_DIR }
    end

    if USE_IRRKLANG then
        defines { "YGOPRO_USE_IRRKLANG" }
        includedirs { IRRKLANG_INCLUDE_DIR }
        if not IRRKLANG_PRO then
            libdirs { IRRKLANG_LIB_DIR }
        end
    end

    filter "system:windows"
        defines { "_IRR_WCHAR_FILESYSTEM" }
        files "ygopro.rc"
        if USE_IRRKLANG then
            links { "irrKlang" }
            if IRRKLANG_PRO then
                defines { "IRRKLANG_STATIC" }
                filter { "not configurations:Debug" }
                    libdirs { IRRKLANG_PRO_RELEASE_LIB_DIR }
                filter { "configurations:Debug" }
                    libdirs { IRRKLANG_PRO_DEBUG_LIB_DIR }
                filter {}
            end
        end
        links { "opengl32", "ws2_32", "winmm", "gdi32", "kernel32", "user32", "imm32" }
    filter "not system:windows"
        links { "event_pthreads", "dl", "pthread" }
    filter "system:macosx"
        openmp "Off"
        links { "z" }
        defines { "GL_SILENCE_DEPRECATION" }
        if MAC_ARM then
            buildoptions { "--target=arm64-apple-macos12" }
            linkoptions { "-arch arm64" }
        end
        if USE_IRRKLANG then
            links { "irrklang" }
        end
    filter "system:linux"
        links { "GL", "X11", "Xxf86vm" }
        linkoptions { "-fopenmp" }
        if USE_IRRKLANG then
            links { "IrrKlang" }
            linkoptions{ IRRKLANG_LINK_RPATH }
        end
