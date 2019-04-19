include "lzma/."
project "ygopro"
	defines "YGOPRO_USE_IRRKLANG"
	configuration "*DLL"
		targetname "ygoprodll"
		defines "YGOPRO_BUILD_DLL"
	configuration {}
	kind "WindowedApp"
	files { "**.cpp", "**.cc", "**.c", "**.h" }
	excludes "lzma/**"
	includedirs { "../ocgcore", "../irrKlang/include", "../fmt/include" }

	links { "ocgcore", "clzma", "Irrlicht", "freetype", "sqlite3" , "event", "IrrKlang", "fmt" }
	configuration "windows"
		kind "ConsoleApp"
		files "../ygopro.rc"
		excludes "CGUIButton.cpp"
		includedirs { "../irrlicht/include", "../freetype/include", "../event/include", "../sqlite3" }
		dofile("../irrlicht defines.lua")
		libdirs "../irrKlang/lib/Win32-visualStudio"
		links { "opengl32", "ws2_32", "winmm", "gdi32", "kernel32", "user32", "imm32" }
		configuration "no-direct3d"
			defines "NO_IRR_COMPILE_WITH_DIRECT3D_9_"

		configuration "not no-direct3d"
			defines "IRR_COMPILE_WITH_DX9_DEV_PACK"

	configuration {"windows", "not vs*"}
		includedirs { "/mingw/include/irrlicht", "/mingw/include/freetype2" }
		libdirs "../irrKlang/lib/Win32-gcc"

	configuration "not vs*"
		buildoptions { "-std=c++14", "-fno-rtti", "-fpermissive" }

	configuration "not windows"
		defines "LUA_COMPAT_5_2"
		includedirs { "/usr/include/irrlicht", "/usr/include/freetype2" }
		excludes "COSOperator.*"
		libdirs "../irrKlang/lib"
		links { "event_pthreads", "GL", "dl", "pthread", "lua5.3-c++", "git2", "curl" }
		linkoptions { "-Wl,-rpath '-Wl,$$ORIGIN'" }

	configuration "macosx"
		defines "LUA_USE_MACOSX"

	configuration "bsd"
		defines "LUA_USE_POSIX"

	configuration "linux"
		defines "LUA_USE_LINUX"
