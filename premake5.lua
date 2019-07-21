newoption {
	trigger		= "no-direct3d",
	description	= "Disable DirectX options in irrlicht if the DirectX SDK isn't installed"
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
workspace "ygo"
	location "build"
	language "C++"
	objdir "obj"
	startproject "ygopro"
	staticruntime "on"

	configurations { "Debug", "Release" }

	filter "system:windows"
		defines { "WIN32", "_WIN32", "NOMINMAX" }

	filter "system:bsd"
		includedirs "/usr/local/include"
		libdirs "/usr/local/lib"

	filter "system:macosx"
		toolset "clang"
		buildoptions { "-fms-extensions" }
		includedirs { "/usr/local/include" }
		libdirs { "/usr/local/lib" }
		links { "Cocoa.framework", "IOKit.framework", "OpenGL.framework" }

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
		buildoptions "-march=native"

	filter "configurations:Release"
		optimize "Size"
		targetdir "bin/release"
	
	subproject = true
	include "ocgcore"
	include "gframe"
	if os.istarget("windows") then
		include "freetype"
		include "irrlicht"
	end

local function vcpkgStaticTriplet(prj)
	premake.w('<VcpkgTriplet Condition="\'$(Platform)\'==\'Win32\'">x86-windows-static</VcpkgTriplet>')
	premake.w('<VcpkgTriplet Condition="\'$(Platform)\'==\'x64\'">x64-windows-static</VcpkgTriplet>')
end

require('vstudio')

premake.override(premake.vstudio.vc2010.elements, "globals", function(base, prj)
	local calls = base(prj)
	table.insertafter(calls, premake.vstudio.vc2010.targetPlatformVersionGlobal, vcpkgStaticTriplet)
	return calls
end) 
