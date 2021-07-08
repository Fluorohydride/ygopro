#include "utils.h"
#include <cmath> // std::round
#include <fstream>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h> // ShellExecute
#else
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
using Stat = struct stat;
#ifdef __ANDROID__
#include "Android/porting_android.h"
#else
#include <sys/wait.h>
#endif //__ANDROID__
#if defined(__linux__)
#include <sys/sendfile.h>
#include <fcntl.h>
#elif defined(__APPLE__)
#import <CoreFoundation/CoreFoundation.h>
#include <mach-o/dyld.h>
#include <CoreServices/CoreServices.h>
#include <copyfile.h>
#endif //__linux__
#endif //_WIN32
#include <IFileArchive.h>
#include <IFileSystem.h>
#include <fmt/format.h>
#include <IOSOperator.h>
#include "config.h"
#include "bufferio.h"
#if defined(__MINGW32__) && defined(UNICODE)
#include <fcntl.h>
#include <ext/stdio_filebuf.h>
#endif

#if defined(_WIN32) && defined(_MSC_VER)
namespace WindowsWeirdStuff {

//https://docs.microsoft.com/en-us/visualstudio/debugger/how-to-set-a-thread-name-in-native-code?view=vs-2015&redirectedfrom=MSDN

static constexpr DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma warning(push)
#pragma warning(disable: 6320 6322)
#pragma pack(push, 8)
struct THREADNAME_INFO {
	DWORD dwType; // Must be 0x1000.
	LPCSTR szName; // Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags; // Reserved for future use, must be zero.
};
#pragma pack(pop)
static inline void NameThread(const char* threadName) {
	const THREADNAME_INFO info{ 0x1000, threadName, ((DWORD)-1), 0 };
	__try {	RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info); }
	__except(EXCEPTION_EXECUTE_HANDLER) {}
}
}
#pragma warning(pop)
#endif

namespace ygo {
	std::vector<SynchronizedIrrArchive> Utils::archives;
	irr::io::IFileSystem* Utils::filesystem{ nullptr };
	irr::IOSOperator* Utils::OSOperator{ nullptr };
	epro::path_string Utils::working_dir;

	void Utils::InternalSetThreadName(const char* name, const wchar_t* wname) {
#if defined(_WIN32)
		static const auto PSetThreadDescription = (HRESULT(WINAPI *)(HANDLE, PCWSTR))GetProcAddress(GetModuleHandle(EPRO_TEXT("kernel32.dll")), "SetThreadDescription");
		if(PSetThreadDescription)
			PSetThreadDescription(GetCurrentThread(), wname);
#if defined(_MSC_VER)
		WindowsWeirdStuff::NameThread(name);
#endif //_MSC_VER
#else
		(void)wname;
#if defined(__linux__)
		pthread_setname_np(pthread_self(), name);
#elif defined(__APPLE__)
		pthread_setname_np(name);
#endif //__linux__
#endif //_WIN32
	}

