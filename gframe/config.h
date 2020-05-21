#ifndef __CONFIG_H
#define __CONFIG_H

#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include "bufferio.h"
#include <mutex>
#include "mysignal.h"
#include "common.h"
#include <fmt/format.h>
#include <fmt/printf.h>
#include "utils.h"

extern unsigned short PRO_VERSION;
extern bool exit_on_return;
extern bool is_from_discord;
extern bool open_file;
extern path_string open_file_name;
extern bool show_changelog;

#define EDOPRO_VERSION_MAJOR 38
#define EDOPRO_VERSION_MINOR 1
#define EDOPRO_VERSION_PATCH 2
#define EDOPRO_VERSION_CODENAME L"Hope Harbinger"
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
// Double macro to convert the macro-defined int to a character literal
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define EDOPRO_USERAGENT "EDOPro-" OSSTRING "-" STR(EDOPRO_VERSION_MAJOR) "." STR(EDOPRO_VERSION_MINOR) "." STR(EDOPRO_VERSION_PATCH)

#endif
