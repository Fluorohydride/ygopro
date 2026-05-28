project "webp"
    kind "StaticLib"

    includedirs { "." }

    files {
        "src/dec/alpha_dec.c",
        "src/dec/buffer_dec.c",
        "src/dec/frame_dec.c",
        "src/dec/io_dec.c",
        "src/dec/quant_dec.c",
        "src/dec/tree_dec.c",
        "src/dec/vp8_dec.c",
        "src/dec/vp8l_dec.c",
        "src/dec/webp_dec.c",
        "src/dsp/alpha_processing.c",
        "src/dsp/cpu.c",
        "src/dsp/dec.c",
        "src/dsp/dec_clip_tables.c",
        "src/dsp/filters.c",
        "src/dsp/lossless.c",
        "src/dsp/rescaler.c",
        "src/dsp/upsampling.c",
        "src/dsp/yuv.c",
        "src/utils/bit_reader_utils.c",
        "src/utils/color_cache_utils.c",
        "src/utils/huffman_utils.c",
        "src/utils/palette.c",
        "src/utils/quant_levels_dec_utils.c",
        "src/utils/random_utils.c",
        "src/utils/rescaler_utils.c",
        "src/utils/thread_utils.c",
        "src/utils/utils.c",
    }

if USE_SIMD ~= "none" then
    -- libwebp will automatically detect and use available SIMD at runtime when HAVE_CONFIG_H is not defined
    filter { "architecture:x86 or x86_64" }
        files {
            "src/dsp/alpha_processing_sse2.c",
            "src/dsp/dec_sse2.c",
            "src/dsp/filters_sse2.c",
            "src/dsp/lossless_sse2.c",
            "src/dsp/rescaler_sse2.c",
            "src/dsp/upsampling_sse2.c",
            "src/dsp/yuv_sse2.c",
            "src/dsp/alpha_processing_sse41.c",
            "src/dsp/dec_sse41.c",
            "src/dsp/lossless_sse41.c",
            "src/dsp/upsampling_sse41.c",
            "src/dsp/yuv_sse41.c",
            "src/dsp/lossless_avx2.c",
        }

    filter { "architecture:AARCH64" }
        files {
            "src/dsp/alpha_processing_neon.c",
            "src/dsp/dec_neon.c",
            "src/dsp/filters_neon.c",
            "src/dsp/lossless_neon.c",
            "src/dsp/rescaler_neon.c",
            "src/dsp/upsampling_neon.c",
            "src/dsp/yuv_neon.c",
        }
else
    -- Include a dummy config file to disable automatic SIMD detection
    defines { "HAVE_CONFIG_H" }
end
