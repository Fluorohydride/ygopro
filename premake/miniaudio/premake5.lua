project "miniaudio"
    kind "StaticLib"
    files { "miniaudio.c", "miniaudio.h" }
    defines { "MA_NO_ENCODING", "MA_NO_GENERATION", "MA_NO_NEON" }

    if MINIAUDIO_SUPPORT_OPUS_VORBIS then
        files { "extras/decoders/libopus/*", "extras/decoders/libvorbis/*" }
        if MINIAUDIO_BUILD_OPUS_VORBIS then
            files {
                "external/ogg/src/bitwise.c",
                "external/ogg/src/framing.c",

                "external/opus/src/opus.c",
                "external/opus/src/opus_decoder.c",
                "external/opus/src/opus_multistream.c",
                "external/opus/src/opus_multistream_decoder.c",

                "external/opus/celt/bands.c",
                "external/opus/celt/celt.c",
                "external/opus/celt/celt_decoder.c",
                "external/opus/celt/celt_lpc.c",
                "external/opus/celt/cwrs.c",
                "external/opus/celt/entcode.c",
                "external/opus/celt/entdec.c",
                "external/opus/celt/entenc.c",
                "external/opus/celt/kiss_fft.c",
                "external/opus/celt/laplace.c",
                "external/opus/celt/mathops.c",
                "external/opus/celt/mdct.c",
                "external/opus/celt/modes.c",
                "external/opus/celt/pitch.c",
                "external/opus/celt/quant_bands.c",
                "external/opus/celt/rate.c",
                "external/opus/celt/vq.c",

                "external/opus/silk/bwexpander.c",
                "external/opus/silk/bwexpander_32.c",
                "external/opus/silk/CNG.c",
                "external/opus/silk/code_signs.c",
                "external/opus/silk/dec_API.c",
                "external/opus/silk/decode_core.c",
                "external/opus/silk/decode_frame.c",
                "external/opus/silk/decode_indices.c",
                "external/opus/silk/decode_parameters.c",
                "external/opus/silk/decode_pitch.c",
                "external/opus/silk/decode_pulses.c",
                "external/opus/silk/decoder_set_fs.c",
                "external/opus/silk/gain_quant.c",
                "external/opus/silk/init_decoder.c",
                "external/opus/silk/lin2log.c",
                "external/opus/silk/log2lin.c",
                "external/opus/silk/LPC_analysis_filter.c",
                "external/opus/silk/LPC_fit.c",
                "external/opus/silk/LPC_inv_pred_gain.c",
                "external/opus/silk/NLSF_decode.c",
                "external/opus/silk/NLSF_stabilize.c",
                "external/opus/silk/NLSF_unpack.c",
                "external/opus/silk/NLSF2A.c",
                "external/opus/silk/pitch_est_tables.c",
                "external/opus/silk/PLC.c",
                "external/opus/silk/resampler.c",
                "external/opus/silk/resampler_private_AR2.c",
                "external/opus/silk/resampler_private_down_FIR.c",
                "external/opus/silk/resampler_private_IIR_FIR.c",
                "external/opus/silk/resampler_private_up2_HQ.c",
                "external/opus/silk/resampler_rom.c",
                "external/opus/silk/shell_coder.c",
                "external/opus/silk/sort.c",
                "external/opus/silk/stereo_decode_pred.c",
                "external/opus/silk/stereo_MS_to_LR.c",
                "external/opus/silk/sum_sqr_shift.c",
                "external/opus/silk/table_LSF_cos.c",
                "external/opus/silk/tables_gain.c",
                "external/opus/silk/tables_LTP.c",
                "external/opus/silk/tables_NLSF_CB_NB_MB.c",
                "external/opus/silk/tables_NLSF_CB_WB.c",
                "external/opus/silk/tables_other.c",
                "external/opus/silk/tables_pitch_lag.c",
                "external/opus/silk/tables_pulses_per_block.c",

                "external/opusfile/src/info.c",
                "external/opusfile/src/internal.c",
                "external/opusfile/src/opusfile.c",
                "external/opusfile/src/stream.c",

                "external/vorbis/lib/bitrate.c",
                "external/vorbis/lib/block.c",
                "external/vorbis/lib/codebook.c",
                "external/vorbis/lib/envelope.c",
                "external/vorbis/lib/floor0.c",
                "external/vorbis/lib/floor1.c",
                "external/vorbis/lib/info.c",
                "external/vorbis/lib/lpc.c",
                "external/vorbis/lib/lsp.c",
                "external/vorbis/lib/mapping0.c",
                "external/vorbis/lib/mdct.c",
                "external/vorbis/lib/psy.c",
                "external/vorbis/lib/registry.c",
                "external/vorbis/lib/res0.c",
                "external/vorbis/lib/sharedbook.c",
                "external/vorbis/lib/smallft.c",
                "external/vorbis/lib/synthesis.c",
                "external/vorbis/lib/vorbisfile.c",
                "external/vorbis/lib/window.c",
            }
            includedirs {
                "external/ogg/include",
                "external/opus/include",
                "external/opus/celt",
                "external/opus/silk",
                "external/opusfile/include",
                "external/vorbis/include",
            }
            defines {
                "OPUS_BUILD",
                "USE_ALLOCA",
                "HAVE_LRINTF",
                "OP_HAVE_LRINTF",
            }
            if not TARGET_MAC_ARM then
                files {
                    "external/opus/celt/x86/pitch_avx.c",
                    "external/opus/celt/x86/pitch_sse.c",
                    "external/opus/celt/x86/vq_sse2.c",
                    "external/opus/celt/x86/x86_celt_map.c",
                    "external/opus/celt/x86/x86cpu.c",
                }
                defines {
                    "OPUS_HAVE_RTCD", "CPU_INFO_BY_ASM",
                    "OPUS_X86_PRESUME_SSE", "OPUS_X86_PRESUME_SSE2",
                    "OPUS_X86_MAY_HAVE_SSE", "OPUS_X86_MAY_HAVE_SSE4_1", "OPUS_X86_MAY_HAVE_AVX2",
                }
            end
        else
            includedirs { OPUS_INCLUDE_DIR, OPUSFILE_INCLUDE_DIR, VORBIS_INCLUDE_DIR, OGG_INCLUDE_DIR }
        end
    end

    filter "system:linux"
        links { "dl", "pthread", "m" }
