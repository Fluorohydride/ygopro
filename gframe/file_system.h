#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <cstdio>
#include <functional>

// was a function before std::fopen support UTF-8 filename on Windows
#define myfopen std::fopen

FILE* mywfopen(const wchar_t* filename, const char* mode);

class FileSystem {
public:
	static void SafeFileName(wchar_t* wfile);
	static bool IsFileExists(const wchar_t* wfile);
	static bool IsFileExists(const char* file);
	static bool IsDirExists(const wchar_t* wdir);
	static bool IsDirExists(const char* dir);
	static bool MakeDir(const wchar_t* wdir);
	static bool MakeDir(const char* dir);
	static bool Rename(const wchar_t* woldname, const wchar_t* wnewname);
	static bool Rename(const char* oldname, const char* newname);
	static bool DeleteDir(const wchar_t* wdir);
	static bool DeleteDir(const char* dir);
	static bool RemoveFile(const wchar_t* wfile);
	static bool RemoveFile(const char* file);
	static void TraversalDir(const wchar_t* wpath, const std::function<void(const wchar_t*, bool)>& cb);
	static void TraversalDir(const char* path, const std::function<void(const char*, bool)>& cb);
};

#endif //FILESYSTEM_H
