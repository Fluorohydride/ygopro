#ifndef YGOPRO_CONFIG_H
#define YGOPRO_CONFIG_H

#define IRR_COMPILE_WITH_DX9_DEV_PACK

#include <cerrno>
#include <cstdio>
#include <string>
#include "bufferio.h"
#include "../ocgcore/ocgapi.h"

#ifdef _WIN32

#if defined(_MSC_VER) || defined(__MINGW32__)
#define mywcsncasecmp _wcsnicmp
#define mystrncasecmp _strnicmp
#else
#define mywcsncasecmp wcsncasecmp
#define mystrncasecmp strncasecmp
#endif

#else //_WIN32

#define mywcsncasecmp wcsncasecmp
#define mystrncasecmp strncasecmp

#endif // _WIN32

template<size_t N, typename... TR>
inline int myswprintf(wchar_t(&buf)[N], const wchar_t* fmt, TR... args) {
	return std::swprintf(buf, N, fmt, args...);
}
template<size_t N, typename... TR>
inline int mysnprintf(char(&buf)[N], const char* fmt, TR... args) {
	return std::snprintf(buf, N, fmt, args...);
}
template<typename T>
inline T myclamp(T v, T lo, T hi) {
	return (v < lo) ? lo : (hi < v) ? hi : v;
}

#include <irrlicht.h>

constexpr uint16_t PRO_VERSION = 0x1362;

#endif // YGOPRO_CONFIG_H
