project "png"
    kind "StaticLib"

    includedirs { ZLIB_INCLUDE_DIR }

    files {
        "png.c",
        "pngerror.c",
        "pngget.c",
        "pngmem.c",
        "pngpread.c",
        "pngread.c",
        "pngrio.c",
        "pngrtran.c",
        "pngrutil.c",
        "pngset.c",
        "pngtrans.c",
        "pngwio.c",
        "pngwrite.c",
        "pngwtran.c",
        "pngwutil.c",
    }

if USE_SIMD ~= "none" then
    filter { "architecture:x86 or x86_64" }
        files {
            "intel/intel_init.c",
            "intel/filter_sse2_intrinsics.c",
        }
        defines {
            "PNG_INTEL_SSE",
        }

    filter { "architecture:AARCH64" }
        files {
            "arm/arm_init.c",
            "arm/filter_neon_intrinsics.c",
            "arm/palette_neon_intrinsics.c",
        }
        defines {
            "PNG_ARM_NEON_OPT=2",
        }
end
