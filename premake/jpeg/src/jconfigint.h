/*
  Minimal internal configuration header for building libjpeg-turbo from source
  (without running libjpeg-turbo's CMake to generate it).

  This file is based on libjpeg-turbo's src/jconfigint.h.in.
*/

/* libjpeg-turbo build number (unknown in this integration) */
#define BUILD  "0"

/* How to hide global symbols. */
#define HIDDEN

/* Compiler's inline keyword */
#undef inline

/* How to obtain function inlining. */
#if defined(_MSC_VER)
#define INLINE  __inline
#elif defined(__GNUC__) || defined(__clang__)
#define INLINE  __inline__
#else
#define INLINE
#endif

/* How to obtain thread-local storage */
#if defined(_MSC_VER)
#define THREAD_LOCAL  __declspec(thread)
#elif defined(__GNUC__) || defined(__clang__)
#define THREAD_LOCAL  __thread
#else
#define THREAD_LOCAL
#endif

/* Define to the full name of this package. */
#define PACKAGE_NAME  "libjpeg-turbo"

/* Version number of package, not actually used by any logic in libjpeg-turbo. */
#define VERSION  "YGOPro"

/* The size of `size_t', as computed by sizeof. */
#if defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__) || defined(__aarch64__)
#define SIZEOF_SIZE_T  8
#else
#define SIZEOF_SIZE_T  4
#endif

#if defined(__GNUC__) || defined(__clang__)
#define HAVE_BUILTIN_CTZL
#endif

/* Define to 1 if you have the <intrin.h> header file. */
#if defined(_MSC_VER)
#define HAVE_INTRIN_H
#endif

#if defined(_MSC_VER) && defined(HAVE_INTRIN_H)
#if (SIZEOF_SIZE_T == 8)
#define HAVE_BITSCANFORWARD64
#elif (SIZEOF_SIZE_T == 4)
#define HAVE_BITSCANFORWARD
#endif
#endif

#if defined(__has_attribute)
#if __has_attribute(fallthrough)
#define FALLTHROUGH  __attribute__((fallthrough));
#else
#define FALLTHROUGH
#endif
#else
#define FALLTHROUGH
#endif

/*
 * Define BITS_IN_JSAMPLE as either
 *   8   for 8-bit sample values (the usual setting)
 *   12  for 12-bit sample values
 */
#ifndef BITS_IN_JSAMPLE
#define BITS_IN_JSAMPLE  8
#endif

#undef C_ARITH_CODING_SUPPORTED
#undef D_ARITH_CODING_SUPPORTED
#undef WITH_SIMD

#if BITS_IN_JSAMPLE == 8

/* Support arithmetic encoding/decoding */
#define C_ARITH_CODING_SUPPORTED  1
#define D_ARITH_CODING_SUPPORTED  1

/* Use accelerated SIMD routines when available.
  We enable this only for Windows x86/x64, where the premake build also
  assembles the NASM SIMD objects. */
#if defined(_WIN32) && (defined(_M_IX86) || defined(_M_X64) || defined(__i386__) || defined(__x86_64__))
#define WITH_SIMD  1
#endif

#endif
