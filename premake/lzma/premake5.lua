project "lzma"
    kind "StaticLib"
    language "C"

    defines {
        "HAVE_INTTYPES_H=1",
        "HAVE_STDINT_H=1",
        "HAVE_STDBOOL_H=1",
        "HAVE_STRING_H=1",
        "HAVE_STDLIB_H=1",
        "HAVE_STDIO_H=1",
        "HAVE_CHECK_CRC32=1",
        "HAVE_ENCODERS=1",
        "HAVE_ENCODER_LZMA1=1",
        "HAVE_DECODERS=1",
        "HAVE_DECODER_LZMA1=1",
        "HAVE_MF_HC3=1",
        "HAVE_MF_HC4=1",
        "HAVE_MF_BT2=1",
        "HAVE_MF_BT3=1",
        "HAVE_MF_BT4=1",
        "HAVE_VISIBILITY=0",
    }

    includedirs {
        "src/common",
        "src/liblzma/api",
        "src/liblzma/common", 
        "src/liblzma/check",
        "src/liblzma/lzma",
        "src/liblzma/lz",
        "src/liblzma/rangecoder",
        "src/liblzma/simple",
        "src/liblzma/delta",
    }

    files {
        "src/liblzma/check/crc32_fast.c",
        "src/liblzma/common/common.c",
        "src/liblzma/common/filter_common.c",
        "src/liblzma/common/filter_buffer_encoder.c",
        "src/liblzma/common/filter_encoder.c",
        "src/liblzma/common/filter_flags_encoder.c",
        "src/liblzma/common/filter_buffer_decoder.c",
        "src/liblzma/common/filter_decoder.c",
        "src/liblzma/common/filter_flags_decoder.c",
        "src/liblzma/lzma/lzma_encoder_presets.c",
        "src/liblzma/lzma/lzma_encoder.c",
        "src/liblzma/lzma/lzma_encoder_optimum_fast.c",
        "src/liblzma/lzma/lzma_encoder_optimum_normal.c",
        "src/liblzma/lzma/lzma_decoder.c",
        "src/liblzma/lzma/fastpos_table.c",
        "src/liblzma/lz/lz_encoder.c",
        "src/liblzma/lz/lz_encoder_mf.c",
        "src/liblzma/lz/lz_decoder.c",
        "src/liblzma/rangecoder/price_table.c",
    }
