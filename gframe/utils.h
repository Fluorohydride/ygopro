#ifndef UTILS_H
#define UTILS_H

#include <algorithm>
#include <cctype>
#include <cwctype>
#include <functional>
#include <map>
#include <memory> // unique_ptr
#include <string>
#include <vector>
#include <mutex>
#include "bufferio.h"
#include "text_types.h"

namespace irr {
namespace io {
class IFileArchive;
class IFileSystem;
class IReadFile;
}
}

struct unzip_payload {
	int percentage;
	int cur;
	int tot;
	const path_char* filename;
	bool is_new;
	void* payload;
};

using unzip_callback = std::function<void(unzip_payload* payload)>;

namespace ygo {
	// Irrlicht has only one handle open per archive, which does not support concurrency and is not thread-safe.
	// Thus, we need to own a corresponding mutex that is also used for all files from this archive
	class SynchronizedIrrArchive {
	public:
		std::unique_ptr<std::mutex> mutex;
		irr::io::IFileArchive* archive;
		SynchronizedIrrArchive(irr::io::IFileArchive* archive) : mutex(std::make_unique<std::mutex>()), archive(archive) {}
	};
	// Mutex should already be locked if not nullptr and takes ownership of the IReadFile if not nullptr
	// In C++17, use std::optional, since we have either file exists or not found (both nullptr)
	class MutexLockedIrrArchivedFile {
	public:
		std::mutex* mutex; // from SynchronizedIrrArchive
		irr::io::IReadFile* reader;
		MutexLockedIrrArchivedFile(std::mutex* mutex = nullptr, irr::io::IReadFile* reader = nullptr) noexcept : mutex(mutex), reader(reader) {}
		MutexLockedIrrArchivedFile(const MutexLockedIrrArchivedFile& copyFrom) = delete;
		MutexLockedIrrArchivedFile(MutexLockedIrrArchivedFile&& moveFrom) noexcept {
			mutex = moveFrom.mutex;
			reader = moveFrom.reader;
			moveFrom.mutex = nullptr;
			moveFrom.reader = nullptr;
		}
		MutexLockedIrrArchivedFile& operator= (MutexLockedIrrArchivedFile&& moveFrom) noexcept {
			mutex = moveFrom.mutex;
			reader = moveFrom.reader;
			moveFrom.mutex = nullptr;
			moveFrom.reader = nullptr;
			return *this;
		}
		~MutexLockedIrrArchivedFile(); // drops reader if not nullptr, then unlocks mutex if not nullptr
		operator bool() const {
			return reader;
		}
	};
	class Utils {
	public:
		template<std::size_t N>
		static constexpr void SetThreadName(char const (&s)[N]) {
			static_assert(N <= 16, "Thread name on posix can't be more than 16 bytes!");
			InternalSetThreadName(s);
		}
		
		static std::vector<SynchronizedIrrArchive> archives;
		static irr::io::IFileSystem* filesystem;
		static path_string working_dir;
		static bool MakeDirectory(path_stringview path);
		static bool FileCopy(path_stringview source, path_stringview destination);
		static bool FileMove(path_stringview source, path_stringview destination);
		static bool FileExists(path_stringview path);
		static inline path_string ToPathString(epro_wstringview input);
		static inline path_string ToPathString(epro_stringview input);
		static inline std::string ToUTF8IfNeeded(path_stringview input);
		static inline std::wstring ToUnicodeIfNeeded(path_stringview input);
		static bool ChangeDirectory(path_stringview newpath);
		static bool FileDelete(path_stringview source);
		static bool ClearDirectory(path_stringview path);
		static bool DeleteDirectory(path_stringview source);
		static void CreateResourceFolders();
		static void FindFiles(path_stringview path, const std::function<void(path_stringview, bool, void*)>& cb, void* payload = nullptr);
		static std::vector<path_string> FindFiles(path_stringview path, const std::vector<path_stringview>& extensions, int subdirectorylayers = 0);
		/** Returned subfolder names are prefixed by the provided path */
		static std::vector<path_string> FindSubfolders(path_stringview path, int subdirectorylayers = 1, bool addparentpath = true);
		static std::vector<int> FindFiles(irr::io::IFileArchive* archive, path_stringview path, const std::vector<path_stringview>& extensions, int subdirectorylayers = 0);
		static MutexLockedIrrArchivedFile FindFileInArchives(path_stringview path, path_stringview name);
		template<typename T>
		static T NormalizePath(T path, bool trailing_slash = true);
		template<typename T>
		static T GetFileExtension(T file);
		template<typename T>
		static T GetFilePath(T file);
		template<typename T>
		static T GetFileName(T file, bool keepextension = false);

