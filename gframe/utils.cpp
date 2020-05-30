#include "utils.h"
#include <cmath> // std::round
#include <fstream>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h> // ShellExecute
#else
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
#include <IFileArchive.h>
#include <IFileSystem.h>
#include "bufferio.h"

namespace ygo {
	std::vector<Utils::locked_archive> Utils::archives;
	irr::io::IFileSystem* Utils::filesystem;

	bool Utils::MakeDirectory(const path_string& path) {
#ifdef _WIN32
		return CreateDirectory(path.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError();
#else
		return !mkdir(&path[0], 0777) || errno == EEXIST;
#endif
	}
	bool Utils::FileCopy(const path_string& source, const path_string& destination) {
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
		dst.close();
		return true;
	}
	bool Utils::FileMove(const path_string& source, const path_string& destination) {
#ifdef _WIN32
		return MoveFile(source.c_str(), destination.c_str());
#else
		return rename(source.c_str(), destination.c_str()) == 0;
#endif
		return false;
	}
	bool Utils::FileDelete(const path_string& source) {
#ifdef _WIN32
		return DeleteFile(source.c_str());
#else
		return remove(source.c_str()) == 0;
#endif
	}
	bool Utils::ClearDirectory(const path_string& path) {
#ifdef _WIN32
		WIN32_FIND_DATA fdata;
		HANDLE fh = FindFirstFile((path + EPRO_TEXT("*.*")).c_str(), &fdata);
		if(fh != INVALID_HANDLE_VALUE) {
			do {
				path_string name = fdata.cFileName;
				if(fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					if(name == EPRO_TEXT("..") || name == EPRO_TEXT(".")) {
						continue;
					}
					DeleteDirectory(path + name + EPRO_TEXT("/"));
					continue;
				} else {
					FileDelete(path + name);
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
					DeleteDirectory(path + name + "/");
					continue;
				} else {
					FileDelete(path + name);
				}
			}
			closedir(dir);
		}
		return true;
#endif
	}
	bool Utils::DeleteDirectory(const path_string& source) {
		ClearDirectory(source);
#ifdef _WIN32
		return RemoveDirectory(source.c_str());
#else
		return rmdir(source.c_str()) == 0;
#endif
	}
	void Utils::CreateResourceFolders() {
		//create directories if missing
		MakeDirectory(EPRO_TEXT("deck"));
		MakeDirectory(EPRO_TEXT("puzzles"));
		MakeDirectory(EPRO_TEXT("pics"));
		MakeDirectory(EPRO_TEXT("pics/field"));
		MakeDirectory(EPRO_TEXT("pics/cover"));
		MakeDirectory(EPRO_TEXT("pics/temp/"));
		ClearDirectory(EPRO_TEXT("pics/temp/"));
		MakeDirectory(EPRO_TEXT("replay"));
		MakeDirectory(EPRO_TEXT("screenshots"));
	}

	void Utils::FindFiles(const path_string& path, const std::function<void(path_string, bool, void*)>& cb, void* payload) {
#ifdef _WIN32
		WIN32_FIND_DATA fdataw;
		HANDLE fh = FindFirstFile((NormalizePath(path) + EPRO_TEXT("*.*")).c_str(), &fdataw);
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

	std::vector<path_string> Utils::FindFiles(const path_string& path, std::vector<path_string> extensions, int subdirectorylayers) {
		std::vector<path_string> res;
		FindFiles(path, [&res, extensions, path, subdirectorylayers](path_string name, bool isdir, void* payload) {
			if(isdir) {
				if(subdirectorylayers) {
					if(name == EPRO_TEXT("..") || name == EPRO_TEXT(".")) {
						return;
					}
					std::vector<path_string> res2 = FindFiles(path + name + EPRO_TEXT("/"), extensions, subdirectorylayers - 1);
					for(auto&file : res2) {
						file = name + EPRO_TEXT("/") + file;
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
		std::sort(res.begin(), res.end(), CompareIgnoreCase<path_string>);
		return res;
	}
	std::vector<path_string> Utils::FindSubfolders(const path_string& path, int subdirectorylayers, bool addparentpath) {
		std::vector<path_string> results;
		FindFiles(path, [&results, path, subdirectorylayers, addparentpath](path_string name, bool isdir, void* payload) {
			if (isdir) {
				if (name == EPRO_TEXT("..") || name == EPRO_TEXT(".")) {
					return;
				}
				if(addparentpath)
					results.push_back(path + name + EPRO_TEXT("/"));
				else
					results.push_back(name);
				if (subdirectorylayers > 1) {
					auto subresults = FindSubfolders(path + name + EPRO_TEXT("/"), subdirectorylayers - 1, false);
					for (auto& folder : subresults) {
						folder = name + EPRO_TEXT("/") + folder;
					}
					results.insert(results.end(), subresults.begin(), subresults.end());
				}
				return;
			}
		});
		std::sort(results.begin(), results.end(), CompareIgnoreCase<path_string>);
		return results;
	}
	std::vector<int> Utils::FindFiles(irr::io::IFileArchive* archive, const path_string& path, std::vector<path_string> extensions, int subdirectorylayers) {
		std::vector<int> res;
		auto list = archive->getFileList();
		for(int i = 0; i < list->getFileCount(); i++) {
			if(list->isDirectory(i))
				continue;
			path_string name = list->getFullFileName(i).c_str();
			if(std::count(name.begin(), name.end(), '/') > subdirectorylayers)
				continue;
			if(extensions.size() && std::find(extensions.begin(), extensions.end(), Utils::GetFileExtension(name)) == extensions.end())
				continue;
			res.push_back(i);
		}
		return res;
	}
	Utils::MutexLockedIrrArchivedFile::~MutexLockedIrrArchivedFile() {
		if (reader) {
			reader->drop();
		}
		if (mutex) {
			mutex->unlock();
		}
	}
	Utils::MutexLockedIrrArchivedFile Utils::FindFileInArchives(const path_string& path, const path_string& name) {
		for(auto& archive : archives) {
			archive.lk->lock();
			int res = -1;
			auto list = archive.archive->getFileList();
			res = list->findFile((path + name).c_str());
			if(res != -1) {
				auto reader = archive.archive->createAndOpenFile(res);
				if(reader)
					return MutexLockedIrrArchivedFile(archive.lk.get(), reader);
			}
			archive.lk->unlock();
		}
		return MutexLockedIrrArchivedFile();
	}
	path_string Utils::ToPathString(const std::wstring& input) {
#ifdef UNICODE
		return input;
#else
		return BufferIO::EncodeUTF8s(input);
#endif
	}
	path_string Utils::ToPathString(const std::string& input) {
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
	bool Utils::ContainsSubstring(const std::wstring& input, const std::vector<std::wstring>& tokens, bool convertInputCasing, bool convertTokenCasing) {
		static std::vector<std::wstring> alttokens;
		static std::wstring casedinput;
		auto string = &input;
		if (input.empty() || tokens.empty())
			return false;
		if(convertInputCasing) {
			casedinput = ToUpperNoAccents(input);
			string = &casedinput;
		}
		if (convertTokenCasing) {
			alttokens.clear();
			for (const auto& token : tokens)
				alttokens.push_back(ToUpperNoAccents(token));
		}
		std::size_t pos1, pos2 = 0;
		for (auto& token : convertTokenCasing ? alttokens : tokens) {
			if ((pos1 = string->find(token, pos2)) == std::wstring::npos)
				return false;
			pos2 = pos1 + token.size();
		}
		return true;
	}
	bool Utils::ContainsSubstring(const std::wstring& input, const std::wstring& token, bool convertInputCasing, bool convertTokenCasing) {
		if (input.empty() && !token.empty())
			return false;
		if (token.empty())
			return true;
		return (convertInputCasing ? ToUpperNoAccents(input) : input).find(convertTokenCasing ? ToUpperNoAccents(token) : token) != std::wstring::npos;
	}
	bool Utils::CreatePath(const path_string& path, const path_string& workingdir) {
		std::vector<path_string> folders;
		path_string temp;
		for(int i = 0; i < (int)path.size(); i++) {
			if(path[i] == EPRO_TEXT('/')) {
				folders.push_back(temp);
				temp.clear();
			} else
				temp += path[i];
		}
		temp.clear();
		for(auto folder : folders) {
			if(temp.empty() && !workingdir.empty())
				temp = workingdir + EPRO_TEXT("/") + folder;
			else
				temp += EPRO_TEXT("/") + folder;
			if(!MakeDirectory(temp.c_str()))
				return false;
		}
		return true;
	}

	const path_string& Utils::GetExePath() {
		static path_string binarypath = []()->path_string {
#ifdef _WIN32
			TCHAR exepath[MAX_PATH];
			GetModuleFileName(NULL, exepath, MAX_PATH);
			return Utils::NormalizePath<path_string>(exepath, false);
#elif defined(__linux__) && !defined(__ANDROID__)
			char buff[PATH_MAX];
			ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff) - 1);
			if(len != -1) {
				buff[len] = '\0';
			}
			// We could do NormalizePath but it returns the same thing anyway
			return buff;
#else
			return EPRO_TEXT(""); // Unused on macOS
#endif
		}();
		return binarypath;
	}

	const path_string& Utils::GetExeFolder() {
		static path_string binarypath = GetFilePath(GetExePath());
		return binarypath;
	}

	const path_string& Utils::GetCorePath() {
		static path_string binarypath = []()->path_string {
#ifdef _WIN32
			return GetExeFolder() + EPRO_TEXT("/ocgcore.dll");
#else
			return EPRO_TEXT(""); // Unused on POSIX
#endif
		}();
		return binarypath;
	}

	bool Utils::UnzipArchive(const path_string& input, unzip_callback callback, unzip_payload* payload, const path_string& dest) {
		thread_local char buff[0x40000];
		constexpr int buff_size = sizeof(buff) / sizeof(*buff);
		if(!filesystem)
			return false;
		CreatePath(dest, EPRO_TEXT("./"));
		irr::io::IFileArchive* archive = nullptr;
		if(!filesystem->addFileArchive(input.c_str(), false, false, irr::io::EFAT_ZIP, "", &archive))
			return false;

		archive->grab();
		auto filelist = archive->getFileList();
		auto count = filelist->getFileCount();

		int totsize = 0;
		int cur_fullsize = 0;

		for(int i = 0; i < count; i++) {
			totsize += filelist->getFileSize(i);
		}

		if(payload)
			payload->tot = count;

		for(int i = 0; i < count; i++) {
			auto filename = path_string(filelist->getFullFileName(i).c_str());
			bool isdir = filelist->isDirectory(i);
			if(isdir)
				CreatePath(filename + EPRO_TEXT("/"), dest);
			else
				CreatePath(filename, dest);
			if(!isdir) {
				int percentage = 0;
				auto reader = archive->createAndOpenFile(i);
				if(reader) {
					std::ofstream out(dest + EPRO_TEXT("/") + filename, std::ofstream::binary);
					int r, rx = reader->getSize();
					if(payload) {
						payload->is_new = true;
						payload->cur = i;
						payload->percentage = 0;
						payload->filename = filename.c_str();
						callback(payload);
						payload->is_new = false;
					}
					for(r = 0; r < rx; /**/) {
						int wx = reader->read(buff, buff_size);
						out.write(buff, wx);
						r += wx;
						cur_fullsize += wx;
						if(callback && totsize) {
							double fractiondownloaded = (double)cur_fullsize / (double)rx;
							percentage = std::round(fractiondownloaded * 100);
							if(payload)
								payload->percentage = percentage;
							callback(payload);
						}
					}
					out.close();
					reader->drop();
				}
			}
		}
		filesystem->removeFileArchive(archive);
		archive->drop();
		return true;
	}

	void Utils::SystemOpen(const path_string& url) {
#ifdef _WIN32
		ShellExecute(NULL, EPRO_TEXT("open"), url.c_str(), NULL, NULL, SW_SHOWNORMAL);
		// system("start URL") opens a shell
#else
		auto pid = fork();
		if(pid == 0) {
#ifdef __APPLE__
			execl("/usr/bin/open", "open", url.c_str(), NULL);
#else
			execl("/usr/bin/xdg-open", "xdg-open", url.c_str(), NULL);
#endif
			perror("Failed to open browser:");
		} else if(pid < 0) {
			perror("Failed to fork:");
		}
#endif
	}
}

