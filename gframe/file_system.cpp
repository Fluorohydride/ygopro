#include "file_system.h"
#include "bufferio.h"

FILE* mywfopen(const wchar_t* filename, const char* mode) {
	FILE* fp{};
#ifdef _WIN32
	wchar_t wmode[20]{};
	BufferIO::CopyCharArray(mode, wmode);
	fp = _wfopen(filename, wmode);
#else
	char fname[1024]{};
	BufferIO::EncodeUTF8(filename, fname);
	fp = std::fopen(fname, mode);
#endif
	return fp;
}

#ifdef _WIN32

#include <Windows.h>
#include <shellapi.h>

void FileSystem::SafeFileName(wchar_t* wfile) {
	while((wfile = std::wcspbrk(wfile, L"<>:\"/\\|?*")) != nullptr)
		*wfile++ = L'_';
}

bool FileSystem::IsFileExists(const wchar_t* wfile) {
	DWORD attr = GetFileAttributesW(wfile);
	return attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY);
}

bool FileSystem::IsFileExists(const char* file) {
	wchar_t wfile[1024];
	BufferIO::DecodeUTF8(file, wfile);
	return IsFileExists(wfile);
}

bool FileSystem::IsDirExists(const wchar_t* wdir) {
	DWORD attr = GetFileAttributesW(wdir);
	return attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY);
}

bool FileSystem::IsDirExists(const char* dir) {
	wchar_t wdir[1024];
	BufferIO::DecodeUTF8(dir, wdir);
	return IsDirExists(wdir);
}

bool FileSystem::MakeDir(const wchar_t* wdir) {
	return CreateDirectoryW(wdir, nullptr);
}

bool FileSystem::MakeDir(const char* dir) {
	wchar_t wdir[1024];
	BufferIO::DecodeUTF8(dir, wdir);
	return MakeDir(wdir);
}

bool FileSystem::Rename(const wchar_t* woldname, const wchar_t* wnewname) {
	return MoveFileW(woldname, wnewname);
}

bool FileSystem::Rename(const char* oldname, const char* newname) {
	wchar_t woldname[1024];
	wchar_t wnewname[1024];
	BufferIO::DecodeUTF8(oldname, woldname);
	BufferIO::DecodeUTF8(newname, wnewname);
	return Rename(woldname, wnewname);
}

bool FileSystem::DeleteDir(const wchar_t* wdir) {
	size_t len = std::wcslen(wdir);
	wchar_t pdir[1024 + 1]{};
	if(len >= 1024)
		return false;
	BufferIO::CopyWideString(wdir, pdir);
	// pFrom must be double-null terminated for SHFileOperationW
	// pdir[len] is already '\0' and pdir[len+1] is '\0' due to zero-init
	SHFILEOPSTRUCTW lpFileOp{};
	lpFileOp.hwnd = nullptr;
	lpFileOp.wFunc = FO_DELETE;
	lpFileOp.pFrom = pdir;
	lpFileOp.pTo = nullptr;
	lpFileOp.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
	return SHFileOperationW(&lpFileOp) == 0;
}

bool FileSystem::DeleteDir(const char* dir) {
	wchar_t wdir[1024];
	BufferIO::DecodeUTF8(dir, wdir);
	return DeleteDir(wdir);
}

bool FileSystem::RemoveFile(const wchar_t* wfile) {
	return DeleteFileW(wfile);
}

bool FileSystem::RemoveFile(const char* file) {
	wchar_t wfile[1024];
	BufferIO::DecodeUTF8(file, wfile);
	return RemoveFile(wfile);
}

void FileSystem::TraversalDir(const wchar_t* wpath, const std::function<void(const wchar_t*, bool)>& cb) {
	wchar_t findstr[1024];
	std::swprintf(findstr, sizeof findstr / sizeof findstr[0], L"%ls/*", wpath);
	WIN32_FIND_DATAW fdataw;
	HANDLE fh = FindFirstFileW(findstr, &fdataw);
	if(fh == INVALID_HANDLE_VALUE)
		return;
	do {
		if(std::wcscmp(fdataw.cFileName, L".") && std::wcscmp(fdataw.cFileName, L".."))
			cb(fdataw.cFileName, (fdataw.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY));
	} while(FindNextFileW(fh, &fdataw));
	FindClose(fh);
}

void FileSystem::TraversalDir(const char* path, const std::function<void(const char*, bool)>& cb) {
	wchar_t wpath[1024];
	BufferIO::DecodeUTF8(path, wpath);
	TraversalDir(wpath, [&cb](const wchar_t* wname, bool isdir) {
		char name[1024];
		BufferIO::EncodeUTF8(wname, name);
		cb(name, isdir);
	});
}

