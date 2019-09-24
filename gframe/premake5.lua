local ygopro_config=function(static_core)
	if _OPTIONS["pics"] then
		defines { "DEFAULT_PIC_URL=" .. _OPTIONS["pics"] }
	end
	if _OPTIONS["fields"] then
		defines { "DEFAULT_FIELD_URL=" .. _OPTIONS["fields"] }
	end
	kind "WindowedApp"
	cppdialect "C++14"
	files { "**.cpp", "**.cc", "**.c", "**.h" }
	excludes "lzma/**"
	defines "CURL_STATICLIB"
	includedirs "../ocgcore"
	links { "clzma", "freetype", "Irrlicht" }
	filter "system:windows"
		kind "ConsoleApp"
		files "ygopro.rc"
		excludes "CGUIButton.cpp"
		includedirs { "../freetype/include", "../irrlicht/include" }
		dofile("../irrlicht/defines.lua")
		links { "opengl32", "ws2_32", "winmm", "gdi32", "kernel32", "user32", "imm32", "Wldap32", "Crypt32", "Advapi32", "Rpcrt4", "Ole32", "Winhttp" }
		filter "options:no-direct3d"
			defines "NO_IRR_COMPILE_WITH_DIRECT3D_9_"

		filter "options:not no-direct3d"
			defines "IRR_COMPILE_WITH_DX9_DEV_PACK"

	filter { "action:not vs*", "system:windows" }
		includedirs { "/mingw/include/irrlicht", "/mingw/include/freetype2" }

	filter "action:not vs*"
		buildoptions { "-fno-rtti", "-fpermissive" }

	filter "system:not windows"
		defines "LUA_COMPAT_5_2"
		excludes "COSOperator.*"
		links { "sqlite3", "event", "event_pthreads", "dl", "pthread", "git2",
		        "openal", "mpg123", "sndfile", "vorbis", "vorbisenc", "ogg", "FLAC" }

	filter "system:macosx"
		defines "LUA_USE_MACOSX"
		buildoptions { "-fms-extensions" }
		includedirs { "/usr/local/include/freetype2", "/usr/local/include/irrlicht", "/usr/local/opt/openal-soft/include" }
		linkoptions { "-Wl,-rpath ./" }
		libdirs "/usr/local/opt/openal-soft/lib"
		links { "fmt", "curl", "Cocoa.framework", "IOKit.framework", "OpenGL.framework" }
		if static_core then
			links  "lua"
		end

	filter { "system:linux", "configurations:Debug" }
		links { "fmtd", "curl-d" }

    filter { "system:linux", "configurations:Release" }
		links { "fmt", "curl" }

	filter "system:linux"
		defines "LUA_USE_LINUX"
		includedirs { "/usr/include/freetype2", "/usr/include/irrlicht" }
		linkoptions { "-Wl,-rpath=./" }
		links { "GL", "X11" }
		if static_core then
			links  "lua:static"
		end
		if _OPTIONS["vcpkg-root"] then
			links { "ssl", "crypto", "z" }
		end
end

include "lzma/."
project "ygopro"
	targetname "ygopro"
	if _OPTIONS["prebuilt-core"] then
		libdirs { _OPTIONS["prebuilt-core"] }
	end
	links { "ocgcore" }
	ygopro_config(true)
		
project "ygoprodll"
	targetname "ygoprodll"
	defines "YGOPRO_BUILD_DLL"
	ygopro_config()	
