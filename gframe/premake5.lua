local ygopro_config=function(static_core)
	kind "WindowedApp"
	cppdialect "C++14"
	rtti "Off"
	files { "**.cpp", "**.cc", "**.c", "**.h" }
	excludes { "lzma/**", "sound_sdlmixer.*", "sound_irrklang.*" }

	defines "CURL_STATICLIB"
	if _OPTIONS["pics"] then
		defines { "DEFAULT_PIC_URL=" .. _OPTIONS["pics"] }
	end
	if _OPTIONS["fields"] then
		defines { "DEFAULT_FIELD_URL=" .. _OPTIONS["fields"] }
	end
	if _OPTIONS["discord"] then
		defines { "DISCORD_APP_ID=" .. _OPTIONS["discord"] }
	end

	includedirs "../ocgcore"
	links { "clzma", "freetype", "Irrlicht" }
	if _OPTIONS["sound"] then
		if _OPTIONS["sound"]=="irrklang" then
			defines "YGOPRO_USE_IRRKLANG"
			includedirs "../irrKlang/include"
			files "sound_irrklang.*"
			links "IrrKlang"
			filter "system:windows"
				libdirs "../irrKlang/lib/Win32-visualStudio"
			filter "system:macosx"
				libdirs "../irrKlang/bin/macosx-gcc/"
			filter "system:linux"
				libdirs "../irrKlang/bin/linux-gcc-64/"
		end
		if _OPTIONS["sound"]=="sdl-mixer" then
			defines "YGOPRO_USE_SDL_MIXER"
			files "sound_sdlmixer.*"
			filter "system:windows"
				links { "version", "setupapi" }
		end
	end

	filter "system:windows"
		kind "ConsoleApp"
		files "ygopro.rc"
		includedirs { "../freetype/include", "../irrlicht/include" }
		dofile("../irrlicht/defines.lua")
		links { "opengl32", "ws2_32", "winmm", "gdi32", "kernel32", "user32", "imm32", "wldap32", "crypt32", "advapi32", "rpcrt4", "ole32", "winhttp" }

	filter { "system:windows", "options:no-direct3d" }
		defines "NO_IRR_COMPILE_WITH_DIRECT3D_9_"

	filter { "system:windows", "options:not no-direct3d" }
		defines "IRR_COMPILE_WITH_DX9_DEV_PACK"

	filter "system:not windows"
		defines "LUA_COMPAT_5_2"
		buildoptions { "-fpermissive" }
		if _OPTIONS["discord"] then
			links "discord-rpc"
		end
		links { "sqlite3", "event", "event_pthreads", "dl", "pthread", "git2" }

	filter "system:macosx"
		files "*.m"
		defines "LUA_USE_MACOSX"
		buildoptions { "-fms-extensions" }
		includedirs { "/usr/local/include/freetype2", "/usr/local/include/irrlicht" }
		linkoptions { "-Wl,-rpath ./" }
		links { "fmt", "curl", "Cocoa.framework", "IOKit.framework", "OpenGL.framework" }
		if static_core then
			links "lua"
		end

	filter { "system:macosx", "options:no-irrklang" }
		includedirs "/usr/local/opt/openal-soft/include"
		libdirs "/usr/local/opt/openal-soft/lib"
		links { "openal", "mpg123", "sndfile", "vorbis", "vorbisenc", "ogg", "FLAC" }

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
