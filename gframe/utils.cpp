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
using Stat = struct stat;
using Dirent = struct dirent;
#endif
#ifdef __ANDROID__
#include "Android/porting_android.h"
#endif
#include <IFileArchive.h>
#include <IFileSystem.h>
#include <fmt/format.h>
#include "bufferio.h"

namespace ygo {
	std::vector<Utils::SynchronizedIrrArchive> Utils::archives;
	irr::io::IFileSystem* Utils::filesystem;

	bool Utils::MakeDirectory(path_stringview path) {
#ifdef _WIN32
		return CreateDirectory(path.data(), NULL) || ERROR_ALREADY_EXISTS == GetLastError();
#else
		return !mkdir(&path[0], 0777) || errno == EEXIST;
#endif
	}
	bool Utils::FileCopy(path_stringview source, path_stringview destination) {
		if(source == destination)
			return false;
#ifdef _WIN32
		return CopyFile(source.data(), destination.data(), false);
#else
		std::ifstream src(source.data(), std::ios::binary);
		if(!src.is_open())
			return false;
		std::ofstream dst(destination.data(), std::ios::binary);
		if(!dst.is_open())
			return false;
		dst << src.rdbuf();
		src.close();
		dst.close();
		return true;
#endif
	}
	bool Utils::FileMove(path_stringview source, path_stringview destination) {
#ifdef _WIN32
		return MoveFile(source.data(), destination.data());
#else
		return rename(source.data(), destination.data()) == 0;
#endif
		return false;
	}
	bool Utils::FileExists(path_stringview path) {
#ifdef _WIN32
		auto dwAttrib = GetFileAttributes(path.data());
		return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#else
		Stat sb;
		if(stat(path.data(), &sb) == -1)
			return false;
		return S_ISREG(sb.st_mode) != 0;
#endif
	}
	bool Utils::FileDelete(path_stringview source) {
#ifdef _WIN32
		return DeleteFile(source.data());
#else
		return remove(source.data()) == 0;
#endif
	}
	bool Utils::ClearDirectory(path_stringview path) {
#ifdef _WIN32
		WIN32_FIND_DATA fdata;
		HANDLE fh = FindFirstFile(fmt::format(EPRO_TEXT("{}*.*"), path).c_str(), &fdata);
		if(fh != INVALID_HANDLE_VALUE) {
			do {
				path_stringview name = fdata.cFileName;
				if(fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY
				   && name != EPRO_TEXT("..") && name != EPRO_TEXT("."))
					DeleteDirectory(fmt::format(EPRO_TEXT("{}{}/"), path, name));
				else
					FileDelete(fmt::format(EPRO_TEXT("{}{}"),path, name));
			} while(FindNextFile(fh, &fdata));
			FindClose(fh);
		}
		return true;
#else
		DIR* dir;
		Dirent* dirp = nullptr;
		if((dir = opendir(path.data())) != nullptr) {
			Stat fileStat;
			while((dirp = readdir(dir)) != nullptr) {
				path_stringview name = dirp->d_name;
				stat(fmt::format("{}{}", path, name).c_str(), &fileStat);
				if(S_ISDIR(fileStat.st_mode) && name != ".." && name != ".")
					DeleteDirectory(fmt::format("{}{}/", path, name));
				else
					FileDelete(fmt::format("{}{}", path, name));
			}
			closedir(dir);
		}
		return true;
#endif
	}
	bool Utils::DeleteDirectory(path_stringview source) {
		ClearDirectory(source);
#ifdef _WIN32
		return RemoveDirectory(source.data());
#else
		return rmdir(source.data()) == 0;
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

	void Utils::FindFiles(path_stringview path, const std::function<void(path_stringview, bool, void*)>& cb, void* payload) {
#ifdef _WIN32
		WIN32_FIND_DATA fdataw;
		HANDLE fh = FindFirstFile(fmt::format(EPRO_TEXT("{}*.*"), NormalizePath<path_string>(path.data())).data(), &fdataw);
		if(fh != INVALID_HANDLE_VALUE) {
			do {
				cb(fdataw.cFileName, !!(fdataw.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY), payload);
			} while(FindNextFile(fh, &fdataw));
			FindClose(fh);
		}
#else
		DIR* dir;
		Dirent* dirp = nullptr;
		auto _path = NormalizePath<path_string>(path.data());
		if((dir = opendir(_path.data())) != nullptr) {
			Stat fileStat;
			while((dirp = readdir(dir)) != nullptr) {
				stat(fmt::format("{}{}",_path, dirp->d_name).data(), &fileStat);
				cb(dirp->d_name, !!S_ISDIR(fileStat.st_mode), payload);
			}
			closedir(dir);
		}
#endif
	}

	std::vector<path_string> Utils::FindFiles(path_stringview path, const std::vector<path_stringview>& extensions, int subdirectorylayers) {
		std::vector<path_string> res;
		FindFiles(path, [&res, extensions, path, subdirectorylayers](path_stringview name, bool isdir, void* payload) {
			if(isdir) {
				if(subdirectorylayers) {
					if(name == EPRO_TEXT("..") || name == EPRO_TEXT(".")) {
						return;
					}
					std::vector<path_string> res2 = FindFiles(fmt::format(EPRO_TEXT("{}{}/"), path, name), extensions, subdirectorylayers - 1);
					for(auto& file : res2) {
						file = fmt::format(EPRO_TEXT("{}/{}"), name, file);
					}
					res.insert(res.end(), std::make_move_iterator(res2.begin()), std::make_move_iterator(res2.end()));
				}
				return;
			} else {
				if(extensions.size() && std::find(extensions.begin(), extensions.end(), Utils::GetFileExtension<path_string>(name.data())) == extensions.end())
					return;
				res.push_back(name.data());
			}
		});
		std::sort(res.begin(), res.end(), CompareIgnoreCase<path_string>);
		return res;
	}
	std::vector<path_string> Utils::FindSubfolders(path_stringview path, int subdirectorylayers, bool addparentpath) {
		std::vector<path_string> results;
		FindFiles(path, [&results, path, subdirectorylayers, addparentpath](path_stringview name, bool isdir, void* payload) {
			if (isdir) {
				if (name == EPRO_TEXT("..") || name == EPRO_TEXT(".")) {
					return;
				}
				if(addparentpath)
					results.push_back(fmt::format(EPRO_TEXT("{}{}/"), path, name));
				else
					results.push_back(name.data());
				if (subdirectorylayers > 1) {
					auto subresults = FindSubfolders(fmt::format(EPRO_TEXT("{}{}/"), path, name), subdirectorylayers - 1, false);
					for (auto& folder : subresults) {
						folder = fmt::format(EPRO_TEXT("{}/{}"), name, folder);
					}
					results.insert(results.end(), std::make_move_iterator(subresults.begin()), std::make_move_iterator(subresults.end()));
				}
				return;
			}
		});
		std::sort(results.begin(), results.end(), CompareIgnoreCase<path_string>);
		return results;
	}
	std::vector<int> Utils::FindFiles(irr::io::IFileArchive* archive, path_stringview path, const std::vector<path_stringview>& extensions, int subdirectorylayers) {
		std::vector<int> res;
		auto list = archive->getFileList();
		for(irr::u32 i = 0; i < list->getFileCount(); i++) {
			if(list->isDirectory(i))
				continue;
			path_stringview name = list->getFullFileName(i).c_str();
			if(std::count(name.begin(), name.end(), EPRO_TEXT('/')) > subdirectorylayers)
				continue;
			if(extensions.size() && std::find(extensions.begin(), extensions.end(), Utils::GetFileExtension<path_string>(name.data())) == extensions.end())
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
	Utils::MutexLockedIrrArchivedFile Utils::FindFileInArchives(path_stringview path, path_stringview name) {
		for(auto& archive : archives) {
			archive.mutex->lock();
			int res = -1;
			auto list = archive.archive->getFileList();
			res = list->findFile(fmt::format(EPRO_TEXT("{}{}"), path, name).c_str());
			if(res != -1) {
				auto reader = archive.archive->createAndOpenFile(res);
				if(reader)
					return MutexLockedIrrArchivedFile(archive.mutex.get(), reader); // drops reader and unlocks when done
			}
			archive.mutex->unlock();
		}
		return MutexLockedIrrArchivedFile(); // file not found
	}
	bool Utils::ContainsSubstring(epro_wstringview input, const std::vector<std::wstring>& tokens, bool convertInputCasing, bool convertTokenCasing) {
		static std::vector<std::wstring> alttokens;
		static std::wstring casedinput;
		if (input.empty() || tokens.empty())
			return false;
		if(convertInputCasing) {
			casedinput = ToUpperNoAccents<std::wstring>(input.data());
			input = casedinput;
		}
		if (convertTokenCasing) {
			alttokens.clear();
			for (const auto& token : tokens)
				alttokens.push_back(ToUpperNoAccents(token));
		}
		std::size_t pos1, pos2 = 0;
		for (auto& token : convertTokenCasing ? alttokens : tokens) {
			if ((pos1 = input.find(token, pos2)) == epro_wstringview::npos)
				return false;
			pos2 = pos1 + token.size();
		}
		return true;
	}
	bool Utils::ContainsSubstring(epro_wstringview input, epro_wstringview token, bool convertInputCasing, bool convertTokenCasing) {
		if (input.empty() && !token.empty())
			return false;
		if (token.empty())
			return true;
		if(convertInputCasing) {
			return ToUpperNoAccents<std::wstring>(input.data()).find(convertTokenCasing ? ToUpperNoAccents<std::wstring>(token.data()).data() : token.data()) != std::wstring::npos;
		} else {
			return input.find(convertTokenCasing ? ToUpperNoAccents<std::wstring>(token.data()).data() : token.data()) != epro_wstringview::npos;
		}
		//return (convertInputCasing ? ToUpperNoAccents<std::wstring>(input.data()) : input).find(convertTokenCasing ? ToUpperNoAccents<std::wstring>(token.data()) : token) != std::wstring::npos;
	}
	bool Utils::CreatePath(path_stringview path, path_stringview workingdir) {
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
			if(temp.empty() && workingdir.size())
				temp = fmt::format(EPRO_TEXT("{}/{}"),workingdir, folder);
			else
				temp += EPRO_TEXT("/") + folder;
			if(!MakeDirectory(temp.data()))
				return false;
		}
		return true;
	}

	path_stringview Utils::GetExePath() {
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

	path_stringview Utils::GetExeFolder() {
		static path_string binarypath = GetFilePath<path_string>(GetExePath().data());
		return binarypath;
	}

	path_stringview Utils::GetCorePath() {
		static path_string binarypath = []()->path_string {
#ifdef _WIN32
			return fmt::format(EPRO_TEXT("{}/ocgcore.dll"), GetExeFolder());
#else
			return EPRO_TEXT(""); // Unused on POSIX
#endif
		}();
		return binarypath;
	}

	bool Utils::UnzipArchive(path_stringview input, unzip_callback callback, unzip_payload* payload, path_stringview dest) {
		thread_local char buff[0x40000];
		constexpr int buff_size = sizeof(buff) / sizeof(*buff);
		if(!filesystem)
			return false;
		CreatePath(dest, EPRO_TEXT("./"));
		irr::io::IFileArchive* archive = nullptr;
		if(!filesystem->addFileArchive(input.data(), false, false, irr::io::EFAT_ZIP, "", &archive))
			return false;

		archive->grab();
		auto filelist = archive->getFileList();
		auto count = filelist->getFileCount();

		irr::u32 totsize = 0;
		irr::u32 cur_fullsize = 0;

		for(irr::u32 i = 0; i < count; i++)
			totsize += filelist->getFileSize(i);

		if(payload)
			payload->tot = count;

		for(irr::u32 i = 0; i < count; i++) {
			path_stringview filename = filelist->getFullFileName(i).c_str();
			bool isdir = filelist->isDirectory(i);
			if(isdir)
				CreatePath(fmt::format(EPRO_TEXT("{}/"), filename), dest);
			else
				CreatePath(filename, dest);
			if(!isdir) {
				int percentage = 0;
				auto reader = archive->createAndOpenFile(i);
				if(reader) {
					std::ofstream out(fmt::format(EPRO_TEXT("{}/{}") , dest, filename), std::ofstream::binary);
					int r, rx = reader->getSize();
					if(payload) {
						payload->is_new = true;
						payload->cur = i;
						payload->percentage = 0;
						payload->filename = filename.data();
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
							percentage = (int)std::round(fractiondownloaded * 100);
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

	void Utils::SystemOpen(path_stringview url, OpenType type) {
#ifdef _WIN32
		ShellExecute(NULL, EPRO_TEXT("open"), (type == OPEN_FILE) ? filesystem->getAbsolutePath(url.data()).c_str() : url.data(), NULL, NULL, SW_SHOWNORMAL);
		// system("start URL") opens a shell
#elif !defined(__ANDROID__)
		auto pid = fork();
		if(pid == 0) {
#ifdef __APPLE__
			execl("/usr/bin/open", "open", url.data(), NULL);
#else
			execl("/usr/bin/xdg-open", "xdg-open", url.data(), NULL);
#endif
			perror("Failed to open browser:");
		} else if(pid < 0) {
			perror("Failed to fork:");
		}
#else
		if(type == OPEN_FILE)
			porting::openFile(porting::working_directory + url);
		else
			porting::openUrl(url);
#endif
	}
}

