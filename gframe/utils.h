#pragma once
#ifndef UTILS_H
#define UTILS_H

#include "config.h"
#include <fstream>
#ifndef _WIN32
#include <sys/stat.h>
#endif

namespace ygo {
	class Utils {
	public:
		static bool Makedirectory(const std::wstring& path);
		static bool Makedirectory(const std::string& path);
		static bool Movefile(const std::string& source, const std::string& destination);
		static bool Movefile(const std::wstring& source, const std::wstring& destination);
		void initUtils();
		void takeScreenshot(irr::IrrlichtDevice* device);
		void changeCursor(ECURSOR_ICON icon);
	};
	extern Utils utils;
}

#endif //UTILS_H
