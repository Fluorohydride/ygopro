#include "utils.h"
#include "game.h"
#include <fstream>
#include "bufferio.h"
#ifdef _WIN32
#include "../irrlicht/src/CIrrDeviceWin32.h"
#endif
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

	void Utils::ToggleFullscreen() {
#ifdef _WIN32
		static RECT nonFullscreenSize;
		static std::vector<RECT> monitors;
		static bool maximized = false;
		if(monitors.empty()) {
			EnumDisplayMonitors(0, 0, [](HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData) -> BOOL {
				auto monitors = reinterpret_cast<std::vector<RECT>*>(pData);
				monitors->push_back(*lprcMonitor);
				return TRUE;
			}, (LPARAM)&monitors);
		}
		mainGame->is_fullscreen = !mainGame->is_fullscreen;
		HWND hWnd;
		irr::video::SExposedVideoData exposedData = mainGame->driver->getExposedVideoData();
		if(mainGame->driver->getDriverType() == irr::video::EDT_DIRECT3D9)
			hWnd = reinterpret_cast<HWND>(exposedData.D3D9.HWnd);
		else
			hWnd = reinterpret_cast<HWND>(exposedData.OpenGLWin32.HWnd);
		LONG_PTR style = WS_POPUP;
		RECT clientSize = {};
		if(mainGame->is_fullscreen) {
			if(GetWindowLong(hWnd, GWL_STYLE) & WS_MAXIMIZE) {
				maximized = true;
			}
			GetWindowRect(hWnd, &nonFullscreenSize);
			style = WS_SYSMENU | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
			for(auto& rect : monitors) {
				POINT windowCenter = { (nonFullscreenSize.left + (nonFullscreenSize.right - nonFullscreenSize.left) / 2), (nonFullscreenSize.top + (nonFullscreenSize.bottom - nonFullscreenSize.top) / 2) };
				if(PtInRect(&rect, windowCenter)) {
					clientSize = rect;
					break;
				}
			}
		} else {
			style = WS_THICKFRAME | WS_SYSMENU | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
			clientSize = nonFullscreenSize;
			if(maximized) {
				style |= WS_MAXIMIZE;
				maximized = false;
			}
		}

		if(!SetWindowLongPtr(hWnd, GWL_STYLE, style))
			mainGame->ErrorLog("Could not change window style.");

		const s32 width = clientSize.right - clientSize.left;
		const s32 height = clientSize.bottom - clientSize.top;

		SetWindowPos(hWnd, HWND_TOP, clientSize.left, clientSize.top, width, height, SWP_FRAMECHANGED | SWP_SHOWWINDOW);
		static_cast<irr::CIrrDeviceWin32::CCursorControl*>(mainGame->device->getCursorControl())->updateBorderSize(mainGame->is_fullscreen, true);
#endif
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
		auto _path = BufferIO::EncodeUTF8s(NormalizePath(path));
		if((dir = opendir(_path.c_str())) != nullptr) {
			struct stat fileStat;
			while((dirp = readdir(dir)) != nullptr) {
				stat((_path + dirp->d_name).c_str(), &fileStat);
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
	std::wstring Utils::NormalizePath(std::wstring path, bool trailing_slash) {
		std::replace(path.begin(), path.end(), L'\\', L'/');
		std::vector<std::wstring> paths = ygo::Game::TokenizeString<std::wstring>(path, L"/");
		if(paths.empty())
			return path;
		std::wstring normalpath;
		if(paths.front() == L".") {
			paths.erase(paths.begin());
			normalpath += L".";
		}
		for(auto it = paths.begin(); it != paths.end();) {
			if((*it).empty()) {
				it = paths.erase(it);
				continue;
			}
			if((*it) == L".") {
				it = paths.erase(it);
				continue;
			}
			if((*it) == L".." && it != paths.begin() && (*(it - 1)) != L"..") {
				it = paths.erase(paths.erase(it - 1, it));
				continue;
			}
			it++;
		}
		if(!paths.empty()) {
			if(!normalpath.empty())
				normalpath += L"/";
			for(auto it = paths.begin(); it != (paths.end() - 1); it++) {
				normalpath += *it + L"/";
			}
			normalpath += paths.back();
		}
		if(trailing_slash && normalpath.back() != L'/')
			normalpath += L"/";
		return normalpath;
	}
	std::wstring Utils::GetFileExtension(std::wstring file) {
		size_t dotpos = file.find_last_of(L".");
		if(dotpos == std::wstring::npos)
			return L"";
		std::wstring extension = file.substr(dotpos + 1);
		std::transform(extension.begin(), extension.end(), extension.begin(), ::towlower);
		return extension;
	}
	std::wstring Utils::GetFileName(std::wstring file) {
		std::replace(file.begin(), file.end(), L'\\', L'/');
		size_t dashpos = file.find_last_of(L"/");
		if(dashpos == std::wstring::npos)
			dashpos = 0;
		else
			dashpos++;
		size_t dotpos = file.find_last_of(L".");
		if(dotpos == std::wstring::npos)
			dotpos = file.size();
		std::wstring name = file.substr(dashpos, dotpos - dashpos);
		return name;
	}
	std::string Utils::NormalizePath(std::string path, bool trailing_slash) {
		std::replace(path.begin(), path.end(), '\\', '/');
		std::vector<std::string> paths = ygo::Game::TokenizeString<std::string>(path, "/");
		if(paths.empty())
			return path;
		std::string normalpath;
		for(auto it = paths.begin(); it != paths.end();) {
			if((*it).empty()) {
				it = paths.erase(it);
				continue;
			}
			if((*it) == "." && it != paths.begin()) {
				it = paths.erase(it);
				continue;
			}
			if((*it) != ".." && it != paths.begin() && (it + 1) != paths.end() && (*(it + 1)) == "..") {
				it = paths.erase(paths.erase(it));
				continue;
			}
			it++;
		}
		for(auto it = paths.begin(); it != (paths.end() - 1); it++) {
			normalpath += *it + "/";
		}
		normalpath += paths.back();
		if(trailing_slash)
			normalpath += "/";
		return normalpath;
	}
	std::string Utils::GetFileExtension(std::string file) {
		size_t dotpos = file.find_last_of(".");
		if(dotpos == std::string::npos)
			return "";
		std::string extension = file.substr(dotpos + 1);
		std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
		return extension;
	}
	std::string Utils::GetFileName(std::string file) {
		std::replace(file.begin(), file.end(), '\\', '/');
		size_t dashpos = file.find_last_of("/");
		if(dashpos == std::wstring::npos)
			dashpos = 0;
		else
			dashpos++;
		size_t dotpos = file.find_last_of(".");
		if(dotpos == std::string::npos)
			dotpos = file.size();
		std::string name = file.substr(dashpos, dotpos - dashpos);
		return name;
	}
	path_string Utils::ParseFilename(const std::wstring& input) {
#ifdef _WIN32
		return input;
#else
		return BufferIO::EncodeUTF8s(input);
#endif
	}
	path_string Utils::ParseFilename(const std::string& input) {
#ifdef _WIN32
		return BufferIO::DecodeUTF8s(input);
#else
		return input;
#endif
	}
}

