/*
 * readonlymemvfs - a basic spatialite vfs for accessing an in-memory read-only database
 *
 * By Alex Paterson, copyright 2012. http://www.tolon.co.uk
 * Based on spmemvfs by Stephen Liu, copyright 2009. http://code.google.com/p/sphivedb/
 *
 * This code and its use is governed by the GNU GPLv2 licence as published by the Free Software Foundation.
 *
 */

#include "ireadfile_sqlite.h"
#include <sqlite3.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <IReadFile.h>
#define READONLY_MEM_VFS_NAME "readonly-mem-vfs"

 //===========================================================================

struct spmemfile_t {
	sqlite3_file base;
	long size;
	irr::io::IReadFile* file;
};

template <typename, typename T, T ret>
struct basefunc;

template <typename T, T ret, typename R, typename ...A>
struct basefunc<R(*)(A...), T, ret> {
private:
	using functype = R(*)(A...);
	static std::conditional_t<std::is_same<void,R>::value,int,R> valueint(A...) {
		return ret;
	}
	static void valuevoid(A...) { }
public:
	static constexpr auto value = !std::is_same<R, void>::value ? (functype)valueint : (functype)valuevoid;
};

using base = sqlite3_io_methods;

#define MAKEDEF(func, ret) basefunc<decltype(sqlite3_io_methods::func), decltype(ret), ret>::value

static int spmemfileRead(sqlite3_file* file, void* buffer, int len, sqlite3_int64 offset) {
	spmemfile_t* memfile = (spmemfile_t*)file;

	if(memfile->file) {
		if((offset + len) > memfile->size) {
			return SQLITE_IOERR_SHORT_READ;
		}
		memfile->file->seek((long)offset);
		memfile->file->read(buffer, len);

		return SQLITE_OK;
	} else {
		return SQLITE_IOERR_SHORT_READ;
	}
}

static int spmemfileFileSize(sqlite3_file* file, sqlite3_int64* size) {
	spmemfile_t* memfile = (spmemfile_t*)file;

	if(memfile->file)
		*size = memfile->size;
	else
		*size = 0;

	return SQLITE_OK;
}

static int spmemfileCheckReservedLock(sqlite3_file* file, int* result) {
	*result = 0;
	return SQLITE_OK;
}

static const sqlite3_io_methods g_spmemfile_io_memthods = {
	1,                                  /* iVersion */
	MAKEDEF(xClose, SQLITE_OK),         /* xClose */
	spmemfileRead,                      /* xRead */
	MAKEDEF(xWrite, SQLITE_READONLY),   /* xWrite */
	MAKEDEF(xTruncate, SQLITE_READONLY),/* xTruncate */
	MAKEDEF(xSync, SQLITE_OK),          /* xSync */
	spmemfileFileSize,                  /* xFileSize */
	MAKEDEF(xLock, SQLITE_OK),          /* xLock */
	MAKEDEF(xUnlock, SQLITE_OK),        /* xUnlock */
	spmemfileCheckReservedLock,         /* xCheckReservedLock */
	MAKEDEF(xFileControl, SQLITE_OK),   /* xFileControl */
	MAKEDEF(xSectorSize, 0),            /* xSectorSize */
	MAKEDEF(xDeviceCharacteristics, 0)  /* xDeviceCharacteristics */
};

#undef MAKEDEF

//===========================================================================

static int spmemvfsOpen(sqlite3_vfs* vfs, const char* path, sqlite3_file* file, int flags, int* outflags) {
	spmemfile_t* memfile = (spmemfile_t*)file;

	memset(memfile, 0, sizeof(spmemfile_t));
	memfile->base.pMethods = &g_spmemfile_io_memthods;

	if(SQLITE_OPEN_MAIN_DB & flags) {
		irr::io::IReadFile** ptr;
		if(sscanf(path, "%p", &ptr) != 1)
			return SQLITE_ERROR;
		memfile->file = *ptr;
		memfile->size = memfile->file->getSize();
	}

	if(SQLITE_OPEN_READONLY & flags)
		return SQLITE_OK;
	else
		return SQLITE_ERROR;
}

static int spmemvfsAccess(sqlite3_vfs* vfs, const char* path, int flags, int* result) {
	*result = 0;
	return SQLITE_OK;
}

static int spmemvfsFullPathname(sqlite3_vfs* vfs, const char* path, int len, char* fullpath) {
	strcpy(fullpath, path);
	return SQLITE_OK;
}

#define MAKEDEF(func, ret) basefunc<decltype(sqlite3_vfs::func), decltype(ret), ret>::value

struct spmemvfs_t {
	sqlite3_vfs base;
	sqlite3_vfs* parent;
};

static spmemvfs_t g_spmemvfs = {
	{
		1,                                           /* iVersion */
		0,                                           /* szOsFile */
		0,                                           /* mxPathname */
		0,                                           /* pNext */
		READONLY_MEM_VFS_NAME,                       /* zName */
		0,                                           /* pAppData */
		spmemvfsOpen,                                /* xOpen */
		MAKEDEF(xDelete,SQLITE_OK),                  /* xDelete */
		spmemvfsAccess,                              /* xAccess */
		spmemvfsFullPathname,                        /* xFullPathname */
		MAKEDEF(xDlOpen,nullptr),                    /* xDlOpen */
		MAKEDEF(xDlError,0),                         /* xDlError */
		MAKEDEF(xDlSym,nullptr),                     /* xDlSym */
		MAKEDEF(xDlClose,0),                         /* xDlClose */
		MAKEDEF(xRandomness,SQLITE_OK),              /* xRandomness */
		MAKEDEF(xSleep,SQLITE_OK),                   /* xSleep */
		MAKEDEF(xCurrentTime,SQLITE_OK)              /* xCurrentTime */
	},
	//{ 0 },
	0                                                /* pParent */
};

//===========================================================================

int readonlymemvfs_init() {
	static const auto initted = [] {
		g_spmemvfs.parent = sqlite3_vfs_find(0);
		g_spmemvfs.base.mxPathname = 0x20;
		g_spmemvfs.base.szOsFile = sizeof(spmemfile_t);
		return sqlite3_vfs_register((sqlite3_vfs*)&g_spmemvfs, 0);
	}();
	return initted;
}

int irrdb_open(irr::io::IReadFile* reader, sqlite3 **ppDb, int flags) {
	char buff[0x20]{};
	snprintf(buff, sizeof(buff), "%p", &reader);
	return sqlite3_open_v2(buff, ppDb, flags, READONLY_MEM_VFS_NAME);
}

//===========================================================================