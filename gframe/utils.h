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
class IOSOperator;
}

struct unzip_payload {
	int percentage;
	int cur;
	int tot;
	const epro::path_char* filename;
	bool is_new;
	void* payload;
};

using unzip_callback = void(*)(unzip_payload* payload);

namespace ygo {
	// Irrlicht has only one handle open per archive, which does not support concurrency and is not thread-safe.
	// Thus, we need to own a corresponding mutex that is also used for all files from this archive
	class SynchronizedIrrArchive {
	public:
		std::unique_ptr<std::mutex> mutex;
		irr::io::IFileArchive* archive;
		SynchronizedIrrArchive(irr::io::IFileArchive* archive) : mutex(std::make_unique<std::mutex>()), archive(archive) {}
	};
	class Utils {
	public:
		template<std::size_t N>
		static inline void SetThreadName(char const (&s)[N], wchar_t const (&ws)[N]) {
			static_assert(N <= 16, "Thread name on posix can't be more than 16 bytes!");
			InternalSetThreadName(s, ws);
		}
		
		static std::vector<SynchronizedIrrArchive> archives;
		static irr::io::IFileSystem* filesystem;
		static irr::IOSOperator* OSOperator;
		static epro::path_string working_dir;
		static bool MakeDirectory(epro::path_stringview path);
		static bool FileCopy(epro::path_stringview source, epro::path_stringview destination);
		static bool FileMove(epro::path_stringview source, epro::path_stringview destination);
		static bool FileExists(epro::path_stringview path);
		static inline epro::path_string ToPathString(epro::wstringview input);
		static inline epro::path_string ToPathString(epro::stringview input);
		static inline std::string ToUTF8IfNeeded(epro::path_stringview input);
		static inline std::wstring ToUnicodeIfNeeded(epro::path_stringview input);
		static bool ChangeDirectory(epro::path_stringview newpath);
		static bool FileDelete(epro::path_stringview source);
		static bool ClearDirectory(epro::path_stringview path);
		static bool DeleteDirectory(epro::path_stringview source);
		static void CreateResourceFolders();
		static void FindFiles(epro::path_stringview path, const std::function<void(epro::path_stringview, bool)>& cb);
		static std::vector<epro::path_string> FindFiles(epro::path_stringview path, const std::vector<epro::path_stringview>& extensions, int subdirectorylayers = 0);
		/** Returned subfolder names are prefixed by the provided path */
		static std::vector<epro::path_string> FindSubfolders(epro::path_stringview path, int subdirectorylayers = 1, bool addparentpath = true);
		static std::vector<int> FindFiles(irr::io::IFileArchive* archive, epro::path_stringview path, const std::vector<epro::path_stringview>& extensions, int subdirectorylayers = 0);
		static irr::io::IReadFile* FindFileInArchives(epro::path_stringview path, epro::path_stringview name);

#define DECLARE_STRING_VIEWED(funcname) \
		template<typename T, typename... Args>\
		static inline auto funcname(const T& _arg, Args&&... args) {\
			const epro::basic_string_view<typename T::value_type> arg{ _arg.data(), _arg.size() };\
			return funcname##Impl(arg, std::forward<Args>(args)...);\
		}\
		template<typename T, typename... Args>\
		static inline auto funcname(const T* _arg, Args&&... args) {\
			const epro::basic_string_view<T> arg{ _arg };\
			return funcname##Impl(arg, std::forward<Args>(args)...);\
		}

		DECLARE_STRING_VIEWED(NormalizePath)

		DECLARE_STRING_VIEWED(GetFileExtension)

		DECLARE_STRING_VIEWED(GetFilePath)

		DECLARE_STRING_VIEWED(GetFileName)

#undef DECLARE_STRING_VIEWED

		static epro::stringview GetUserAgent();

		static epro::path_string GetAbsolutePath(epro::path_stringview path);

