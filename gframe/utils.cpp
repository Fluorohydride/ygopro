#include "utils.h"
#include "game.h"
#include <fstream>
#include "bufferio.h"
#ifdef _WIN32
#include "../irrlicht/src/CIrrDeviceWin32.h"
#elif defined(__linux__)
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#elif defined(__APPLE__)
#include "osx_menu.h"
#endif
namespace ygo {
	bool Utils::Makedirectory(const path_string& path) {
#ifdef _WIN32
		return CreateDirectory(path.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError();
#else
		return !mkdir(&path[0], 0777) || errno == EEXIST;
#endif
	}
	bool Utils::Movefile(const path_string& _source, const path_string& _destination) {
		path_string source = ParseFilename(_source);
		path_string destination = ParseFilename(_destination);
		if(source == destination)
			return false;
		std::ifstream src(source, std::ios::binary);
		if(!src.is_open())
			return false;
		std::ofstream dst(destination, std::ios::binary);
		if(!dst.is_open())
			return false;
		dst << src.rdbuf();
		src.close();
		Deletefile(source);
		return true;
	}
	bool Utils::Deletefile(const path_string& source) {
#ifdef _WIN32
		return DeleteFile(source.c_str());
#else
		return remove(source.c_str()) == 0;
#endif
	}
	bool Utils::ClearDirectory(const path_string& path) {
#ifdef _WIN32
		WIN32_FIND_DATA fdata;
		HANDLE fh = FindFirstFile((path + TEXT("*.*")).c_str(), &fdata);
		if(fh != INVALID_HANDLE_VALUE) {
			do {
				path_string name = fdata.cFileName;
				if(fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					if(name == TEXT("..") || name == TEXT(".")) {
						continue;
					}
					Deletedirectory(path + name + TEXT("/"));
					continue;
				} else {
					Deletefile(path + name);
				}
			} while(FindNextFile(fh, &fdata));
			FindClose(fh);
		}
		return true;
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
	bool Utils::Deletedirectory(const path_string& source) {
		ClearDirectory(source);
#ifdef _WIN32
		return RemoveDirectory(source.c_str());
#else
		return rmdir(source.c_str()) == 0;
#endif
	}
	void Utils::CreateResourceFolders() {
		//create directories if missing
		Makedirectory(TEXT("deck"));
		Makedirectory(TEXT("puzzles"));
		Makedirectory(TEXT("pics"));
		Makedirectory(TEXT("pics/field"));
		Makedirectory(TEXT("pics/cover"));
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
#elif defined(__linux__)
		struct {
			unsigned long   flags;
			unsigned long   functions;
			unsigned long   decorations;
			long            inputMode;
			unsigned long   status;
		} hints = {};
		Display* display = XOpenDisplay(NULL);;
		Window window;
		static bool wasHorizontalMaximized = false, wasVerticalMaximized = false;
		Window child;
		int revert;
		mainGame->is_fullscreen = !mainGame->is_fullscreen;
		XGetInputFocus(display, &window, &revert);

		Atom wm_state = XInternAtom(display, "_NET_WM_STATE", false);
		Atom max_horz = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_HORZ", false);
		Atom max_vert = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_VERT", false);

		auto checkMaximized = [&]() {
			long maxLength = 1024;
			Atom actualType;
			int actualFormat;
			unsigned long i, numItems, bytesAfter;
			unsigned char *propertyValue = NULL;
			if(XGetWindowProperty(display, window, wm_state,
								  0l, maxLength, false, XA_ATOM, &actualType,
								  &actualFormat, &numItems, &bytesAfter,
								  &propertyValue) == Success) {
				Atom* atoms = (Atom *)propertyValue;
				for(i = 0; i < numItems; ++i) {
					if(atoms[i] == max_vert) {
						wasVerticalMaximized = true;
					} else if(atoms[i] == max_horz) {
						wasHorizontalMaximized = true;
					}
				}
				XFree(propertyValue);
			}
		};
		if(mainGame->is_fullscreen)
			checkMaximized();
		if(!wasHorizontalMaximized && !wasVerticalMaximized) {
			XEvent xev = {};
			xev.type = ClientMessage;
			xev.xclient.window = window;
			xev.xclient.message_type = wm_state;
			xev.xclient.format = 32;
			xev.xclient.data.l[0] = mainGame->is_fullscreen ? 1 : 0;
			int i = 1;
			if(!wasHorizontalMaximized)
				xev.xclient.data.l[i++] = max_horz;
			if(!wasVerticalMaximized)
				xev.xclient.data.l[i++] = max_vert;
			if(i == 2)
				xev.xclient.data.l[i] = 0;
			XSendEvent(display, DefaultRootWindow(display), False, SubstructureNotifyMask, &xev);
		}

		Atom property = XInternAtom(display, "_MOTIF_WM_HINTS", true);
		hints.flags = 2;
		hints.decorations = mainGame->is_fullscreen ? 0 : 1;
		XChangeProperty(display, window, property, property, 32, PropModeReplace, (unsigned char*)&hints, 5);
		XMapWindow(display, window);
		XFlush(display);
#elif defined(__APPLE__)
		EDOPRO_ToggleFullScreen();
#endif
	}

	void Utils::changeCursor(irr::gui::ECURSOR_ICON icon) {
		irr::gui::ICursorControl* cursor = mainGame->device->getCursorControl();
		if (cursor->getActiveIcon() != icon) {
			cursor->setActiveIcon(icon);
		}
	}
	void Utils::FindfolderFiles(const path_string& path, const std::function<void(path_string, bool, void*)>& cb, void* payload) {
#ifdef _WIN32
		WIN32_FIND_DATA fdataw;
		HANDLE fh = FindFirstFile((NormalizePath(path) + TEXT("*.*")).c_str(), &fdataw);
		if(fh != INVALID_HANDLE_VALUE) {
			do {
				cb(fdataw.cFileName, !!(fdataw.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY), payload);
			} while(FindNextFile(fh, &fdataw));
			FindClose(fh);
		}
#else
		DIR * dir;
		struct dirent * dirp = nullptr;
		auto _path = NormalizePath(path);
		if((dir = opendir(_path.c_str())) != nullptr) {
			struct stat fileStat;
			while((dirp = readdir(dir)) != nullptr) {
				stat((_path + dirp->d_name).c_str(), &fileStat);
				cb(dirp->d_name, !!S_ISDIR(fileStat.st_mode), payload);
			}
			closedir(dir);
		}
#endif
	}
	std::vector<path_string> Utils::FindfolderFiles(const path_string& path, std::vector<path_string> extensions, int subdirectorylayers) {
		std::vector<path_string> res;
		FindfolderFiles(path, [&res, extensions, path, subdirectorylayers](path_string name, bool isdir, void* payload) {
			if(isdir) {
				if(subdirectorylayers) {
					if(name == TEXT("..") || name == TEXT(".")) {
						return;
					}
					std::vector<path_string> res2 = FindfolderFiles(path + name + TEXT("/"), extensions, subdirectorylayers - 1);
					for(auto&file : res2) {
						file = name + TEXT("/") + file;
					}
					res.insert(res.end(), res2.begin(), res2.end());
				}
				return;
			} else {
				if(extensions.size() && std::find(extensions.begin(), extensions.end(), Utils::GetFileExtension(name)) == extensions.end())
					return;
				res.push_back(name);
			}
		});
		std::sort(res.begin(), res.end());
		return res;
	}
	std::vector<path_string> Utils::FindSubfolders(const path_string& path, int subdirectorylayers) {
		std::vector<path_string> results;
		FindfolderFiles(path, [&results, path, subdirectorylayers](path_string name, bool isdir, void* payload) {
			if (isdir) {
				if (name == TEXT("..") || name == TEXT(".")) {
					return;
				}
				results.push_back(path + name + TEXT("/"));
				if (subdirectorylayers > 1) {
					auto subresults = FindSubfolders(path + name + TEXT("/"), subdirectorylayers - 1);
					for (auto& folder : subresults) {
						folder = name + TEXT("/") + folder;
					}
					results.insert(results.end(), subresults.begin(), subresults.end());
				}
				return;
			}
		});
		std::sort(results.begin(), results.end());
		return results;
	}
	void Utils::FindfolderFiles(IrrArchiveHelper& archive, const path_string& path, const std::function<bool(int, path_string, bool, void*)>& cb, void* payload) {
		auto _path = ParseFilename(NormalizePath(path, false));
		auto& indexfolders = archive.folderindexes[_path].first;
		auto& indexfiles = archive.folderindexes[_path].second;
		for(int i = indexfolders.first; i < indexfolders.second && cb(i, archive.archive->getFileList()->getFileName(i).c_str(), true, payload); i++) {}
		for(int i = indexfiles.first; i < indexfiles.second && cb(i, archive.archive->getFileList()->getFileName(i).c_str(), false, payload); i++) {}

	}
	std::vector<int> Utils::FindfolderFiles(IrrArchiveHelper& archive, const path_string& path, std::vector<path_string> extensions, int subdirectorylayers) {
		std::vector<int> res;
		FindfolderFiles(archive, path, [&res, arc = archive.archive, extensions, path, subdirectorylayers, &archive](int index, path_string name, bool isdir, void* payload)->bool {
			if(isdir) {
				if(subdirectorylayers) {
					if(name == TEXT("..") || name == TEXT(".")) {
						return true;
					}
					std::vector<int> res2 = FindfolderFiles(archive, path + name + TEXT("/"), extensions, subdirectorylayers - 1);
					res.insert(res.end(), res2.begin(), res2.end());
				}
				return true;
			} else {
				if(extensions.size() && std::find(extensions.begin(), extensions.end(), Utils::GetFileExtension(name)) == extensions.end())
					return true;
				res.push_back(index);
			}
			return true;
		});

		return res;
	}
	irr::io::IReadFile* Utils::FindandOpenFileFromArchives(const path_string & path, const path_string & name) {
		for(auto& archive : mainGame->archives) {
			int res = -1;
			Utils::FindfolderFiles(archive, path, [match = &name, &res](int index, path_string name, bool isdir, void* payload)->bool {
				if(isdir)
					return false;
				if(name == (*match)) {
					res = index;
					return false;
				}
				return true;
			});
			if(res != -1) {
				auto reader = archive.archive->createAndOpenFile(res);
				if(reader)
					return reader;
			}
		}
		return nullptr;
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
	std::wstring Utils::GetFilePath(std::wstring file) {
		std::replace(file.begin(), file.end(), L'\\', L'/');
		size_t slashpos = file.find_last_of(L'/');
		if(slashpos == std::wstring::npos)
			return file;
		std::wstring extension = file.substr(0, slashpos);
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
	std::string Utils::GetFilePath(std::string file) {
		std::replace(file.begin(), file.end(), '\\', '/');
		size_t slashpos = file.find_last_of(".");
		if(slashpos == std::string::npos)
			return file;
		std::string extension = file.substr(0, slashpos);
		std::transform(extension.begin(), extension.end(), extension.begin(), ::towlower);
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
#ifdef UNICODE
		return input;
#else
		return BufferIO::EncodeUTF8s(input);
#endif
	}
	path_string Utils::ParseFilename(const std::string& input) {
#ifdef UNICODE
		return BufferIO::DecodeUTF8s(input);
#else
		return input;
#endif
	}
	std::string Utils::ToUTF8IfNeeded(const path_string& input) {
#ifdef UNICODE
		return BufferIO::EncodeUTF8s(input);
#else
		return input;
#endif
	}
	std::wstring Utils::ToUnicodeIfNeeded(const path_string & input) {
#ifdef UNICODE
		return input;
#else
		return BufferIO::DecodeUTF8s(input);
#endif
	}
	void Utils::IrrArchiveHelper::ParseList(irr::io::IFileArchive* _archive) {
		archive = _archive;
		auto list = archive->getFileList();
		std::vector<path_string> list_full;
		folderindexes[TEXT(".")] = { { -1, -1 }, { -1, -1 } };
		for(u32 i = 0; i < list->getFileCount(); ++i) {
			list_full.push_back(list->getFullFileName(i).c_str());
			if(list->isDirectory(i)) {
				folderindexes[list->getFullFileName(i).c_str()] = { { -1, -1 }, { -1, -1 } };
				auto& name_path = list->getFullFileName(i);
				auto& name = list->getFileName(i);
				if(name_path.size() == name.size()) {
					/*special case, root folder*/
					folderindexes[TEXT("")] = { { std::min((unsigned)folderindexes[TEXT("")].first.first, i), i + 1 }, folderindexes[TEXT("")].second };
				} else {
					path_string path = NormalizePath(name_path.subString(0, name_path.size() - name.size() - 1).c_str(), false);
					folderindexes[path] = { { std::min((unsigned)folderindexes[path].first.first, i), i + 1 }, folderindexes[path].second };
				}
			} else {
				auto& name_path = list->getFullFileName(i);
				auto& name = list->getFileName(i);
				if(name_path.size() == name.size()) {
					/*special case, root folder*/
					folderindexes[TEXT("")] = { folderindexes[TEXT("")].first, { std::min((unsigned)folderindexes[TEXT("")].second.first, i), i + 1 } };
				} else {
					path_string path = NormalizePath(name_path.subString(0, name_path.size() - name.size() - 1).c_str(), false);
					folderindexes[path] = { folderindexes[path].first, { std::min((unsigned)folderindexes[path].second.first, i), i + 1 } };
				}
			}
		}
	}
}

