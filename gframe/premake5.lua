include "lzma/."
include "spmemvfs/."

project "ygopro"
    kind "WindowedApp"

    files { "*.cpp", "*.h" }
    includedirs { "../ocgcore" }
    links { "ocgcore", "clzma", "cspmemvfs", "lua", "sqlite3", "irrlicht", "freetype", "event" }

    if BUILD_EVENT then
        includedirs { "../event/include" }
    end

    if BUILD_IRRLICHT or os.ishost("macosx") then
        includedirs { "../irrlicht/include" }
    end

    if BUILD_FREETYPE then
        includedirs { "../freetype/include" }
    end

    if BUILD_SQLITE then
        includedirs { "../sqlite3" }
    end

    if USE_IRRKLANG then
        defines { "YGOPRO_USE_IRRKLANG" }
        includedirs { "../irrklang/include" }
    end

    filter "system:windows"
        defines { "_IRR_WCHAR_FILESYSTEM" }
        files "ygopro.rc"
        libdirs { "$(DXSDK_DIR)Lib/x86" }
        if USE_IRRKLANG then
            links { "irrKlang" }
            if IRRKLANG_PRO then
                defines { "IRRKLANG_STATIC" }
                links { "ikpmp3" }
                filter { "not configurations:Debug" }
                    libdirs { "../irrklang/lib/Win32-vs2019" }
                filter { "configurations:Debug" }
                    libdirs { "../irrklang/lib/Win32-visualStudio-debug" }
                filter {}
            else
                libdirs { "../irrklang/lib/Win32-visualStudio" }
            end
        end
        links { "opengl32", "ws2_32", "winmm", "gdi32", "kernel32", "user32", "imm32" }
    filter { "system:windows", "not action:vs*"}
        includedirs { "/mingw/include/irrlicht", "/mingw/include/freetype2" }
    filter "not action:vs*"
        buildoptions { "-std=c++14", "-fno-rtti" }
    filter "not system:windows"
        includedirs { "/usr/include/irrlicht", "/usr/include/freetype2" }
        links { "event_pthreads", "dl", "pthread", "X11" }
    filter "system:macosx"
        libdirs { "../irrlicht/source/Irrlicht/MacOSX/build/Release/" }
        links { "z" }
        defines { "GL_SILENCE_DEPRECATION" }
        if USE_IRRKLANG then
            links { "irrklang" }
            libdirs { "../irrklang/bin/macosx-gcc" }
        end
    filter "system:linux"
        links { "GL", "Xxf86vm" }
        if USE_IRRKLANG then
            links { "IrrKlang" }
            linkoptions{ "-Wl,-rpath=./irrklang/bin/linux-gcc-64/" }
            libdirs { "../irrklang/bin/linux-gcc-64" }
        end
