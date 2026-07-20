project "jpeg"
    kind "StaticLib"

    files {
        "src/jconfig.h",
        "src/jconfigint.h",
        "src/jcapimin.c",
        "src/jchuff.c",
        "src/jcicc.c",
        "src/jcinit.c",
        "src/jclhuff.c",
        "src/jcmarker.c",
        "src/jcmaster.c",
        "src/jcomapi.c",
        "src/jcparam.c",
        "src/jcphuff.c",
        "src/jctrans.c",
        "src/jdapimin.c",
        "src/jdatadst.c",
        "src/jdatasrc.c",
        "src/jdhuff.c",
        "src/jdicc.c",
        "src/jdinput.c",
        "src/jdlhuff.c",
        "src/jdmarker.c",
        "src/jdmaster.c",
        "src/jdphuff.c",
        "src/jdtrans.c",
        "src/jerror.c",
        "src/jfdctflt.c",
        "src/jmemmgr.c",
        "src/jmemnobs.c",
        "src/jpeg_nbits.c",
        "src/jaricom.c",
        "src/jcarith.c",
        "src/jdarith.c",
        "src/wrapper/j*.c",
    }

if USE_SIMD == "none" then
    defines { "YGOPRO_NO_SIMD" }
else
    local nasm = os.getenv("ASM_NASM") or "nasm"
    local turboSimdNasmDir = path.getabsolute("simd/nasm")
    local turboSimdX64Dir  = path.getabsolute("simd/x86_64")
    local turboSimdX86Dir  = path.getabsolute("simd/i386")

    filter { "architecture:x86_64" }
        defines { "SIMD_ARCHITECTURE=X86_64" }
        includedirs {
            "simd",
            "simd/nasm",
            "simd/x86_64",
        }
        files {
            "simd/jsimd.c",
            "simd/x86_64/*.asm",
        }
        removefiles {
            "simd/x86_64/jccolext-*.asm",
            "simd/x86_64/jcgryext-*.asm",
            "simd/x86_64/jdcolext-*.asm",
            "simd/x86_64/jdmrgext-*.asm",
        }

    filter { "architecture:x86" }
        defines { "SIMD_ARCHITECTURE=I386" }
        includedirs {
            "simd",
            "simd/nasm",
            "simd/i386",
        }
        files {
            "simd/jsimd.c",
            "simd/i386/*.asm",
        }
        removefiles {
            "simd/i386/jccolext-*.asm",
            "simd/i386/jcgryext-*.asm",
            "simd/i386/jdcolext-*.asm",
            "simd/i386/jdmrgext-*.asm",
        }

    filter { "action:vs*", "architecture:x86_64", "files:simd/**.asm" }
        buildmessage "NASM Compiling %{file.relpath}"
        buildcommands {
            '"' .. nasm .. '" -f win64 -DWIN64 -D__x86_64__ '
            .. '-I"' .. turboSimdNasmDir .. '" '
            .. '-I"' .. turboSimdX64Dir .. '" '
            .. '"%{file.relpath}" -o "%{cfg.objdir}/%{file.basename}.obj"'
        }
        buildoutputs { "%{cfg.objdir}/%{file.basename}.obj" }

    filter { "action:vs*", "architecture:x86", "files:simd/**.asm" }
        buildmessage "NASM Compiling %{file.relpath}"
        buildcommands {
            '"' .. nasm .. '" -f win32 -DWIN32 '
            .. '-I"' .. turboSimdNasmDir .. '" '
            .. '-I"' .. turboSimdX86Dir .. '" '
            .. '"%{file.relpath}" -o "%{cfg.objdir}/%{file.basename}.obj"'
        }
        buildoutputs { "%{cfg.objdir}/%{file.basename}.obj" }

    filter { "action:gmake", "system:windows", "architecture:x86_64", "files:simd/**.asm" }
        buildmessage "NASM Compiling %{file.relpath}"
        buildcommands {
            '"' .. nasm .. '" -f win64 -DWIN64 -D__x86_64__ -DPIC '
            .. '-I"' .. turboSimdNasmDir .. '" '
            .. '-I"' .. turboSimdX64Dir .. '" '
            .. '"%{file.relpath}" -o "%{cfg.objdir}/%{file.basename}.o"'
        }
        buildoutputs { "%{cfg.objdir}/%{file.basename}.o" }

    filter { "action:gmake", "system:linux", "architecture:x86_64", "files:simd/**.asm" }
        buildmessage "NASM Compiling %{file.relpath}"
        buildcommands {
            '"' .. nasm .. '" -f elf64 -DELF -D__x86_64__ -DPIC '
            .. '-I"' .. turboSimdNasmDir .. '" '
            .. '-I"' .. turboSimdX64Dir .. '" '
            .. '"%{file.relpath}" -o "%{cfg.objdir}/%{file.basename}.o"'
        }
        buildoutputs { "%{cfg.objdir}/%{file.basename}.o" }

    filter { "action:gmake", "system:macosx", "architecture:x86_64", "files:simd/**.asm" }
        buildmessage "NASM Compiling %{file.relpath}"
        buildcommands {
            '"' .. nasm .. '" -f macho64 -DMACHO -D__x86_64__ -DPIC '
            .. '-I"' .. turboSimdNasmDir .. '" '
            .. '-I"' .. turboSimdX64Dir .. '" '
            .. '"%{file.relpath}" -o "%{cfg.objdir}/%{file.basename}.o"'
        }
        buildoutputs { "%{cfg.objdir}/%{file.basename}.o" }

    filter { "architecture:AARCH64" }
        defines { "SIMD_ARCHITECTURE=ARM64" }
        includedirs {
            "simd",
            "simd/arm",
        }
        files {
            "simd/jsimd.c",
            "simd/arm/jccolor-neon.c",
            "simd/arm/jcgray-neon.c",
            "simd/arm/jcphuff-neon.c",
            "simd/arm/jcsample-neon.c",
            "simd/arm/jdcolor-neon.c",
            "simd/arm/jdmerge-neon.c",
            "simd/arm/jdsample-neon.c",
            "simd/arm/jfdctfst-neon.c",
            "simd/arm/jfdctint-neon.c",
            "simd/arm/jidctfst-neon.c",
            "simd/arm/jidctint-neon.c",
            "simd/arm/jidctred-neon.c",
            "simd/arm/jquanti-neon.c",
            "simd/arm/aarch64/jchuff-neon.c",
            "simd/arm/aarch64/jsimdcpu.c",
        }
end
