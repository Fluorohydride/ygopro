#ifndef UTILS_H
#define UTILS_H

#include <algorithm>
#include <cctype>
#include <cwctype>
#include <functional>
#include <map>
#include <string>
#include <vector>

#ifndef TEXT
#ifdef UNICODE
#define TEXT(x) L##x
using path_string = std::basic_string<wchar_t>;
#else
#define TEXT(x) x
using path_string = std::basic_string<char>;
#endif // UNICODE
#endif

namespace irr {
namespace io {
class IFileArchive;
class IReadFile;
}
}

namespace ygo {
	class Utils {
	public:
		class IrrArchiveHelper {
		public:
			irr::io::IFileArchive* archive;
			std::map<path_string/*folder name*/, std::pair<std::pair<int/*begin folder offset*/, int/*end folder offset*/>, std::pair<int/*begin file offset*/, int/*end file offset*/>>> folderindexes;
			IrrArchiveHelper(irr::io::IFileArchive* archive) { ParseList(archive); };
			void ParseList(irr::io::IFileArchive* archive);
		};
		static std::vector<IrrArchiveHelper> archives;
		static bool Makedirectory(const path_string& path);
		static bool Movefile(const path_string& source, const path_string& destination);
		static path_string ParseFilename(const std::wstring& input);
		static path_string ParseFilename(const std::string& input);
		static std::string ToUTF8IfNeeded(const path_string& input);
		static std::wstring ToUnicodeIfNeeded(const path_string& input);
		static bool Deletefile(const path_string& source);
		static bool ClearDirectory(const path_string& path);
		static bool Deletedirectory(const path_string& source);
		static void CreateResourceFolders();
		static void FindfolderFiles(const path_string& path, const std::function<void(path_string, bool, void*)>& cb, void* payload = nullptr);
		static std::vector<path_string> FindfolderFiles(const path_string& path, std::vector<path_string> extensions, int subdirectorylayers = 0);
		/** Returned subfolder names are prefixed by the provided path */
		static std::vector<path_string> FindSubfolders(const path_string& path, int subdirectorylayers = 1);
		static void FindfolderFiles(const IrrArchiveHelper& archive, const path_string& path, const std::function<bool(int, path_string, bool, void*)>& cb, void* payload = nullptr);
		static std::vector<int> FindfolderFiles(const IrrArchiveHelper& archive, const path_string& path, std::vector<path_string> extensions, int subdirectorylayers = 0);
		static irr::io::IReadFile* FindandOpenFileFromArchives(const path_string& path, const path_string& name);
		static std::wstring NormalizePath(std::wstring path, bool trailing_slash = true);
		static std::wstring GetFileExtension(std::wstring file);
		static std::wstring GetFilePath(std::wstring file);
		static std::wstring GetFileName(std::wstring file);
		static std::string NormalizePath(std::string path, bool trailing_slash = true);
		static std::string GetFileExtension(std::string file);
		static std::string GetFilePath(std::string file);
		static std::string GetFileName(std::string file);

		template<typename T>
		static std::vector<T> TokenizeString(const T& input, const T& token);
		template<typename T>
		static T ToUpperNoAccents(T input);
		/** Returns true if and only if all tokens are contained in the input. */
		static bool ContainsSubstring(std::wstring input, const std::vector<std::wstring>& tokens, bool ignoreInputCasingAccents = false, bool ignoreTokenCasingAccents = false);
		static bool ContainsSubstring(std::wstring input, std::wstring token, bool ignoreInputCasingAccents = false, bool ignoreTokenCasingAccents = false);
	};

template<typename T>
inline std::vector<T> Utils::TokenizeString(const T& input, const T& token) {
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
	std::transform(input.begin(), input.end(), input.begin(), [](wchar_t c) {
#define IN_INTERVAL(start, end) (c >= start && c <= end)
#define CHAR_T typename T::value_type
#define CAST(c) static_cast<CHAR_T>(c)
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
#undef CAST
#undef CHAR_T
#undef IN_INTERVAL
	});
	return input;
}

}

#endif //UTILS_H
