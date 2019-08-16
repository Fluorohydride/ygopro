#ifndef UTILS_H
#define UTILS_H

#include <irrlicht.h>
#include <string>
#include <vector>
#include <functional>
#include <fstream>
#include <map>
#ifndef _WIN32
#include <dirent.h>
#include <sys/stat.h>
#endif

#ifdef _WIN32
using path_string = std::wstring;
#else
using path_string = std::string;
#endif

namespace ygo {
	class Utils {
	public:
		class IrrArchiveHelper {
		public:
			irr::io::IFileArchive* archive;
			std::map<path_string/*folder name*/, std::pair<std::pair<int/*begin folder offset*/, int/*end folder offset*/>, std::pair<int/*begin file offset*/, int/*end file offset*/>>> folderindexes;
			IrrArchiveHelper(irr::io::IFileArchive* archive) { ParseList(archive); };
			void ParseList(irr::io::IFileArchive* archive);
		};
		static bool Makedirectory(const std::wstring& path);
		static bool Makedirectory(const std::string& path);
		template<typename T, typename T2>
		static bool Movefile(const T& source, const T2& destination);
		static path_string ParseFilename(const std::wstring& input);
		static path_string ParseFilename(const std::string& input);
		static bool Deletefile(const std::string& source);
		static bool Deletefile(const std::wstring& source);
		static bool ClearDirectory(const std::string& path);
		static bool ClearDirectory(const std::wstring& path);
		static bool Deletedirectory(const std::string& source);
		static bool Deletedirectory(const std::wstring& source);
		static void CreateResourceFolders();
		static void takeScreenshot(irr::IrrlichtDevice* device);
		static void ToggleFullscreen();
		static void changeCursor(irr::gui::ECURSOR_ICON icon);
		static void FindfolderFiles(const std::wstring & path, const std::function<void(std::wstring, bool, void*)>& cb, void* payload = nullptr);
		static std::vector<std::wstring> FindfolderFiles(const std::wstring& path, std::vector<std::wstring> extensions, int subdirectorylayers = 0);
		static void FindfolderFiles(IrrArchiveHelper& archive, const path_string& path, const std::function<bool(int, path_string, bool, void*)>& cb, void* payload = nullptr);
		static std::vector<int> FindfolderFiles(IrrArchiveHelper& archive, const path_string& path, std::vector<path_string> extensions, int subdirectorylayers = 0);
		static irr::io::IReadFile* FindandOpenFileFromArchives(const path_string& path, const path_string& name);
		static std::wstring NormalizePath(std::wstring path, bool trailing_slash = true);
		static std::wstring GetFileExtension(std::wstring file);
		static std::wstring GetFileName(std::wstring file);
		static std::string NormalizePath(std::string path, bool trailing_slash = true);
		static std::string GetFileExtension(std::string file);
		static std::string GetFileName(std::string file);
	};
	template<typename T, typename T2>
	bool Utils::Movefile(const T& _source, const T2& _destination) {
		path_string source = ParseFilename(_source);
		path_string destination = ParseFilename(_destination);
		if(source == destination)
			return false;
		std::ifstream  src(source, std::ios::binary);
		if(!src.is_open())
			return false;
		std::ofstream  dst(destination, std::ios::binary);
		if(!dst.is_open())
			return false;
		dst << src.rdbuf();
		src.close();
		Deletefile(source);
		return true;
	}
}

#endif //UTILS_H
