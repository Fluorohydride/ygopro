newoption {
	trigger	= "no-direct3d",
	description = "Disable DirectX options in irrlicht if the DirectX SDK isn't installed"
}
newoption {
	trigger = "oldwindows",
	description = "Use some tricks to support up to windows 2000"
}
newoption {
	trigger = "sound",
	value = "backend",
	description = "Choose sound backend",
	allowed = {
		{ "irrklang",  "irrklang" },
		{ "sdl-mixer",  "SDL2-mixer" },
		{ "sfml",  "SFML" }
	}
}
newoption {
	trigger = "use-mpg123",
	description = "Use mpg123 mp3 backend instead of minimp3 (Available only when using SFML audio backend)"
}
newoption {
	trigger = "no-joystick",
	description = "Add base joystick compatibility (Requires SDL2)"
}
newoption {
	trigger = "pics",
	value = "url_template",
	description = "Default URL for card images"
}
newoption {
	trigger = "fields",
	value = "url_template",
	description = "Default URL for Field Spell backgrounds"
}
newoption {
	trigger = "covers",
	value = "url_template",
	description = "Default URL for cover images"
}
newoption {
	trigger = "prebuilt-core",
	value = "path",
	description = "Path to library folder containing libocgcore"
}
newoption {
	trigger = "vcpkg-root",
	value = "path",
	description = "Path to vcpkg installation"
}
newoption {
	trigger = "discord",
	value = "app_id_token",
	description = "Discord App ID for rich presence"
}
newoption {
	trigger = "update-url",
	value = "url",
	description = "API endpoint to check for updates from"
}
workspace "ygo"
	location "build"
	language "C++"
	objdir "obj"
	startproject "ygopro"
	staticruntime "on"
	if _OPTIONS["oldwindows"] then
		filter "action:vs*"
			toolset "v141_xp"
		filter {}
	end

	configurations { "Debug", "Release" }

	filter "system:windows"
		systemversion "latest"
		defines { "WIN32", "_WIN32", "NOMINMAX" }

	if _OPTIONS["vcpkg-root"] then
		filter "system:linux"
			includedirs { _OPTIONS["vcpkg-root"] .. "/installed/x64-linux/include" }

		filter { "system:linux", "configurations:Debug" }
			libdirs { _OPTIONS["vcpkg-root"] .. "/installed/x64-linux/debug/lib" }

		filter { "system:linux", "configurations:Release" }
			libdirs { _OPTIONS["vcpkg-root"] .. "/installed/x64-linux/lib" }

		filter "system:macosx"
			includedirs { _OPTIONS["vcpkg-root"] .. "/installed/x64-osx/include" }

		filter { "system:macosx", "configurations:Debug" }
			libdirs { _OPTIONS["vcpkg-root"] .. "/installed/x64-osx/debug/lib" }

		filter { "system:macosx", "configurations:Release" }
			libdirs { _OPTIONS["vcpkg-root"] .. "/installed/x64-osx/lib" }
	end

	filter "system:macosx"
		defines { "GL_SILENCE_DEPRECATION" }
		includedirs { "/usr/local/include" }
		libdirs { "/usr/local/lib" }

	filter "action:vs*"
		vectorextensions "SSE2"
		buildoptions "-wd4996"
		defines "_CRT_SECURE_NO_WARNINGS"

	filter "action:not vs*"
		buildoptions { "-fno-strict-aliasing", "-Wno-multichar" }

	filter { "action:not vs*", "system:windows" }
	  buildoptions { "-static-libgcc" }

	filter "configurations:Debug"
		symbols "On"
		defines "_DEBUG"
		targetdir "bin/debug"
		runtime "Debug"

	filter { "configurations:Release*" , "action:not vs*" }
		symbols "On"
		defines "NDEBUG"

	filter "configurations:Release"
		optimize "Size"
		targetdir "bin/release"

	subproject = true
	if not _OPTIONS["prebuilt-core"] then
		include "ocgcore"
	end
	include "gframe"
	include "freetype"
	if os.istarget("windows") then
		include "irrlicht"
	end
	if os.istarget("macosx") and _OPTIONS["discord"] then
		include "discord-launcher"
	end

local function vcpkgStaticTriplet(prj)
	premake.w('<VcpkgTriplet Condition="\'$(Platform)\'==\'Win32\'">x86-windows-static</VcpkgTriplet>')
	premake.w('<VcpkgTriplet Condition="\'$(Platform)\'==\'x64\'">x64-windows-static</VcpkgTriplet>')
end

local function disableWinXPWarnings(prj)
	premake.w('<XPDeprecationWarning>false</XPDeprecationWarning>')
end

local function vcpkgStaticTriplet202006(prj)
	premake.w('<VcpkgEnabled>true</VcpkgEnabled>')
    premake.w('<VcpkgUseStatic>true</VcpkgUseStatic>')
	premake.w('<VcpkgAutoLink>true</VcpkgAutoLink>')
end

require('vstudio')

premake.override(premake.vstudio.vc2010.elements, "globals", function(base, prj)
	local calls = base(prj)
	table.insertafter(calls, premake.vstudio.vc2010.targetPlatformVersionGlobal, vcpkgStaticTriplet)
	table.insertafter(calls, premake.vstudio.vc2010.targetPlatformVersionGlobal, disableWinXPWarnings)
	table.insertafter(calls, premake.vstudio.vc2010.globals, vcpkgStaticTriplet202006)
	return calls
end)
