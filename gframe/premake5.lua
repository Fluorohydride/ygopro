include "lzma/."
project "ygopro"
	filter "*DLL"
		targetname "ygoprodll"
		defines "YGOPRO_BUILD_DLL"
	filter {}
	kind "WindowedApp"
	files { "**.cpp", "**.cc", "**.c", "**.h" }
	excludes "lzma/**"
	includedirs { "../ocgcore", "../irrKlang/include", "../fmt/include" }

	links { "ocgcore", "clzma", "Irrlicht", "freetype", "sqlite3" , "event", "IrrKlang", "fmt" }
	filter "system:windows"
		kind "ConsoleApp"
		files "../ygopro.rc"
		excludes "CGUIButton.cpp"
		includedirs { "../irrlicht/include", "../freetype/include", "../event/include", "../sqlite3" }
		dofile("../irrlicht defines.lua")
		libdirs "../irrKlang/lib/Win32-visualStudio"
		links { "opengl32", "ws2_32", "winmm", "gdi32", "kernel32", "user32", "imm32" }
		filter "options:no-direct3d"
			defines "NO_IRR_COMPILE_WITH_DIRECT3D_9_"

	filter { "action:not vs*", "system:windows" }
		includedirs { "/mingw/include/irrlicht", "/mingw/include/freetype2" }
		libdirs "../irrKlang/lib/Win32-gcc"

	filter "action:not vs*"
		buildoptions { "-std=c++14", "-fno-rtti", "-fpermissive" }

	filter "system:not windows"
		defines "LUA_COMPAT_5_2"
		includedirs { "/usr/include/irrlicht", "/usr/include/freetype2" }
		excludes "COSOperator.*"
		libdirs "../irrKlang/lib"
		links { "event_pthreads", "GL", "dl", "pthread", "lua5.3-c++" }
		linkoptions { "-Wl,-rpath '-Wl,$$ORIGIN'" }

	filter "system:bsd"
		defines "LUA_USE_POSIX"

	filter "system:macosx"
		defines "LUA_USE_MACOSX"

	filter "system:linux"
		defines "LUA_USE_LINUX"