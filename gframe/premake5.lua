include "lzma/."
include "spmemvfs/."

project "ygopro"
    kind "WindowedApp"

    files { "*.cpp", "*.h" }
    includedirs { "../ocgcore" }
    links { "ocgcore", "clzma", "cspmemvfs", "lua", "irrlicht", "freetype", "event" }

    if BUILD_EVENT then
    	includedirs { "../event/include" }
    end

    if BUILD_IRRLICHT then
        includedirs { "../irrlicht/include" }
    end

    if BUILD_FREETYPE then
        includedirs { "../freetype/include" }
    end

    if BUILD_SQLITE then
        includedirs { "../sqlite" }
        links { "sqlite" }
    else
        links { "sqlite3" } --most system use this name
    end

    filter "system:windows"
        defines { "_IRR_WCHAR_FILESYSTEM" }
        files "ygopro.rc"
        libdirs { "$(DXSDK_DIR)Lib/x86" }
        if USE_IRRKLANG then
            defines { "YGOPRO_USE_IRRKLANG" }
            links { "irrKlang" }
            includedirs { "../irrklang/include" }
            if IRRKLANG_PRO then
                defines { "IRRKLANG_STATIC" }
                links { "ikpmp3" }
            else
                libdirs { "../irrklang/lib/Win32-visualStudio" }
            end
        end
        links { "opengl32", "ws2_32", "winmm", "gdi32", "kernel32", "user32", "imm32" }
    if IRRKLANG_PRO then
        filter { "system:windows", "not configurations:Debug" }
            libdirs { "../irrklang/lib/Win32-vs2019" }
        filter { "system:windows", "configurations:Debug" }
            libdirs { "../irrklang/lib/Win32-visualStudio" }
    end
    filter { "system:windows", "not action:vs*"}
        includedirs { "/mingw/include/irrlicht", "/mingw/include/freetype2" }
    filter "not action:vs*"
        buildoptions { "-std=c++14", "-fno-rtti" }
    filter "not system:windows"
        includedirs { "/usr/include/irrlicht", "/usr/include/freetype2" }
        links { "event_pthreads", "GL", "dl", "pthread", "X11", "Xxf86vm" }
    filter "system:linux"
        if USE_IRRKLANG then
            defines { "YGOPRO_USE_IRRKLANG" }
            links { "IrrKlang" }
            linkoptions{ "-Wl,-rpath=./irrklang/bin/linux-gcc-64/" }
            libdirs { "../irrklang/bin/linux-gcc-64" }
            includedirs { "../irrklang/include" }
        end
