#include "utils.h"
#include "game.h"
#include <fstream>
#include "bufferio.h"
namespace ygo {
	Utils utils;
#ifdef _WIN32
	bool Utils::Makedirectory(const std::wstring& path) {
		return CreateDirectory(path.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError();
	}
	bool Utils::Makedirectory(const std::string& path) {
		return Makedirectory(BufferIO::DecodeUTF8s(path));
	}
#else
	bool Utils::Makedirectory(const std::wstring& path) {
		return Makedirectory(BufferIO::EncodeUTF8s(path));
	}
	bool Utils::Makedirectory(const std::string& path) {
		return !mkdir(&path[0], 0777) || errno == EEXIST;
	}
#endif
	bool Utils::Movefile(const std::string& source, const std::string& destination) {
#ifndef _WIN32
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
#else
	return Movefile(BufferIO::DecodeUTF8s(source), BufferIO::DecodeUTF8s(destination));
#endif
	}
	bool Utils::Movefile(const std::wstring& source, const std::wstring& destination) {
#ifdef _WIN32
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
#else
		return Movefile(BufferIO::EncodeUTF8s(source), BufferIO::EncodeUTF8s(destination));
#endif
	}
	bool Utils::Deletefile(const std::string & source) {
#ifdef _WIN32
		return Deletefile(BufferIO::DecodeUTF8s(source));
#else
		return remove(source.c_str()) == 0;
#endif
	}
	bool Utils::Deletefile(const std::wstring & source) {
#ifdef _WIN32
		return DeleteFile(source.c_str());
#else
		return Deletefile(BufferIO::EncodeUTF8s(source));
#endif
	}
	bool Utils::ClearDirectory(const std::string & path) {
#ifdef _WIN32
		return ClearDirectory(BufferIO::DecodeUTF8s(path));
#else
		DIR * dir;
		struct dirent * dirp = nullptr;
		if((dir = opendir(path.c_str())) != nullptr) {
			struct stat fileStat;
			while((dirp = readdir(dir)) != nullptr) {
				stat((path + dirp->d_name).c_str(), &fileStat);
				std::string name = dirp->d_name;
				if(S_ISDIR(fileStat.st_mode)) {
					if(name == ".." || name == ".") {
						continue;
					}
					Deletedirectory(path + name + "/");
					continue;
				} else {
					Deletefile(path + name);
				}
			}
			closedir(dir);
		}
		return true;
#endif
	}
	bool Utils::ClearDirectory(const std::wstring & path) {
#ifdef _WIN32
		WIN32_FIND_DATAW fdataw;
		HANDLE fh = FindFirstFileW((path + L"*.*").c_str(), &fdataw);
		if(fh != INVALID_HANDLE_VALUE) {
			do {
				std::wstring name = fdataw.cFileName;
				if(fdataw.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					if(name == L".." || name == L".") {
						continue;
					}
					Deletedirectory(path + name + L"/");
					continue;
				} else {
					Deletefile(path + name);
				}
			} while(FindNextFileW(fh, &fdataw));
			FindClose(fh);
		}
		return true;
#else
		return ClearDirectory(BufferIO::EncodeUTF8s(path));
#endif
	}
	bool Utils::Deletedirectory(const std::string & source) {
#ifdef _WIN32
		return Deletedirectory(BufferIO::DecodeUTF8s(source));
#else
		ClearDirectory(source);
		return rmdir(source.c_str()) == 0;
#endif
	}
	bool Utils::Deletedirectory(const std::wstring & source) {
#ifdef _WIN32
		ClearDirectory(source);
		return RemoveDirectory(source.c_str());
#else
		return Deletedirectory(BufferIO::EncodeUTF8s(source));
#endif
	}
	void Utils::CreateResourceFolders() {
		//create directories if missing
		Makedirectory(TEXT("deck"));
		Makedirectory(TEXT("puzzles"));
		Makedirectory(TEXT("pics"));
		Makedirectory(TEXT("pics/field"));
		Makedirectory(TEXT("pics/temp/"));
		ClearDirectory(TEXT("pics/temp/"));
		Makedirectory(TEXT("replay"));
		Makedirectory(TEXT("screenshots"));
	}

	void Utils::takeScreenshot(irr::IrrlichtDevice* device)
	{
		irr::video::IVideoDriver* const driver = device->getVideoDriver();

		//get image from the last rendered frame 
		irr::video::IImage* const image = driver->createScreenShot();
		if (image) //should always be true, but you never know. ;) 
		{
			//construct a filename, consisting of local time and file extension 
			irr::c8 filename[64];
			snprintf(filename, 64, "screenshots/ygopro_%u.png", device->getTimer()->getRealTime());

			//write screenshot to file 
			if (!driver->writeImageToFile(image, filename))
				device->getLogger()->log(L"Failed to take screenshot.", irr::ELL_WARNING);

			//Don't forget to drop image since we don't need it anymore. 
			image->drop();
		}
	}

	void Utils::changeCursor(irr::gui::ECURSOR_ICON icon) {
		irr::gui::ICursorControl* cursor = mainGame->device->getCursorControl();
		if (cursor->getActiveIcon() != icon) {
			cursor->setActiveIcon(icon);
		}
	}
	void Utils::FindfolderFiles(const std::wstring & path, const std::function<void(std::wstring, bool, void*)>& cb, void* payload) {
#ifdef _WIN32
		WIN32_FIND_DATAW fdataw;
		HANDLE fh = FindFirstFileW((NormalizePath(path) + L"*.*").c_str(), &fdataw);
		if(fh != INVALID_HANDLE_VALUE) {
			do {
				cb(fdataw.cFileName, !!(fdataw.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY), payload);
			} while(FindNextFileW(fh, &fdataw));
			FindClose(fh);
		}
#else
		DIR * dir;
		struct dirent * dirp = nullptr;
		if((dir = opendir(BufferIO::EncodeUTF8s(NormalizePath(path)).c_str())) != nullptr) {
			struct stat fileStat;
			while((dirp = readdir(dir)) != nullptr) {
				cb(BufferIO::DecodeUTF8s(dirp->d_name), !!S_ISDIR(fileStat.st_mode), payload);
			}
			closedir(dir);
		}
#endif
	}
	std::vector<std::wstring> Utils::FindfolderFiles(const std::wstring & path, std::vector<std::wstring> extensions, int subdirectorylayers) {
		std::vector<std::wstring> res;
		FindfolderFiles(path, [&res, extensions, path, subdirectorylayers](std::wstring name, bool isdir, void* payload) {
			if(isdir) {
				if(subdirectorylayers) {
					if(name == L".." || name == L".") {
						return;
					}
					std::vector<std::wstring> res2 = FindfolderFiles(path + name + L"/", extensions, subdirectorylayers - 1);
					for(auto&file : res2) {
						file = name + L"/" + file;
					}
					res.insert(res.end(), res2.begin(), res2.end());
				}
				return;
			} else {
				if(extensions.size() && std::find(extensions.begin(), extensions.end(), Utils::GetFileExtension(name)) == extensions.end())
					return;
				res.push_back(name.c_str());
			}
		});
		return res;
	}
	std::wstring Utils::NormalizePath(const std::wstring & path, bool trailing_slash) {
		std::vector<std::wstring> paths = ygo::Game::TokenizeString(path, L"/");
		if(paths.empty())
			return path;
		std::wstring normalpath;
		for(auto it = paths.begin(); it != paths.end();) {
			if((*it).empty()) {
				it = paths.erase(it);
				continue;
			}
			if((*it) == L"." && it != paths.begin()) {
				it = paths.erase(it);
				continue;
			}
			if((*it) != L".." && it != paths.begin() && (it + 1) != paths.end() && (*(it + 1)) == L"..") {
				it = paths.erase(paths.erase(it));
				continue;
			}
			it++;
		}
		for(auto it = paths.begin(); it != (paths.end() - 1); it++) {
			normalpath += *it + L"/";
		}
		normalpath += paths.back();
		if(trailing_slash)
			normalpath += L"/";
		return normalpath;
	}
	std::wstring Utils::GetFileExtension(const std::wstring & file) {
		size_t dotpos = file.find_last_of(L".");
		if(dotpos == std::wstring::npos)
			return file;
		std::wstring extension = file.substr(dotpos + 1);
		std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
		return extension;
	}
	std::wstring Utils::GetFileName(const std::wstring & file) {
		auto dash1 = file.find_last_of(L"\\");
		auto dash2 = file.find_last_of(L"/");
		size_t dash;
		if(dash1 == std::wstring::npos && dash2 == std::wstring::npos)
			dash = 0;
		else if(dash1 != std::wstring::npos && dash2 != std::wstring::npos)
			dash = std::max(dash1, dash2) + 1;
		else
			dash = std::min(dash1, dash2) + 1;
		size_t dotpos = file.find_last_of(L".");
		if(dotpos == std::wstring::npos)
			dotpos = 0;
		std::wstring name = file.substr(dash, dotpos - dash);
		return name;
	}
}