		template<typename T>
		static inline std::vector<T> TokenizeString(const T& input, const T& token);
		template<typename T>
		static inline std::vector<T> TokenizeString(const T& input, const typename T::value_type& token);
		template<typename T>
		static T ToUpperChar(T c);
		template<typename T>
		static T ToUpperNoAccents(T input);
		/** Returns true if and only if all tokens are contained in the input. */
		static bool ContainsSubstring(epro_wstringview input, const std::vector<std::wstring>& tokens, bool convertInputCasing = false, bool convertTokenCasing = false);
		static bool ContainsSubstring(epro_wstringview input, epro_wstringview token, bool convertInputCasing = false, bool convertTokenCasing = false);
		template<typename T>
		static bool KeepOnlyDigits(T& input, bool negative = false);
		template<typename T>
		static inline bool EqualIgnoreCase(const T& a, const T& b) {
			return std::equal(a.begin(), a.end(), b.begin(), b.end(), [](const typename T::value_type& _a, const typename T::value_type& _b) {
				return ToUpperChar(_a) == ToUpperChar(_b);
			});
		};
		template<typename T>
		static inline bool CompareIgnoreCase(const T& a, const T& b) {
			return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end(), [](const typename T::value_type& _a, const typename T::value_type& _b) {
				return ToUpperChar(_a) < ToUpperChar(_b);
			});
			//return Utils::ToUpperNoAccents(a) < Utils::ToUpperNoAccents(b);
		};
		static bool CreatePath(path_stringview path, path_stringview workingdir = EPRO_TEXT("./"));
		static path_stringview GetExePath();
		static path_stringview GetExeFolder();
		static path_stringview GetCorePath();
		static bool UnzipArchive(path_stringview input, unzip_callback callback = nullptr, unzip_payload* payload = nullptr, path_stringview dest = EPRO_TEXT("./"));

		enum OpenType {
			OPEN_URL,
			OPEN_FILE
		};

		static void SystemOpen(path_stringview url, OpenType type = OPEN_URL);

	private:
		static void InternalSetThreadName(const char* name);
	};

#define CHAR_T typename T::value_type
#define CAST(c) static_cast<CHAR_T>(c)
template<typename T>
T Utils::NormalizePath(T path, bool trailing_slash) {
	static const T prev{ CAST('.'), CAST('.') };
	static const T cur{ CAST('.') };
	constexpr auto slash = CAST('/');
	std::replace(path.begin(), path.end(), CAST('\\'), slash);
	auto paths = TokenizeString(path, slash);
	if(paths.empty())
		return path;
	T normalpath;
	for(auto it = paths.begin(); it != paths.end();) {
		if(it->empty() && it != paths.begin()) {
			it = paths.erase(it);
			continue;
		}
		if((*it) == cur && it != paths.begin()) {
			it = paths.erase(it);
			continue;
		}
		if((*it) != prev && it != paths.begin() && (it + 1) != paths.end() && (*(it + 1)) == prev) {
			it = paths.erase(paths.erase(it));
			continue;
		}
		it++;
	}
	for(auto it = paths.begin(); it != (paths.end() - 1); it++) {
		normalpath += *it + slash;
	}
	normalpath += paths.back();
	if(trailing_slash)
		normalpath += slash;
	return normalpath;
}

template<typename T>
T Utils::GetFileExtension(T file) {
	size_t dotpos = file.find_last_of(CAST('.'));
	if(dotpos == T::npos)
		return T();
	T extension = file.substr(dotpos + 1);
	std::transform(extension.begin(), extension.end(), extension.begin(), ::towlower);
	return extension;
}

template<typename T>
T Utils::GetFilePath(T file) {
	std::replace(file.begin(), file.end(), CAST('\\'), CAST('/'));
	size_t slashpos = file.find_last_of(CAST('/'));
	if(slashpos == T::npos)
		return T{ CAST('.'),CAST('/') };
	T path = file.substr(0, slashpos + 1);
	return path;
}