#else

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <string>

struct file_unit {
	std::string filename;
	bool is_dir;
};

inline bool TraversalDirSort(file_unit file1, file_unit file2) {
	if(file1.is_dir != file2.is_dir) {
		return file2.is_dir;
	} else {
		return file1.filename < file2.filename;
	}
}

void FileSystem::SafeFileName(wchar_t* wfile) {
	while((wfile = std::wcspbrk(wfile, L"/")) != nullptr)
		*wfile++ = L'_';
}

bool FileSystem::IsFileExists(const char* file) {
	struct stat fileStat;
	return (stat(file, &fileStat) == 0) && !S_ISDIR(fileStat.st_mode);
}

bool FileSystem::IsFileExists(const wchar_t* wfile) {
	char file[1024];
	BufferIO::EncodeUTF8(wfile, file);
	return IsFileExists(file);
}

bool FileSystem::IsDirExists(const char* dir) {
	struct stat fileStat;
	return (stat(dir, &fileStat) == 0) && S_ISDIR(fileStat.st_mode);
}

bool FileSystem::IsDirExists(const wchar_t* wdir) {
	char dir[1024];
	BufferIO::EncodeUTF8(wdir, dir);
	return IsDirExists(dir);
}

bool FileSystem::MakeDir(const char* dir) {
	return mkdir(dir, 0775) == 0;
}

bool FileSystem::MakeDir(const wchar_t* wdir) {
	char dir[1024];
	BufferIO::EncodeUTF8(wdir, dir);
	return MakeDir(dir);
}

bool FileSystem::Rename(const wchar_t* woldname, const wchar_t* wnewname) {
	char oldname[1024];
	char newname[1024];
	BufferIO::EncodeUTF8(woldname, oldname);
	BufferIO::EncodeUTF8(wnewname, newname);
	return Rename(oldname, newname);
}

bool FileSystem::Rename(const char* oldname, const char* newname) {
	return rename(oldname, newname) == 0;
}

bool FileSystem::DeleteDir(const wchar_t* wdir) {
	char dir[1024];
	BufferIO::EncodeUTF8(wdir, dir);
	return DeleteDir(dir);
}

bool FileSystem::DeleteDir(const char* dir) {
	bool success = true;
	TraversalDir(dir, [dir, &success](const char *name, bool isdir) {
		char full_path[1024];
		int len = std::snprintf(full_path, sizeof(full_path), "%s/%s", dir, name);
		if (len < 0 || len >= (int)(sizeof full_path)) {
			success = false;
			return;
		}
		if (isdir)
		{
			if(!DeleteDir(full_path))
				success = false;
		}
		else
		{
			if(unlink(full_path) != 0)
				success = false;
		}
	});
	if (rmdir(dir) != 0)
		success = false;
	return success;
}

bool FileSystem::RemoveFile(const wchar_t* wfile) {
	char file[1024];
	BufferIO::EncodeUTF8(wfile, file);
	return RemoveFile(file);
}

bool FileSystem::RemoveFile(const char* file) {
	return unlink(file) == 0;
}

void FileSystem::TraversalDir(const char* path, const std::function<void(const char*, bool)>& cb) {
	DIR* dir = nullptr;
	struct dirent* dirp = nullptr;
	if((dir = opendir(path)) == nullptr)
		return;
	struct stat fileStat;
	std::vector<file_unit> file_list;
	while((dirp = readdir(dir)) != nullptr) {
		file_unit funit;
		char fname[1024];
		int len = std::snprintf(fname, sizeof(fname), "%s/%s", path, dirp->d_name);
		if (len < 0 || len >= (int)(sizeof fname))
			continue;
		if (stat(fname, &fileStat) != 0)
			continue;
		funit.filename = std::string(dirp->d_name);
		funit.is_dir = S_ISDIR(fileStat.st_mode);
		if(funit.is_dir && (std::strcmp(dirp->d_name, ".") == 0 || std::strcmp(dirp->d_name, "..") == 0))
			continue;
		file_list.push_back(funit);
	}
	closedir(dir);
	std::sort(file_list.begin(), file_list.end(), TraversalDirSort);
	for (file_unit funit : file_list)
		cb(funit.filename.c_str(), funit.is_dir);
}

void FileSystem::TraversalDir(const wchar_t* wpath, const std::function<void(const wchar_t*, bool)>& cb) {
	char path[1024];
	BufferIO::EncodeUTF8(wpath, path);
	TraversalDir(path, [&cb](const char* name, bool isdir) {
		wchar_t wname[1024];
		BufferIO::DecodeUTF8(name, wname);
		cb(wname, isdir);
	});
}

#endif // _WIN32
