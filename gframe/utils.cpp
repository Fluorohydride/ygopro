#include "utils.h"
#include <fstream>
#ifndef _WIN32
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include "config.h"
#include "bufferio.h"
#include "logging.h"

namespace ygo {
	std::vector<Utils::IrrArchiveHelper> Utils::archives;

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
	void Utils::FindfolderFiles(const IrrArchiveHelper& archive, const path_string& path, const std::function<bool(int, path_string, bool, void*)>& cb, void* payload) {
		auto _path = ParseFilename(NormalizePath(path, false));
		auto& indexfolders = archive.folderindexes.at(_path).first;
		auto& indexfiles = archive.folderindexes.at(_path).second;
		for(int i = indexfolders.first; i < indexfolders.second && cb(i, archive.archive->getFileList()->getFileName(i).c_str(), true, payload); i++) {}
		for(int i = indexfiles.first; i < indexfiles.second && cb(i, archive.archive->getFileList()->getFileName(i).c_str(), false, payload); i++) {}

	}
	std::vector<int> Utils::FindfolderFiles(const IrrArchiveHelper& archive, const path_string& path, std::vector<path_string> extensions, int subdirectorylayers) {
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
		for(auto& archive : archives) {
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
		std::vector<std::wstring> paths = TokenizeString<std::wstring>(path, L"/");
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
		std::vector<std::string> paths = TokenizeString<std::string>(path, "/");
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
	std::wstring Utils::ToUpperNoAccents(std::wstring input) {
		std::transform(input.begin(), input.end(), input.begin(), [](wchar_t c) {
#define IN_INTERVAL(start, end) (c >= start && c <= end)
			if (IN_INTERVAL(192, 197) || IN_INTERVAL(224, 229)) {
				return L'A';
			}
			if (IN_INTERVAL(192, 197) || IN_INTERVAL(224, 229)) {
				return L'E';
			}
			if (IN_INTERVAL(200, 203) || IN_INTERVAL(232, 235)) {
				return L'I';
			}
			if (IN_INTERVAL(210, 214) || IN_INTERVAL(242, 246)) {
				return L'O';
			}
			if (IN_INTERVAL(217, 220) || IN_INTERVAL(249, 252)) {
				return L'U';
			}
			if (c == 209 || c == 241) {
				return L'N';
			}
			return (wchar_t)::towupper(c);
#undef IN_INTERVAL
		});
		return input;
	}
	bool Utils::ContainsSubstring(std::wstring input, const std::vector<std::wstring>& tokens, bool ignoreInputCasingAccents, bool ignoreTokenCasingAccents) {
		if (input.empty() || tokens.empty())
			return false;
		if (ignoreInputCasingAccents)
			input = ToUpperNoAccents(input);
		std::vector<std::wstring> alttokens;
		if (ignoreTokenCasingAccents) {
			alttokens = tokens;
			for (auto& token : alttokens)
				token = ToUpperNoAccents(token);
		}
		std::size_t pos;
		for (auto& token : ignoreTokenCasingAccents ? alttokens : tokens) {
			if ((pos = input.find(token)) == std::wstring::npos)
				return false;
			input = input.substr(pos + 1);
		}
		return true;
	}
	bool Utils::ContainsSubstring(std::wstring input, std::wstring token, bool ignoreInputCasingAccents, bool ignoreTokenCasingAccents) {
		if (input.empty() && !token.empty())
			return false;
		if (token.empty())
			return true;
		if (ignoreInputCasingAccents)
			input = ToUpperNoAccents(input);
		if (ignoreTokenCasingAccents)
			token = ToUpperNoAccents(token);
		return input.find(token) != std::wstring::npos;
	}
}

