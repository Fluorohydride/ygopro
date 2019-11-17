project "discord-launcher"
    targetname "discord-launcher"
    kind "ConsoleApp"
    cdialect "C11"
    warnings "Extra"
    buildoptions { "-pedantic" }
    files "discord-launcher.m"
    links "AppKit.framework"