template<typename T>
T Utils::GetFileName(T file, bool keepextension) {
	std::replace(file.begin(), file.end(), CAST('\\'), CAST('/'));
	size_t dashpos = file.find_last_of(CAST('/'));
	if(dashpos == T::npos)
		dashpos = 0;
	else
		dashpos++;
	size_t dotpos = file.size();
	if(!keepextension) {
		dotpos = file.find_last_of(CAST('.'));
		if(dotpos == T::npos)
			dotpos = file.size();
	}
	T name = file.substr(dashpos, dotpos - dashpos);
	return name;
}

template<typename T>
inline std::vector<T> Utils::TokenizeString(const T& input, const T& token) {
	std::vector<T> res;
	typename T::size_type pos1, pos2 = 0;
	while((pos1 = input.find(token, pos2)) != T::npos) {
		if(pos1 != pos2)
			res.emplace_back(input.begin() + pos2, input.begin() + pos1);
		pos2 = pos1 + token.size();
	}
	if(pos2 != input.size())
		res.emplace_back(input.begin() + pos2, input.end());
	return res;
}

template<typename T>
inline std::vector<T> Utils::TokenizeString(const T & input, const typename T::value_type & token) {
	std::vector<T> res;
	typename T::size_type pos1, pos2 = 0;
	while((pos1 = input.find(token, pos2)) != T::npos) {
		if(pos1 != pos2)
			res.emplace_back(input.begin() + pos2, input.begin() + pos1);
		pos2 = pos1 + 1;
	}
	if(pos2 != input.size())
		res.emplace_back(input.begin() + pos2, input.end());
	return res;
}

template<typename T>
T Utils::ToUpperChar(T c) {
#define IN_INTERVAL(start, end) (c >= start && c <= end)
	if(IN_INTERVAL(192, 197) || IN_INTERVAL(224, 229)) {
		return static_cast<T>('A');
	}
	if(IN_INTERVAL(192, 197) || IN_INTERVAL(224, 229)) {
		return static_cast<T>('E');
	}
	if(IN_INTERVAL(200, 203) || IN_INTERVAL(232, 235)) {
		return static_cast<T>('I');
	}
	if(IN_INTERVAL(210, 214) || IN_INTERVAL(242, 246)) {
		return static_cast<T>('O');
	}
	if(IN_INTERVAL(217, 220) || IN_INTERVAL(249, 252)) {
		return static_cast<T>('U');
	}
	if(c == 209 || c == 241) {
		return static_cast<T>('N');
	}
	if(std::is_same<T, wchar_t>::value)
		return static_cast<T>(std::towupper(c));
	else
		return static_cast<T>(std::toupper(c));
#undef IN_INTERVAL
}

template<typename T>
inline T Utils::ToUpperNoAccents(T input) {
	std::transform(input.begin(), input.end(), input.begin(), ToUpperChar<CHAR_T>);
	return input;
}

template<typename T>
inline bool Utils::KeepOnlyDigits(T& input, bool negative) {
	bool changed = false;
	for (auto it = input.begin(); it != input.end();) {
		if(*it == CAST('-') && negative && it == input.begin()) {
			it++;
			continue;
		}
		if ((uint32_t)(*it - CAST('0')) > 9) {
			it = input.erase(it);
			changed = true;
			continue;
		}
		it++;
	}
	return changed;
}
#undef CAST
#undef CHAR_T

path_string Utils::ToPathString(epro_wstringview input) {
#ifdef UNICODE
	return { input.data(), input.size() };
#else
	return BufferIO::EncodeUTF8s(input);
#endif
}
path_string Utils::ToPathString(epro_stringview input) {
#ifdef UNICODE
	return BufferIO::DecodeUTF8s(input);
#else
	return { input.data(), input.size() };
#endif
}
std::string Utils::ToUTF8IfNeeded(path_stringview input) {
#ifdef UNICODE
	return BufferIO::EncodeUTF8s(input);
#else
	return { input.data(), input.size() };
#endif
}
std::wstring Utils::ToUnicodeIfNeeded(path_stringview input) {
#ifdef UNICODE
	return { input.data(), input.size() };
#else
	return BufferIO::DecodeUTF8s(input);
#endif
}

}

#endif //UTILS_H
