#ifndef UTILS_H
#define UTILS_H

#include <irrlicht.h>
#include <string>
#include <vector>
#include <functional>
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
		static bool ClearDirectory(const std::string& path);
		static bool ClearDirectory(const std::wstring& path);
		static bool Deletedirectory(const std::string& source);
		static bool Deletedirectory(const std::wstring& source);
		static void CreateResourceFolders();
		static void takeScreenshot(irr::IrrlichtDevice* device);
		static void changeCursor(irr::gui::ECURSOR_ICON icon);
		static void FindfolderFiles(const std::wstring & path, const std::function<void(std::wstring, bool, void*)>& cb, void* = nullptr);
		static std::vector<std::wstring> FindfolderFiles(const std::wstring& path, std::vector<std::wstring> extensions, int subdirectorylayers = 0);
		static std::wstring NormalizePath(const std::wstring& path, bool trailing_slash = true);
	};
}

#endif //UTILS_H
