project "irrlicht"
    kind "StaticLib"

    includedirs { "include", "source/Irrlicht", "source/Irrlicht/jpeglib", "source/Irrlicht/libpng", "source/Irrlicht/zlib" }
    
    dofile("defines.lua")
    exceptionhandling "Off"
    rtti "Off"
    
    files { "include/*.h",
            "source/Irrlicht/*.cpp",
            "source/Irrlicht/lzma/*.h",
            "source/Irrlicht/lzma/*.c",
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
            "source/Irrlicht/jpeglib/jaricom.c",
            "source/Irrlicht/jpeglib/jcapimin.c",
            "source/Irrlicht/jpeglib/jcapistd.c",
            "source/Irrlicht/jpeglib/jcarith.c",
            "source/Irrlicht/jpeglib/jccoefct.c",
            "source/Irrlicht/jpeglib/jccolor.c",
            "source/Irrlicht/jpeglib/jcdctmgr.c",
            "source/Irrlicht/jpeglib/jchuff.c",
            "source/Irrlicht/jpeglib/jcinit.c",
            "source/Irrlicht/jpeglib/jcmainct.c",
            "source/Irrlicht/jpeglib/jcmarker.c",
            "source/Irrlicht/jpeglib/jcmaster.c",
            "source/Irrlicht/jpeglib/jcomapi.c",
            "source/Irrlicht/jpeglib/jcparam.c",
            "source/Irrlicht/jpeglib/jcprepct.c",
            "source/Irrlicht/jpeglib/jcsample.c",
            "source/Irrlicht/jpeglib/jctrans.c",
            "source/Irrlicht/jpeglib/jdapimin.c",
            "source/Irrlicht/jpeglib/jdapistd.c",
            "source/Irrlicht/jpeglib/jdarith.c",
            "source/Irrlicht/jpeglib/jdatadst.c",
            "source/Irrlicht/jpeglib/jdatasrc.c",
            "source/Irrlicht/jpeglib/jdcoefct.c",
            "source/Irrlicht/jpeglib/jdcolor.c",
            "source/Irrlicht/jpeglib/jddctmgr.c",
            "source/Irrlicht/jpeglib/jdhuff.c",
            "source/Irrlicht/jpeglib/jdinput.c",
            "source/Irrlicht/jpeglib/jdmainct.c",
            "source/Irrlicht/jpeglib/jdmarker.c",
            "source/Irrlicht/jpeglib/jdmaster.c",
            "source/Irrlicht/jpeglib/jdmerge.c",
            "source/Irrlicht/jpeglib/jdpostct.c",
            "source/Irrlicht/jpeglib/jdsample.c",
            "source/Irrlicht/jpeglib/jdtrans.c",
            "source/Irrlicht/jpeglib/jerror.c",
            "source/Irrlicht/jpeglib/jfdctflt.c",
            "source/Irrlicht/jpeglib/jfdctfst.c",
            "source/Irrlicht/jpeglib/jfdctint.c",
            "source/Irrlicht/jpeglib/jidctflt.c",
            "source/Irrlicht/jpeglib/jidctfst.c",
            "source/Irrlicht/jpeglib/jidctint.c",
            "source/Irrlicht/jpeglib/jmemmgr.c",
            "source/Irrlicht/jpeglib/jmemnobs.c",
            "source/Irrlicht/jpeglib/jquant1.c",
            "source/Irrlicht/jpeglib/jquant2.c",
            "source/Irrlicht/jpeglib/jutils.c",
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
            "source/Irrlicht/aesGladman/*.h",
            "source/Irrlicht/aesGladman/*.cpp" }

    filter { "system:windows" }
        defines { "_IRR_WCHAR_FILESYSTEM" }
        includedirs { "$(DXSDK_DIR)Include" }
        libdirs { "$(DXSDK_DIR)Lib/x86" }
        links { "imm32" }

    filter { "system:linux" }
        links { "X11", "Xxf86vm" }

