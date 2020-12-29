#ifndef __CONFIG_H
#define __CONFIG_H

#include "ocgapi_types.h"
#include "text_types.h"

extern uint16_t PRO_VERSION;
extern bool exit_on_return;
extern bool is_from_discord;
extern bool open_file;
extern epro::path_string open_file_name;
extern bool show_changelog;

#define EDOPRO_VERSION_MAJOR 39
#define EDOPRO_VERSION_MINOR 0
#define EDOPRO_VERSION_PATCH 2
#define EDOPRO_VERSION_CODENAME L"Utopia"
#define CLIENT_VERSION (EDOPRO_VERSION_MAJOR & 0xff | ((EDOPRO_VERSION_MINOR & 0xff) << 8) | ((OCG_VERSION_MAJOR & 0xff) << 16) | ((OCG_VERSION_MINOR & 0xff) << 24))
#define EXPAND_VERSION(ver) (ver) & 0xff, (((ver) >> 8) & 0xff), (((ver) >> 16) & 0xff), (((ver) >> 24) & 0xff)

#if defined(_WIN32)
#define OSSTRING "Windows"
#elif defined(__APPLE__)
#define OSSTRING "Mac"
#elif defined (__linux__) && !defined(__ANDROID__)
#define OSSTRING "Linux"
#else
#define OSSTRING "Android"
#endif

#endif
