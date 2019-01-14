#ifndef UTILS_H
#define UTILS_H

#include "config.h"
#include <fstream>
#include <vector>
#ifndef _WIN32
#include <dirent.h>
#include <sys/stat.h>
#endif

namespace ygo {
	class Utils {
	public:
		static bool Makedirectory(const std::wstring& path);
		static bool Makedirectory(const std::string& path);
		static bool Movefile(const std::string& source, const std::string& destination);
		static bool Movefile(const std::wstring& source, const std::wstring& destination);
		static bool Deletefile(const std::string& source);
		static bool Deletefile(const std::wstring& source);
		static void CreateResourceFolders();
		static void takeScreenshot(irr::IrrlichtDevice* device);
		static void changeCursor(irr::gui::ECURSOR_ICON icon);
		static std::vector<std::wstring> FindfolderFiles(const std::wstring& path, std::vector<std::wstring> extensions, int subdirectorylayers = 0);
	};
}

#endif //UTILS_H
