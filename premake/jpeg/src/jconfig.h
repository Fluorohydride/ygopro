/*
  Minimal configuration header for building libjpeg-turbo from source
  (without running libjpeg-turbo's CMake to generate it).

  This file is based on libjpeg-turbo's src/jconfig.h.in.
*/

/* Version ID for the JPEG library.
 * Irrlicht 1.8 uses Version 8d of the IJG libjpeg, so we set the version to 8.0 to avoid confusion.
 * JPEG_LIB_VERSION_MAJOR and JPEG_LIB_VERSION_MINOR are not used by libjpeg-turbo.
 */
#define JPEG_LIB_VERSION  80

/* Support arithmetic encoding/decoding when using 8-bit samples */
#define C_ARITH_CODING_SUPPORTED  1
#define D_ARITH_CODING_SUPPORTED  1

/* Support in-memory source/destination managers */
#define MEM_SRCDST_SUPPORTED  1

/* Use accelerated SIMD routines when available.
  We enable this only for Windows x86/x64, where the premake build also
  assembles the NASM SIMD objects. */
#if defined(_WIN32) && (defined(_M_IX86) || defined(_M_X64) || defined(__i386__) || defined(__x86_64__))
#define WITH_SIMD  1
#endif

/* See libjpeg-turbo comments: define BITS_IN_JSAMPLE for downstream code. */
#ifndef BITS_IN_JSAMPLE
#define BITS_IN_JSAMPLE  8
#endif

#ifdef _WIN32

#undef RIGHT_SHIFT_IS_UNSIGNED

/* Define "boolean" as unsigned char, not int, per Windows custom */
#ifndef __RPCNDR_H__
typedef unsigned char boolean;
#endif
#define HAVE_BOOLEAN

/* Define "INT32" as int, not long, per Windows custom */
#if !(defined(_BASETSD_H_) || defined(_BASETSD_H))
typedef short INT16;
typedef signed int INT32;
#endif
#define XMD_H

#else

/* Define if your (broken) compiler shifts signed values as if they were unsigned. */
/* #define RIGHT_SHIFT_IS_UNSIGNED 1 */

#endif
