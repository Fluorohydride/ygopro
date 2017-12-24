include "lzma/."

project "ygopro"
    kind "WindowedApp"

    files { "**.cpp", "**.cc", "**.c", "**.h" }
    excludes "lzma/**"
    includedirs { "../ocgcore" }
    links { "ocgcore", "clzma", "Irrlicht", "freetype", "sqlite3", "lua" , "event" }

    configuration "windows"
        files "ygopro.rc"
        excludes "CGUIButton.cpp"
        includedirs { "../irrlicht/include", "../freetype/include", "../event/include", "../sqlite3" }
        if USE_IRRKLANG then
            defines { "YGOPRO_USE_IRRKLANG" }
            links { "irrKlang" }
            includedirs { "../irrklang/include" }
            libdirs { "../irrklang/lib/Win32-visualStudio" }
            if IRRKLANG_PRO then
                defines { "IRRKLANG_STATIC" }
                links { "ikpmp3" }
            end
        end
        links { "opengl32", "ws2_32", "winmm", "gdi32", "kernel32", "user32", "imm32" }
    configuration {"windows", "not vs*"}
        includedirs { "/mingw/include/irrlicht", "/mingw/include/freetype2" }
    configuration "not vs*"
        buildoptions { "-std=gnu++0x", "-fno-rtti" }
    configuration "not windows"
        includedirs { "/usr/include/lua", "/usr/include/lua5.2", "/usr/include/lua/5.2", "/usr/include/irrlicht", "/usr/include/freetype2" }
        excludes { "COSOperator.*" }
        links { "event_pthreads", "GL", "dl", "pthread" }
    configuration "linux"
        removelinks { "lua" }
        links { "lua5.2" }
        if USE_IRRKLANG then
            defines { "YGOPRO_USE_IRRKLANG" }
            links { "IrrKlang" }
            linkoptions{ "-Wl,-rpath=./irrklang/bin/linux-gcc-64/" }
            libdirs { "../irrklang/bin/linux-gcc-64" }
            includedirs { "../irrklang/include" }
        end