		template<typename T>
		static inline std::vector<T> TokenizeString(const T& input, const T& token);
		template<typename T>
		static inline std::vector<T> TokenizeString(const T& input, const typename T::value_type& token);
		template<typename T>
		static T ToUpperChar(T c);
		template<typename T>
		static T ToUpperNoAccents(T input);
		/** Returns true if and only if all tokens are contained in the input. */
		static bool ContainsSubstring(epro::wstringview input, const std::vector<std::wstring>& tokens);
		template<typename T>
		static bool KeepOnlyDigits(T& input, bool negative = false);
		template<typename T>
		static inline bool EqualIgnoreCase(const T& a, const T& b) {
			return std::equal(a.begin(), a.end(), b.begin(), b.end(), [](const typename T::value_type& _a, const typename T::value_type& _b) {
				return ToUpperChar(_a) == ToUpperChar(_b);
			});
		};
		template<typename T>
		static inline bool EqualIgnoreCaseFirst(const T& a, const T& b) {
			return std::equal(a.begin(), a.end(), b.begin(), b.end(), [](const typename T::value_type& _a, const typename T::value_type& _b) {
				return _a == ToUpperChar(_b);
			});
		};
		template<typename T>
		static inline bool CompareIgnoreCase(const T& a, const T& b) {
			return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end(), [](const typename T::value_type& _a, const typename T::value_type& _b) {
				return ToUpperChar(_a) < ToUpperChar(_b);
			});
			//return Utils::ToUpperNoAccents(a) < Utils::ToUpperNoAccents(b);
		};
		static bool CreatePath(epro::path_stringview path, epro::path_string workingdir = EPRO_TEXT("./"));
		static epro::path_stringview GetExePath();
		static epro::path_stringview GetExeFolder();
		static epro::path_stringview GetCorePath();
		static bool UnzipArchive(epro::path_stringview input, unzip_callback callback = nullptr, unzip_payload* payload = nullptr, epro::path_stringview dest = EPRO_TEXT("./"));

		enum OpenType {
			OPEN_URL,
			OPEN_FILE
		};

		static void SystemOpen(epro::path_stringview arg, OpenType type = OPEN_URL);

	private:
		static void InternalSetThreadName(const char* name, const wchar_t* wname);
		template<typename T>
		static auto NormalizePathImpl(const epro::basic_string_view<T>& path, bool trailing_slash = true);
		template<typename T>
		static auto GetFileExtensionImpl(const epro::basic_string_view<T>& file, bool convert_case = true);
		template<typename T>
		static auto GetFilePathImpl(const epro::basic_string_view<T>& file);
		template<typename T>
		static auto GetFileNameImpl(const epro::basic_string_view<T>& file, bool keepextension = false);
	};
	
#define CHAR_T_STRING(text) epro::basic_string_view<T>{ std::is_same<T, wchar_t>::value ? reinterpret_cast<const T*>(L ##text) : reinterpret_cast<const T*>(text) }
#define CAST(c) static_cast<T>(c)
template<typename T>
auto Utils::NormalizePathImpl(const epro::basic_string_view<T>& _path, bool trailing_slash) {
	std::basic_string<T> path{ _path.data(), _path.size() };
	static const auto cur = CHAR_T_STRING(".");
	static const auto prev = CHAR_T_STRING("..");
	static constexpr auto slash = CAST('/');
	std::replace(path.begin(), path.end(), CAST('\\'), slash);
	auto paths = TokenizeString(path, slash);
	if(paths.empty())
		return path;
	bool is_absolute = path.size() && path[0] == slash;
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
	path.clear();
	for(auto it = paths.begin(); it != (paths.end() - 1); it++)
		path += *it + slash;
	path += paths.back();
	if(trailing_slash)
		path += slash;
	if(is_absolute)
		path = slash + path;
	return path;
}
#undef CHAR_T_STRING

template<typename T>
auto Utils::GetFileExtensionImpl(const epro::basic_string_view<T>& file, bool convert_case) {
	using rettype = std::basic_string<T>;
	size_t dotpos = file.find_last_of(CAST('.'));
	if(dotpos == file.npos)
		return rettype();
	rettype ret(file.substr(dotpos + 1));
	if(convert_case)
		std::transform(ret.begin(), ret.end(), ret.begin(), ::towlower);
	return ret;
}

