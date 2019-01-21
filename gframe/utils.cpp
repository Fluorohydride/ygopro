#include "utils.h"
#include "game.h"

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
		return remove(source.c_str()) == 0;
	}
	bool Utils::Deletefile(const std::wstring & source) {
		return Deletefile(BufferIO::EncodeUTF8s(source));
	}
	void Utils::CreateResourceFolders() {
		//create directories if missing
		Makedirectory("deck");
		Makedirectory("pics");
		Makedirectory("pics/field");
		Makedirectory("replay");
		Makedirectory("screenshots");
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
	std::vector<std::wstring> Utils::FindfolderFiles(const std::wstring & path, std::vector<std::wstring> extensions, int subdirectorylayers) {
		std::vector<std::wstring> res;
#ifdef _WIN32
		WIN32_FIND_DATAW fdataw;
		HANDLE fh = FindFirstFileW((path + L"*.*").c_str(), &fdataw);
		if(fh != INVALID_HANDLE_VALUE) {
			do {
				std::wstring name = fdataw.cFileName;
				if(fdataw.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					if(subdirectorylayers) {
						if(name == L".." || name == L".") {
							continue;
						}
						std::vector<std::wstring> res2 = FindfolderFiles(path + name + L"/", extensions, subdirectorylayers - 1);
						for(auto&file : res2) {
							file = name + L"/" + file;
						}
						res.insert(res.end(), res2.begin(), res2.end());
					}
					continue;
				}
				if(!(fdataw.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
					size_t dotpos = name.find_last_of(L".");
					if(dotpos == std::wstring::npos)
						continue;
					auto extension = name.substr(dotpos + 1);
					std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
					if(extensions.size() && std::find(extensions.begin(), extensions.end(), extension) == extensions.end())
						continue;
					res.push_back(name.c_str());
				}
			} while(FindNextFileW(fh, &fdataw));
			FindClose(fh);
		}
#else
		DIR * dir;
		struct dirent * dirp = nullptr;
		std::string _path = BufferIO::EncodeUTF8s(path);
		if((dir = opendir(_path.c_str())) != nullptr) {
			struct stat fileStat;
			while((dirp = readdir(dir)) != nullptr) {
				stat((_path + dirp->d_name).c_str(), &fileStat);
				std::wstring name = BufferIO::DecodeUTF8s(dirp->d_name);
				if(S_ISDIR(fileStat.st_mode)) {
					if(subdirectorylayers) {
						if(name == L".." || name == L".")
							continue;
						std::vector<std::wstring> res2 = FindfolderFiles(path + name + L"/", extensions, subdirectorylayers - 1);
						for(auto&file : res2) {
							file = name + L"/" + file;
						}
						res.insert(res.end(), res2.begin(), res2.end());
					}
					continue;
				}
				size_t dotpos = name.find_last_of(L".");
				if(dotpos == std::wstring::npos)
					continue;
				auto extension = name.substr(dotpos + 1);
				std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
				if(std::find(extensions.begin(), extensions.end(), extension) == extensions.end())
					continue;
				res.push_back(name.c_str());
			}
			closedir(dir);
		}
#endif
		return res;
	}
}