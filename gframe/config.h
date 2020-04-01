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

#define EDOPRO_VERSION_MAJOR 37
#define EDOPRO_VERSION_MINOR 3
#define EDOPRO_VERSION_PATCH 0
#define EDOPRO_VERSION_CODENAME L"Spider Shark"
#define CLIENT_VERSION (EDOPRO_VERSION_MAJOR & 0xff | ((EDOPRO_VERSION_MINOR & 0xff) << 8) | ((OCG_VERSION_MAJOR & 0xff) << 16) | ((OCG_VERSION_MINOR & 0xff) << 24))
#define EXPAND_VERSION(ver) (ver) & 0xff, (((ver) >> 8) & 0xff), (((ver) >> 16) & 0xff), (((ver) >> 24) & 0xff)

#endif
