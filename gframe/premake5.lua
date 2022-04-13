local ygopro_config=function(static_core)
	kind "WindowedApp"
	cppdialect "C++14"
	rtti "Off"
	files { "**.cpp", "**.cc", "**.c", "**.h", "**.hpp" }
	excludes { "lzma/**", "sound_sdlmixer.*", "sound_irrklang.*", "irrklang_dynamic_loader.*", "sound_sfml.*", "sfAudio/**", "Android/**" }
	if _OPTIONS["oldwindows"] then
		files { "../overwrites/overwrites.cpp", "../overwrites/loader.asm" }
		filter "files:**.asm"
			exceptionhandling 'SEH'
		filter {}
	end

	defines "CURL_STATICLIB"
	if _OPTIONS["pics"] then
		defines { "DEFAULT_PIC_URL=" .. _OPTIONS["pics"] }
	end
	if _OPTIONS["fields"] then
		defines { "DEFAULT_FIELD_URL=" .. _OPTIONS["fields"] }
	end
	if _OPTIONS["covers"] then
		defines { "DEFAULT_COVER_URL=" .. _OPTIONS["covers"] }
	end
	if _OPTIONS["discord"] then
		defines { "DISCORD_APP_ID=" .. _OPTIONS["discord"] }
	end
	if _OPTIONS["update-url"] then
		defines { "UPDATE_URL=" .. _OPTIONS["update-url"] }
	end
	includedirs "../ocgcore"
	links { "clzma", "Irrlicht" }
	filter "system:macosx"
		links { "iconv" }
	filter {}
	if _OPTIONS["no-joystick"]=="false" then
		defines "YGOPRO_USE_JOYSTICK"
		filter { "system:not windows", "configurations:Debug" }
			links { "SDL2d" }
		filter { "system:not windows", "configurations:Release" }
			links { "SDL2" }
		filter "system:macosx"
			links { "CoreAudio.framework", "AudioToolbox.framework", "CoreVideo.framework", "ForceFeedback.framework", "Carbon.framework" }
		filter {}
	end
	if _OPTIONS["sound"] then
		if _OPTIONS["sound"]=="irrklang" then
			defines "YGOPRO_USE_IRRKLANG"
			includedirs "../irrKlang/include"
			files "sound_irrklang.*"
			files "irrklang_dynamic_loader.*"
		end
		if _OPTIONS["sound"]=="sdl-mixer" then
			defines "YGOPRO_USE_SDL_MIXER"
			files "sound_sdlmixer.*"
			filter "system:windows"
				links { "version", "setupapi" }
			filter { "system:not windows", "configurations:Debug" }
				links { "SDL2d" }
			filter { "system:not windows", "configurations:Release" }
				links { "SDL2" }
			filter "system:not windows"
				links { "SDL2_mixer", "FLAC", "mpg123", "vorbisfile", "vorbis", "ogg" }
			filter "system:macosx"
				links { "CoreAudio.framework", "AudioToolbox.framework", "CoreVideo.framework", "ForceFeedback.framework", "Carbon.framework" }
		end
		if _OPTIONS["sound"]=="sfml" then
			defines "YGOPRO_USE_SFML"
			files "sound_sfml.*"
			includedirs "../sfAudio/include"
			links { "sfAudio" }
			filter "system:not windows"
				links { "FLAC", "vorbisfile", "vorbis", "ogg", "openal" }
				if _OPTIONS["use-mpg123"] then
					links { "mpg123" }
				end
			filter "system:macosx"
				links { "CoreAudio.framework", "AudioToolbox.framework" }
			filter { "system:windows", "action:not vs*" }
				links { "FLAC", "vorbisfile", "vorbis", "ogg", "OpenAL32" }
				if _OPTIONS["use-mpg123"] then
					links { "mpg123" }
				end
		end
	end

	filter "system:windows"
		kind "ConsoleApp"
		files "ygopro.rc"
		includedirs { "../irrlicht/include" }
		dofile("../irrlicht/defines.lua")

	filter { "system:windows", "action:vs*" }
		files "dpiawarescaling.manifest"

	filter { "system:windows", "options:no-direct3d" }
		defines "NO_IRR_COMPILE_WITH_DIRECT3D_9_"

	filter { "system:windows", "options:not no-direct3d" }
		defines "IRR_COMPILE_WITH_DX9_DEV_PACK"

	filter "system:not windows"
		defines "LUA_COMPAT_5_2"
		if _OPTIONS["discord"] then
			links "discord-rpc"
		end
		links { "sqlite3", "event", "event_pthreads", "dl", "git2" }

	filter { "system:windows", "action:not vs*" }
		if _OPTIONS["discord"] then
			links "discord-rpc"
		end
		links { "sqlite3", "event", "git2" }

	filter "system:macosx"
		files { "*.m", "*.mm" }
		defines "LUA_USE_MACOSX"
		includedirs { "/usr/local/include/irrlicht" }
		links { "ssl", "crypto", "Cocoa.framework", "IOKit.framework", "OpenGL.framework", "Security.framework" }
		if static_core then
			links "lua"
		end

	filter { "system:macosx", "configurations:Debug" }
		links { "fmtd", "curl-d", "ldap", "freetyped" }

	filter { "system:macosx", "configurations:Release" }
		links { "fmt", "curl", "ldap", "freetype" }

	filter { "system:linux or windows", "action:not vs*", "configurations:Debug" }
		if _OPTIONS["vcpkg-root"] then
			links { "png16d", "bz2d", "fmtd", "curl-d", "freetyped" }
		else
			links { "fmt", "curl" }
		end

	filter { "system:linux or windows", "action:not vs*", "configurations:Release" }
		if _OPTIONS["vcpkg-root"] then
			links { "png", "bz2", "freetype" }
		end
		links { "fmt", "curl" }

	filter "system:linux"
		defines "LUA_USE_LINUX"
		if _OPTIONS["vcpkg-root"] then
			includedirs { _OPTIONS["vcpkg-root"] .. "/installed/x64-linux/include/irrlicht" }
		else
			includedirs "/usr/include/irrlicht"
		end
		if static_core then
			links  "lua:static"
		end
		if _OPTIONS["vcpkg-root"] then
			links { "ssl", "crypto", "z", "jpeg" }
		end
		
		
	filter { "system:windows", "action:not vs*" }
		if static_core then
			links "lua-c++"
		end
		if _OPTIONS["vcpkg-root"] then
			links { "ssl", "crypto", "z", "jpeg" }
		end

	filter "system:not windows"
		links { "pthread" }
	
	filter "system:windows"
		links { "opengl32", "ws2_32", "winmm", "gdi32", "kernel32", "user32", "imm32", "wldap32", "crypt32", "advapi32", "rpcrt4", "ole32", "uuid", "winhttp" }
		if not _OPTIONS["oldwindows"] then
			links "Iphlpapi"
		end
end

include "lzma/."
if _OPTIONS["sound"]=="sfml" then
	include "../sfAudio"
end
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
