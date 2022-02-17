#ifndef __CONFIG_H
#define __CONFIG_H

#include "ocgapi_types.h"
#include "text_types.h"

extern uint16_t PRO_VERSION;
extern bool is_from_discord;
extern bool open_file;
extern epro::path_string open_file_name;
extern bool show_changelog;

#ifdef _MSC_VER
#define unreachable() __assume(0)
#else
#define unreachable() __builtin_unreachable()
#endif

#define EDOPRO_VERSION_MAJOR 39
#define EDOPRO_VERSION_MINOR 3
#define EDOPRO_VERSION_PATCH 2
#define EDOPRO_VERSION_CODENAME "Utopia"
#define EDOPRO_VERSION_STRING_DEBUG "EDOPro version " STR(EDOPRO_VERSION_MAJOR) "." STR(EDOPRO_VERSION_MINOR) "." STR(EDOPRO_VERSION_PATCH)
#define EDOPRO_VERSION_STRING L"Project Ignis: EDOPro | " STR(EDOPRO_VERSION_MAJOR) "." STR(EDOPRO_VERSION_MINOR) "." STR(EDOPRO_VERSION_PATCH) " \"" EDOPRO_VERSION_CODENAME "\""
#define CLIENT_VERSION (EDOPRO_VERSION_MAJOR & 0xff | ((EDOPRO_VERSION_MINOR & 0xff) << 8) | ((OCG_VERSION_MAJOR & 0xff) << 16) | ((OCG_VERSION_MINOR & 0xff) << 24))
#define EXPAND_VERSION(ver) (ver) & 0xff, (((ver) >> 8) & 0xff), (((ver) >> 16) & 0xff), (((ver) >> 24) & 0xff)

#if defined(__APPLE__)
#include <TargetConditionals.h>
#if TARGET_OS_IOS == 1
#define EDOPRO_IOS
#else
#define EDOPRO_MACOS
#endif
#endif

#if defined(_WIN32)
#define OSSTRING "Windows"
#elif defined(EDOPRO_MACOS)
#define OSSTRING "Mac"
#elif defined(EDOPRO_IOS)
#define OSSTRING "iOS"
#elif defined (__linux__) && !defined(__ANDROID__)
#define OSSTRING "Linux"
#else
#define OSSTRING "Android"
#endif

#endif