template<typename T>
auto Utils::GetFilePathImpl(const epro::basic_string_view<T>& _file) {
	using rettype = std::basic_string<T>;
	static const rettype current{ CAST('.'),CAST('/') };
	rettype file{ _file.data(), _file.size() };
	std::replace(file.begin(), file.end(), CAST('\\'), CAST('/'));
	size_t slashpos = file.find_last_of(CAST('/'));
	if(slashpos == file.npos)
		return current;
	file.erase(slashpos + 1);
	return file;
}

template<typename T>
auto Utils::GetFileNameImpl(const epro::basic_string_view<T>& _file, bool keepextension) {
	using rettype = std::basic_string<T>;
	rettype file{ _file.data(), _file.size() };
	std::replace(file.begin(), file.end(), CAST('\\'), CAST('/'));
	size_t dashpos = file.find_last_of(CAST('/'));
	if(dashpos == file.npos)
		dashpos = 0;
	else
		dashpos++;
	size_t dotpos = file.size();
	if(!keepextension) {
		auto _dotpos = file.find_last_of(CAST('.'));
		if(_dotpos != file.npos)
			dotpos = _dotpos;
	}
	file.erase(dotpos);
	file.erase(0, dashpos);
	return file;
}
#undef CAST

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
inline std::vector<T> Utils::TokenizeString(const T& input, const typename T::value_type& token) {
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
	if(std::is_same<T, wchar_t>::value) {
		if(IN_INTERVAL(192, 197) || IN_INTERVAL(224, 229)
		   || c == 0x2c6f || c == 0x250 //latin capital/small letter turned a
		   || c == 0x2200) //for all
		{
			return static_cast<T>('A');
		}
		if(IN_INTERVAL(200, 203) || IN_INTERVAL(232, 235)) {
			return static_cast<T>('E');
		}
		if(IN_INTERVAL(204, 207) || IN_INTERVAL(236, 239)) {
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
		if(c == 161) { //inverted exclamation mark
			return static_cast<T>('!');
		}
		if(c == 191) { //inverted question mark
			return static_cast<T>('?');
		}
		return static_cast<T>(std::towupper(c));
	} else
		return static_cast<T>(std::toupper(c));
#undef IN_INTERVAL
}

template<typename T>
inline T Utils::ToUpperNoAccents(T input) {
	std::transform(input.begin(), input.end(), input.begin(), ToUpperChar<typename T::value_type>);
	return input;
}

template<typename T>
inline bool Utils::KeepOnlyDigits(T& input, bool negative) {
	bool changed = false;
	for (auto it = input.begin(); it != input.end();) {
		if(*it == static_cast<typename T::value_type>('-') && negative && it == input.begin()) {
			it++;
			continue;
		}
		if ((uint32_t)(*it - static_cast<typename T::value_type>('0')) > 9) {
			it = input.erase(it);
			changed = true;
			continue;
		}
		it++;
	}
	return changed;
}

inline epro::path_string Utils::ToPathString(epro::wstringview input) {
#ifdef UNICODE
	return { input.data(), input.size() };
#else
	return BufferIO::EncodeUTF8(input);
#endif
}
inline epro::path_string Utils::ToPathString(epro::stringview input) {
#ifdef UNICODE
	return BufferIO::DecodeUTF8(input);
#else
	return { input.data(), input.size() };
#endif
}
inline std::string Utils::ToUTF8IfNeeded(epro::path_stringview input) {
#ifdef UNICODE
	return BufferIO::EncodeUTF8(input);
#else
	return { input.data(), input.size() };
#endif
}
inline std::wstring Utils::ToUnicodeIfNeeded(epro::path_stringview input) {
#ifdef UNICODE
	return { input.data(), input.size() };
#else
	return BufferIO::DecodeUTF8(input);
#endif
}

}
#define SetThreadName(name) SetThreadName(name, L##name)

#endif //UTILS_H
