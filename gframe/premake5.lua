local stuff=function(static_core)
	defines { "YGOPRO_USE_IRRKLANG", "CURL_STATICLIB" }
	kind "WindowedApp"
	files { "**.cpp", "**.cc", "**.c", "**.h" }
	excludes "lzma/**"
	includedirs { "../ocgcore", "../irrKlang/include" }
	links { "clzma", "Irrlicht", "IrrKlang" }
	filter "system:windows"
		kind "ConsoleApp"
		files "../ygopro.rc"
		excludes "CGUIButton.cpp"
		includedirs "../irrlicht/include"
		dofile("../irrlicht/defines.lua")
		libdirs "../irrKlang/lib/Win32-visualStudio"
		links { "opengl32", "ws2_32", "winmm", "gdi32", "kernel32", "user32", "imm32", "Wldap32", "Crypt32", "Advapi32", "Rpcrt4", "Ole32", "Winhttp" }
		filter "options:no-direct3d"
			defines "NO_IRR_COMPILE_WITH_DIRECT3D_9_"

		filter "options:not no-direct3d"
			defines "IRR_COMPILE_WITH_DX9_DEV_PACK"

	filter { "action:not vs*", "system:windows" }
		includedirs { "/mingw/include/irrlicht", "/mingw/include/freetype2" }
		libdirs "../irrKlang/lib/Win32-gcc"

	filter "action:not vs*"
		buildoptions { "-std=c++14", "-fno-rtti", "-fpermissive" }

	filter "system:not windows"
		defines "LUA_COMPAT_5_2"
		includedirs { "/usr/include/irrlicht", "/usr/include/freetype2" }
		excludes "COSOperator.*"
		links { "freetype", "sqlite3" , "event", "fmt", "event_pthreads", "dl", "pthread", "git2", "curl" }

	filter "system:bsd"
		defines "LUA_USE_POSIX"
		linkoptions { "-Wl,-rpath=./" }
		links "GL"
		if static_core then
			links  "lua5.3-c++"
		end

	filter "system:macosx"
		defines "LUA_USE_MACOSX"
		linkoptions { "-Wl,-rpath ./" }
		libdirs "../irrKlang/bin/macosx-gcc/"
		links "OpenGL.framework"
		if static_core then
			links  "lua"
		end

	filter "system:linux"
		defines "LUA_USE_LINUX"
		linkoptions { "-Wl,-rpath=./" }
		libdirs "../irrKlang/bin/linux-gcc-64/"
		links "GL"
		if static_core then
			links  "lua5.3-c++"
		end
end

include "lzma/."
project "ygopro"
	targetname "ygopro"
	links { "ocgcore" }
	stuff(true)
		
project "ygoprodll"
	targetname "ygoprodll"
	defines "YGOPRO_BUILD_DLL"
	stuff()
		
