project "sfAudio"
	kind "StaticLib"
	cppdialect "C++14"
	includedirs { "include", "external/headers" }
	files { "src/**" }
	filter "system:not windows"
		links { "FLAC", "vorbisfile", "vorbis", "ogg", "openAL" }