#ifndef UTILS_H
#define UTILS_H

#include <algorithm>
#include <cctype>
#include <cwctype>
#include <functional>
#include <map>
#include <string>
#include <vector>
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
	class Utils {
	public:
		static std::vector<irr::io::IFileArchive*> archives;
		static irr::io::IFileSystem* filesystem;
		static bool MakeDirectory(const path_string& path);
		static bool FileCopy(const path_string& source, const path_string& destination);
		static bool FileMove(const path_string& source, const path_string& destination);
		static path_string ToPathString(const std::wstring& input);
		static path_string ToPathString(const std::string& input);
		static std::string ToUTF8IfNeeded(const path_string& input);
		static std::wstring ToUnicodeIfNeeded(const path_string& input);
		static bool FileDelete(const path_string& source);
		static bool ClearDirectory(const path_string& path);
		static bool DeleteDirectory(const path_string& source);
		static void CreateResourceFolders();
		static void FindFiles(const path_string& path, const std::function<void(path_string, bool, void*)>& cb, void* payload = nullptr);
		static std::vector<path_string> FindFiles(const path_string& path, std::vector<path_string> extensions, int subdirectorylayers = 0);
		/** Returned subfolder names are prefixed by the provided path */
		static std::vector<path_string> FindSubfolders(const path_string& path, int subdirectorylayers = 1, bool addparentpath = true);
		static std::vector<int> FindFiles(irr::io::IFileArchive* archive, const path_string& path, std::vector<path_string> extensions, int subdirectorylayers = 0);
		static irr::io::IReadFile* FindFileInArchives(const path_string& path, const path_string& name);
		template<typename T>
		static T NormalizePath(T path, bool trailing_slash = true);
		template<typename T>
		static T GetFileExtension(T file);
		template<typename T>
		static T GetFilePath(T file);
		template<typename T>
		static T GetFileName(T file, bool keepextension = false);

		template<typename T>
		static std::vector<T> TokenizeString(const T& input, const T& token);
		template<typename T>
		static std::vector<T> TokenizeString(const T& input, const typename T::value_type& token);
		template<typename T>
		static T ToUpperNoAccents(T input);
		/** Returns true if and only if all tokens are contained in the input. */
		static bool ContainsSubstring(const std::wstring& input, const std::vector<std::wstring>& tokens, bool convertInputCasing = false, bool convertTokenCasing = false);
		static bool ContainsSubstring(const std::wstring& input, const std::wstring& token, bool convertInputCasing = false, bool convertTokenCasing = false);
		template<typename T>
		static bool KeepOnlyDigits(T& input, bool negative = false);
		template<typename T>
		static inline bool EqualIgnoreCase(const T& a, const T& b) {
			return Utils::ToUpperNoAccents(a) == Utils::ToUpperNoAccents(b);
		};
		template<typename T>
		static inline bool CompareIgnoreCase(const T& a, const T& b) {
			return Utils::ToUpperNoAccents(a) < Utils::ToUpperNoAccents(b);
		};
		static bool CreatePath(const path_string& path, const path_string& workingdir = EPRO_TEXT("./"));
		static const path_string& GetExePath();
		static const path_string& GetExeFolder();
		static const path_string& GetCorePath();
		static bool UnzipArchive(const path_string& input, unzip_callback callback = nullptr, unzip_payload* payload = nullptr, const path_string& dest = EPRO_TEXT("./"));

		static void SystemOpen(const path_string& url);
	};

#define CHAR_T typename T::value_type
#define CAST(c) static_cast<CHAR_T>(c)
template<typename T>
T Utils::NormalizePath(T path, bool trailing_slash) {
	const T prev{ CAST('.'), CAST('.') };
	const T cur{ CAST('.') };
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
inline T Utils::ToUpperNoAccents(T input) {
	std::transform(input.begin(), input.end(), input.begin(), [](CHAR_T c) {
#define IN_INTERVAL(start, end) (c >= start && c <= end)
		if (IN_INTERVAL(192, 197) || IN_INTERVAL(224, 229)) {
			return CAST('A');
		}
		if (IN_INTERVAL(192, 197) || IN_INTERVAL(224, 229)) {
			return CAST('E');
		}
		if (IN_INTERVAL(200, 203) || IN_INTERVAL(232, 235)) {
			return CAST('I');
		}
		if (IN_INTERVAL(210, 214) || IN_INTERVAL(242, 246)) {
			return CAST('O');
		}
		if (IN_INTERVAL(217, 220) || IN_INTERVAL(249, 252)) {
			return CAST('U');
		}
		if (c == 209 || c == 241) {
			return CAST('N');
		}
		if (std::is_same<CHAR_T, wchar_t>::value)
			return CAST(std::towupper(c));
		else
			return CAST(std::toupper(c));
#undef IN_INTERVAL
	});
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
		if ((unsigned)(*it - CAST('0')) > 9) {
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

}

#endif //UTILS_H
