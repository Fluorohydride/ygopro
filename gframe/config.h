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

#endif