	bool Utils::MakeDirectory(epro::path_stringview path) {
#ifdef _WIN32
		return CreateDirectory(path.data(), nullptr) || ERROR_ALREADY_EXISTS == GetLastError();
#else
		return mkdir(path.data(), 0777) == 0 || errno == EEXIST;
#endif
	}
	bool Utils::FileCopy(epro::path_stringview source, epro::path_stringview destination) {
		if(source == destination)
			return false;
#ifdef _WIN32
		return CopyFile(source.data(), destination.data(), false);
#elif defined(__linux__)
		int input, output;
		if((input = open(source.data(), O_RDONLY)) == -1) {
			return false;
		}
		if((output = creat(destination.data(), 0660)) == -1) {
			close(input);
			return false;
		}
		off_t bytesCopied = 0;
		struct stat fileinfo = { 0 };
		fstat(input, &fileinfo);
		int result = sendfile(output, input, &bytesCopied, fileinfo.st_size);
		close(input);
		close(output);
		return result != -1;
#elif defined(__APPLE__)
		return copyfile(source.data(), destination.data(), 0, COPYFILE_ALL) == 0;
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
	bool Utils::FileMove(epro::path_stringview source, epro::path_stringview destination) {
#ifdef _WIN32
		return MoveFile(source.data(), destination.data());
#else
		return rename(source.data(), destination.data()) == 0;
#endif
	}
	bool Utils::FileExists(epro::path_stringview path) {
#ifdef _WIN32
		const auto dwAttrib = GetFileAttributes(path.data());
		return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#else
		Stat sb;
		return stat(path.data(), &sb) != -1 && S_ISREG(sb.st_mode) != 0;
#endif
	}
	bool Utils::ChangeDirectory(epro::path_stringview newpath) {
#ifdef _WIN32
		return SetCurrentDirectory(newpath.data());
#else
		return chdir(newpath.data()) == 0;
#endif
	}
	bool Utils::FileDelete(epro::path_stringview source) {
#ifdef _WIN32
		return DeleteFile(source.data());
#else
		return remove(source.data()) == 0;
#endif
	}

	void Utils::FindFiles(epro::path_stringview _path, const std::function<void(epro::path_stringview, bool)>& cb) {
		const auto path = Utils::NormalizePath(_path);
#ifdef _WIN32
		WIN32_FIND_DATA fdata;
		auto fh = FindFirstFile(fmt::format(EPRO_TEXT("{}*.*"), path).data(), &fdata);
		if(fh != INVALID_HANDLE_VALUE) {
			do {
				cb(fdata.cFileName, !!(fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY));
			} while(FindNextFile(fh, &fdata));
			FindClose(fh);
		}
#else
		if(auto dir = opendir(path.data())) {
			while(auto dirp = readdir(dir)) {
				bool isdir = false;
#ifdef _DIRENT_HAVE_D_TYPE //avoid call to format and stat
				if(dirp->d_type != DT_LNK && dirp->d_type != DT_UNKNOWN) {
					isdir = dirp->d_type == DT_DIR;
					if(!isdir && dirp->d_type != DT_REG) //not a folder or file, skip
						continue;
				} else
#endif
				{
					Stat fileStat;
					stat(fmt::format("{}{}", path, dirp->d_name).data(), &fileStat);
					isdir = !!S_ISDIR(fileStat.st_mode);
					if(!isdir && !S_ISREG(fileStat.st_mode)) //not a folder or file, skip
						continue;
				}
				cb(dirp->d_name, isdir);
			}
			closedir(dir);
		}
#endif
	}

#define ROOT_OR_CUR(str) (str == EPRO_TEXT(".") || (str == EPRO_TEXT("..")))
	bool Utils::ClearDirectory(epro::path_stringview path) {
		FindFiles(path, [&path](epro::path_stringview name, bool isdir) {
			if(isdir) {
				if(!ROOT_OR_CUR(name))
					DeleteDirectory(fmt::format(EPRO_TEXT("{}{}/"), path, name));
			} else
				FileDelete(fmt::format(EPRO_TEXT("{}{}"), path, name));
		});
		return true;
	}
	bool Utils::DeleteDirectory(epro::path_stringview source) {
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

	std::vector<epro::path_string> Utils::FindFiles(epro::path_stringview path, const std::vector<epro::path_stringview>& extensions, int subdirectorylayers) {
		std::vector<epro::path_string> res;
		FindFiles(path, [&res, extensions, path, subdirectorylayers](epro::path_stringview name, bool isdir) {
			if(isdir) {
				if(subdirectorylayers && !ROOT_OR_CUR(name)) {
					auto res2 = FindFiles(fmt::format(EPRO_TEXT("{}{}/"), path, name), extensions, subdirectorylayers - 1);
					for(auto& file : res2)
						file = fmt::format(EPRO_TEXT("{}/{}"), name, file);
					res.insert(res.end(), std::make_move_iterator(res2.begin()), std::make_move_iterator(res2.end()));
				}
			} else {
				if(extensions.empty() || std::find(extensions.begin(), extensions.end(), Utils::GetFileExtension(name)) != extensions.end())
					res.emplace_back(name.data(), name.size());
			}
		});
		std::sort(res.begin(), res.end(), CompareIgnoreCase<epro::path_string>);
		return res;
	}
	std::vector<epro::path_string> Utils::FindSubfolders(epro::path_stringview path, int subdirectorylayers, bool addparentpath) {
		std::vector<epro::path_string> results;
		FindFiles(path, [&results, path, subdirectorylayers, addparentpath](epro::path_stringview name, bool isdir) {
			if (!isdir || ROOT_OR_CUR(name))
				return;
			epro::path_string fullpath = fmt::format(EPRO_TEXT("{}{}/"), path, name);
			epro::path_stringview cur = name;
			if(addparentpath)
				cur = fullpath;
			results.emplace_back(cur.data(), cur.size());
			if(subdirectorylayers > 1) {
				auto subresults = FindSubfolders(fullpath, subdirectorylayers - 1, false);
				for(auto& folder : subresults) {
					folder = fmt::format(EPRO_TEXT("{}{}/"), fullpath, folder);
				}
				results.insert(results.end(), std::make_move_iterator(subresults.begin()), std::make_move_iterator(subresults.end()));
			}
		});
		std::sort(results.begin(), results.end(), CompareIgnoreCase<epro::path_string>);
		return results;
	}
	std::vector<int> Utils::FindFiles(irr::io::IFileArchive* archive, epro::path_stringview path, const std::vector<epro::path_stringview>& extensions, int subdirectorylayers) {
		std::vector<int> res;
		auto list = archive->getFileList();
		for(irr::u32 i = 0; i < list->getFileCount(); i++) {
			if(list->isDirectory(i))
				continue;
			const auto name = list->getFullFileName(i);
			if(std::count(name.data(), name.data() + name.size(), EPRO_TEXT('/')) > subdirectorylayers)
				continue;
			if(extensions.empty() || std::find(extensions.begin(), extensions.end(), Utils::GetFileExtension(name)) != extensions.end())
				res.push_back(i);
		}
		return res;
	}
	irr::io::IReadFile* Utils::FindFileInArchives(epro::path_stringview path, epro::path_stringview name) {
		for(auto& archive : archives) {
			auto list = archive.archive->getFileList();
			int res = list->findFile(fmt::format(EPRO_TEXT("{}{}"), path, name).data());
			if(res != -1) {
				std::lock_guard<std::mutex> lk(*archive.mutex);
				auto reader = archive.archive->createAndOpenFile(res);
				return reader;
			}
		}
		return nullptr;
	}
	const std::string& Utils::GetUserAgent() {
		static const std::string agent = fmt::format("EDOPro-" OSSTRING "-" STR(EDOPRO_VERSION_MAJOR) "." STR(EDOPRO_VERSION_MINOR) "." STR(EDOPRO_VERSION_PATCH)" {}",
											   ygo::Utils::OSOperator->getOperatingSystemVersion());
		return agent;
	}
	epro::path_string Utils::GetAbsolutePath(epro::path_stringview path) {
#ifdef _WIN32
		epro::path_char fpath[MAX_PATH];
		auto len = GetFullPathName(path.data(), MAX_PATH, fpath, nullptr);
		epro::path_string ret{ fpath, len };
		std::replace(ret.begin(), ret.end(), EPRO_TEXT('\\'), EPRO_TEXT('/'));
		return ret;
#else
		epro::path_char* p = realpath(path.data(), nullptr);
		epro::path_string ret{ p };
		free(p);
		return ret;
#endif
	}
	bool Utils::ContainsSubstring(epro::wstringview input, const std::vector<std::wstring>& tokens) {
		if (input.empty() || tokens.empty())
			return false;
		std::size_t pos1, pos2 = 0;
		for (const auto& token : tokens) {
			if((pos1 = input.find(token, pos2)) == epro::wstringview::npos)
				return false;
			pos2 = pos1 + token.size();
		}
		return true;
	}
	bool Utils::CreatePath(epro::path_stringview path, epro::path_string workingdir) {
		const bool wasempty = workingdir.empty();
		epro::path_stringview::size_type pos1, pos2 = 0;
		while((pos1 = path.find(EPRO_TEXT('/'), pos2)) != epro::path_stringview::npos) {
			if(pos1 != pos2) {
				if(pos2 != 0 || !wasempty)
					workingdir.append(1, EPRO_TEXT('/'));
				workingdir.append(path.begin() + pos2, path.begin() + pos1);
				if(!MakeDirectory(workingdir))
					return false;
			}
			pos2 = pos1 + 1;
		}
		return true;
	}

	const epro::path_string& Utils::GetExePath() {
		static const epro::path_string binarypath = []()->epro::path_string {
#ifdef _WIN32
			TCHAR exepath[MAX_PATH];
			GetModuleFileName(nullptr, exepath, MAX_PATH);
			return Utils::NormalizePath<TCHAR>(exepath, false);
#elif defined(__linux__) && !defined(__ANDROID__)
			epro::path_char buff[PATH_MAX];
			ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff) - 1);
			if(len != -1)
				buff[len] = EPRO_TEXT('\0');
			return buff;
#elif defined(__APPLE__)
			CFURLRef bundle_url = CFBundleCopyBundleURL(CFBundleGetMainBundle());
			CFStringRef bundle_path = CFURLCopyFileSystemPath(bundle_url, kCFURLPOSIXPathStyle);
			CFURLRef bundle_base_url = CFURLCreateCopyDeletingLastPathComponent(nullptr, bundle_url);
			CFRelease(bundle_url);
			CFStringRef path = CFURLCopyFileSystemPath(bundle_base_url, kCFURLPOSIXPathStyle);
			CFRelease(bundle_base_url);
			/*
			#ifdef MAC_OS_DISCORD_LAUNCHER
				system(fmt::format("open {}/Contents/MacOS/discord-launcher.app --args random", CFStringGetCStringPtr(bundle_path, kCFStringEncodingUTF8)).data());
			#endif
			*/
			epro::path_string res = epro::path_string(CFStringGetCStringPtr(path, kCFStringEncodingUTF8)) + "/";
			CFRelease(path);
			CFRelease(bundle_path);
			return res;
#else
			return EPRO_TEXT("");
#endif
		}();
		return binarypath;
	}

