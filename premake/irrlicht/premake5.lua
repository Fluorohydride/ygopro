project "irrlicht"
    kind "StaticLib"

    includedirs { "include", JPEG_INCLUDE_DIR }

    exceptionhandling "Off"
    rtti "Off"

    defines {
        "_IRR_STATIC_LIB_",
        "NO_IRR_USE_NON_SYSTEM_JPEG_LIB_",
        "NO_IRR_LINUX_X11_VIDMODE_",
        "NO_IRR_COMPILE_WITH_BZIP2_",
        "NO_IRR_COMPILE_WITH_LZMA_",
        "NO_IRR_COMPILE_WITH_CONSOLE_DEVICE_",
        "NO_IRR_COMPILE_WITH_DIRECT3D_8_",
        "NO_IRR_COMPILE_WITH_DIRECTINPUT_JOYSTICK_",
        "NO_IRR_COMPILE_WITH_JOYSTICK_EVENTS_",
        "NO_IRR_COMPILE_WITH_SOFTWARE_",
        "NO_IRR_COMPILE_WITH_BURNINGSVIDEO_",
        "NO_IRR_COMPILE_WITH_IRR_SCENE_LOADER_",
        "NO_IRR_COMPILE_WITH_SKINNED_MESH_SUPPORT_",
        "NO_IRR_COMPILE_WITH_IRR_MESH_LOADER_",
        "NO_IRR_COMPILE_WITH_HALFLIFE_LOADER_",
        "NO_IRR_COMPILE_WITH_MD2_LOADER_",
        "NO_IRR_COMPILE_WITH_MD3_LOADER_",
        "NO_IRR_COMPILE_WITH_3DS_LOADER_",
        "NO_IRR_COMPILE_WITH_COLLADA_LOADER_",
        "NO_IRR_COMPILE_WITH_CSM_LOADER_",
        "NO_IRR_COMPILE_WITH_BSP_LOADER_",
        "NO_IRR_COMPILE_WITH_DMF_LOADER_",
        "NO_IRR_COMPILE_WITH_LMTS_LOADER_",
        "NO_IRR_COMPILE_WITH_MY3D_LOADER_",
        "NO_IRR_COMPILE_WITH_OBJ_LOADER_",
        "NO_IRR_COMPILE_WITH_OCT_LOADER_",
        "NO_IRR_COMPILE_WITH_LWO_LOADER_",
        "NO_IRR_COMPILE_WITH_STL_LOADER_",
        "NO_IRR_COMPILE_WITH_PLY_LOADER_",
        "NO_IRR_COMPILE_WITH_SMF_LOADER_",
        "NO_IRR_COMPILE_WITH_IRR_WRITER_",
        "NO_IRR_COMPILE_WITH_COLLADA_WRITER_",
        "NO_IRR_COMPILE_WITH_STL_WRITER_",
        "NO_IRR_COMPILE_WITH_OBJ_WRITER_",
        "NO_IRR_COMPILE_WITH_PLY_WRITER_",
        "NO_IRR_COMPILE_WITH_PCX_LOADER_",
        "NO_IRR_COMPILE_WITH_PPM_LOADER_",
        "NO_IRR_COMPILE_WITH_PSD_LOADER_",
        "NO_IRR_COMPILE_WITH_PVR_LOADER_",
        "NO_IRR_COMPILE_WITH_DDS_LOADER_",
        "NO_IRR_COMPILE_WITH_TGA_LOADER_",
        "NO_IRR_COMPILE_WITH_WAL_LOADER_",
        "NO_IRR_COMPILE_WITH_LMP_LOADER_",
        "NO_IRR_COMPILE_WITH_RGB_LOADER_",
        "NO_IRR_COMPILE_WITH_PCX_WRITER_",
        "NO_IRR_COMPILE_WITH_PPM_WRITER_",
        "NO_IRR_COMPILE_WITH_PSD_WRITER_",
        "NO_IRR_COMPILE_WITH_TGA_WRITER_",
        "NO__IRR_COMPILE_WITH_PAK_ARCHIVE_LOADER_",
        "NO__IRR_COMPILE_WITH_NPK_ARCHIVE_LOADER_",
        "NO__IRR_COMPILE_WITH_WAD_ARCHIVE_LOADER_",
        "NO_IRR_COMPILE_WITH_ZIP_ENCRYPTION_",
    }

    files {
        "include/*.h",
        "source/Irrlicht/*.cpp",
        "source/Irrlicht/zlib/zlib.h",
        "source/Irrlicht/zlib/adler32.c",
        "source/Irrlicht/zlib/compress.c",
        "source/Irrlicht/zlib/crc32.h",
        "source/Irrlicht/zlib/crc32.c",
        "source/Irrlicht/zlib/deflate.h",
        "source/Irrlicht/zlib/deflate.c",
        "source/Irrlicht/zlib/inffast.h",
        "source/Irrlicht/zlib/inffast.c",
        "source/Irrlicht/zlib/inflate.h",
        "source/Irrlicht/zlib/inflate.c",
        "source/Irrlicht/zlib/inftrees.h",
        "source/Irrlicht/zlib/inftrees.c",
        "source/Irrlicht/zlib/trees.h",
        "source/Irrlicht/zlib/trees.c",
        "source/Irrlicht/zlib/uncompr.c",
        "source/Irrlicht/zlib/zutil.h",
        "source/Irrlicht/zlib/zutil.c",
    }

    if BUILD_PNG_IRRLICHT then
        files {
            "source/Irrlicht/libpng/png.c",
            "source/Irrlicht/libpng/pngerror.c",
            "source/Irrlicht/libpng/pngget.c",
            "source/Irrlicht/libpng/pngmem.c",
            "source/Irrlicht/libpng/pngpread.c",
            "source/Irrlicht/libpng/pngread.c",
            "source/Irrlicht/libpng/pngrio.c",
            "source/Irrlicht/libpng/pngrtran.c",
            "source/Irrlicht/libpng/pngrutil.c",
            "source/Irrlicht/libpng/pngset.c",
            "source/Irrlicht/libpng/pngtrans.c",
            "source/Irrlicht/libpng/pngwio.c",
            "source/Irrlicht/libpng/pngwrite.c",
            "source/Irrlicht/libpng/pngwtran.c",
            "source/Irrlicht/libpng/pngwutil.c",
            "source/Irrlicht/libpng/intel/intel_init.c",
            "source/Irrlicht/libpng/intel/filter_sse2_intrinsics.c",
        }
        defines {
            "PNG_INTEL_SSE",
            "PNG_ARM_NEON_OPT=0",
            "PNG_ARM_NEON_IMPLEMENTATION=0",
        }
    else
        includedirs { PNG_INCLUDE_DIR }
        defines {
            "NO_IRR_USE_NON_SYSTEM_LIB_PNG_",
        }
    end

    filter { "system:windows" }
        if USE_DXSDK then
            includedirs { "$(DXSDK_DIR)Include" }
        else
            defines { "NO_IRR_COMPILE_WITH_DIRECT3D_9_" }
        end

    filter { "system:macosx" }
        cppdialect "gnu++14"
        defines { "GL_SILENCE_DEPRECATION" }
        files {
            "source/Irrlicht/*.mm",
        }

    filter { "system:macosx", "files:source/Irrlicht/Irrlicht.cpp or source/Irrlicht/COSOperator.cpp" }
        compileas "Objective-C++" 
