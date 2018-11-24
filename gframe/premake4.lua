include "lzma/."

project "ygopro"
    kind "WindowedApp"

    files { "**.cpp", "**.cc", "**.c", "**.h" }
    excludes "lzma/**"
    includedirs { "../ocgcore", "../irrKlang/include", "../fmt/include" }
    links { "ocgcore", "clzma", "Irrlicht", "freetype", "sqlite3" , "event", "IrrKlang", "fmt" }

    configuration "windows"
        files "../ygopro.rc"
        excludes "CGUIButton.cpp"
        includedirs { "../irrlicht/include", "../freetype/include", "../event/include", "../sqlite3" }
        libdirs { "../irrKlang/lib/Win32-visualStudio" }
        links { "opengl32", "ws2_32", "winmm", "gdi32", "kernel32", "user32", "imm32" }
    configuration {"windows", "not vs*"}
        includedirs { "/mingw/include/irrlicht", "/mingw/include/freetype2" }
        libdirs { "../irrKlang/lib/Win32-gcc" }
    configuration "not vs*"
        buildoptions { "-std=c++14", "-fno-rtti", "-fpermissive" }
    configuration "not windows"
        includedirs { "/usr/include/irrlicht", "/usr/include/freetype2" }
        excludes { "COSOperator.*" }
        libdirs { "../irrKlang/lib" }
        links { "event_pthreads", "GL", "dl", "pthread", "lua5.3" }
        linkoptions { "-Wl,-rpath '-Wl,\$\$ORIGIN'" }