	const epro::path_string& Utils::GetExeFolder() {
		static const epro::path_string binarypath = GetFilePath(GetExePath());
		return binarypath;
	}

	const epro::path_string& Utils::GetCorePath() {
		static const epro::path_string binarypath = [] {
#ifdef _WIN32
			return fmt::format(EPRO_TEXT("{}/ocgcore.dll"), GetExeFolder());
#else
			return EPRO_TEXT(""); // Unused on POSIX
#endif
		}();
		return binarypath;
	}

	bool Utils::UnzipArchive(epro::path_stringview input, unzip_callback callback, unzip_payload* payload, epro::path_stringview dest) {
		thread_local char buff[0x2000];
		constexpr int buff_size = sizeof(buff) / sizeof(*buff);
		if(!filesystem)
			return false;
		CreatePath(dest, EPRO_TEXT("./"));
		irr::io::IFileArchive* archive = nullptr;
		if(!filesystem->addFileArchive({ input.data(), (irr::u32)input.size() }, false, false, irr::io::EFAT_ZIP, "", &archive))
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
			auto filename = filelist->getFullFileName(i);
			bool isdir = filelist->isDirectory(i);
			if(isdir)
				CreatePath(fmt::format(EPRO_TEXT("{}/"), filename), { dest.data(), dest.size() });
			else
				CreatePath({ filename.data(), filename.size() }, { dest.data(), dest.size() });
			if(!isdir) {
				int percentage = 0;
				auto reader = archive->createAndOpenFile(i);
				if(reader) {
#if defined(__MINGW32__) && defined(UNICODE)
					auto fd = _wopen(fmt::format(EPRO_TEXT("{}/{}"), dest, filename).data(), _O_WRONLY | _O_BINARY);
					if(fd == -1)
						return false;
					__gnu_cxx::stdio_filebuf<char> b(fd, std::ios::out);
					std::ostream out(&b);
#else
					std::ofstream out(fmt::format(EPRO_TEXT("{}/{}"), dest, filename), std::ofstream::binary);
#endif
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
					reader->drop();
				}
			}
		}
		filesystem->removeFileArchive(archive);
		archive->drop();
		return true;
	}

	void Utils::SystemOpen(epro::path_stringview arg, OpenType type) {
#ifdef _WIN32
		ShellExecute(nullptr, EPRO_TEXT("open"), (type == OPEN_FILE) ? fmt::format(EPRO_TEXT("{}/{}"), working_dir, arg).data() : arg.data(), nullptr, nullptr, SW_SHOWNORMAL);
#elif !defined(__ANDROID__)
#ifdef __APPLE__
#define OPEN "open"
#else
#define OPEN "xdg-open"
#endif
		auto pid = vfork();
		if(pid == 0) {
			execl("/usr/bin/" OPEN, OPEN, arg.data(), nullptr);
			_exit(EXIT_FAILURE);
		} else if(pid < 0)
			perror("Failed to fork:");
		if(waitpid(pid, nullptr, WNOHANG) != 0)
			perror("Failed to open arg or file:");
#else
		if(type == OPEN_FILE)
			porting::openFile(fmt::format("{}/{}", working_dir, arg));
		else
			porting::openUrl(arg);
#endif
	}
}

