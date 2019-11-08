/*
 * readonlymemvfs - a basic spatialite vfs for accessing an in-memory read-only database
 *
 * By Alex Paterson, copyright 2012. http://www.tolon.co.uk
 * Based on spmemvfs by Stephen Liu, copyright 2009. http://code.google.com/p/sphivedb/
 *
 * This code and its use is governed by the GNU GPLv2 licence as published by the Free Software Foundation.
 *
 */

#include "readonlymemvfs.h"
#include <sqlite3.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//===========================================================================

static char* s_pData;
static unsigned int s_nDataLen;

void set_mem_db( void* pData, unsigned int len )
{
	s_pData = (char*)pData;
	s_nDataLen = len;
}

//===========================================================================

typedef struct spmemfile_t spmemfile_t;

struct spmemfile_t {
	sqlite3_file base;
	char * path;
	int flags;
	int total;
	int len;
	char * buffer;
};

static int spmemfileClose( sqlite3_file * file );
static int spmemfileRead( sqlite3_file * file, void * buffer, int len, sqlite3_int64 offset );
static int spmemfileWrite( sqlite3_file * file, const void * buffer, int len, sqlite3_int64 offset );
static int spmemfileTruncate( sqlite3_file * file, sqlite3_int64 size );
static int spmemfileSync( sqlite3_file * file, int flags );
static int spmemfileFileSize( sqlite3_file * file, sqlite3_int64 * size );
static int spmemfileLock( sqlite3_file * file, int type );
static int spmemfileUnlock( sqlite3_file * file, int type );
static int spmemfileCheckReservedLock( sqlite3_file * file, int * result );
static int spmemfileFileControl( sqlite3_file * file, int op, void * arg );
static int spmemfileSectorSize( sqlite3_file * file );
static int spmemfileDeviceCharacteristics( sqlite3_file * file );

static sqlite3_io_methods g_spmemfile_io_memthods = {
	1,                                  /* iVersion */
	spmemfileClose,                     /* xClose */
	spmemfileRead,                      /* xRead */
	spmemfileWrite,                     /* xWrite */
	spmemfileTruncate,                  /* xTruncate */
	spmemfileSync,                      /* xSync */
	spmemfileFileSize,                  /* xFileSize */
	spmemfileLock,                      /* xLock */
	spmemfileUnlock,                    /* xUnlock */
	spmemfileCheckReservedLock,         /* xCheckReservedLock */
	spmemfileFileControl,               /* xFileControl */
	spmemfileSectorSize,                /* xSectorSize */
	spmemfileDeviceCharacteristics      /* xDeviceCharacteristics */
};

int spmemfileClose( sqlite3_file * file )
{
	return SQLITE_OK;
}

int spmemfileRead( sqlite3_file * file, void * buffer, int len, sqlite3_int64 offset )
{
	// Read part of the in-memory data, decrypting it in-place if necessary.

	spmemfile_t * memfile = (spmemfile_t*)file;

	if (memfile->buffer == s_pData)
	{
		if( ( offset + len ) > s_nDataLen ) {
			return SQLITE_IOERR_SHORT_READ;
		}

		memcpy( buffer, s_pData + offset, len );

		return SQLITE_OK;
	}
	else
	{
		return SQLITE_IOERR_SHORT_READ;
	}
}

int spmemfileWrite( sqlite3_file * file, const void * buffer, int len, sqlite3_int64 offset )
{
	return SQLITE_READONLY;
}

int spmemfileTruncate( sqlite3_file * file, sqlite3_int64 size )
{
	return SQLITE_READONLY;
}

int spmemfileSync( sqlite3_file * file, int flags )
{
	return SQLITE_OK;
}

int spmemfileFileSize( sqlite3_file * file, sqlite3_int64 * size )
{
	spmemfile_t * memfile = (spmemfile_t*)file;

	if (memfile->buffer == s_pData)
		* size = s_nDataLen;
	else
		* size = 0;

	return SQLITE_OK;
}

int spmemfileLock( sqlite3_file * file, int type )
{
	return SQLITE_OK;
}

int spmemfileUnlock( sqlite3_file * file, int type )
{
	return SQLITE_OK;
}

int spmemfileCheckReservedLock( sqlite3_file * file, int * result )
{
	*result = 0;

	return SQLITE_OK;
}

int spmemfileFileControl( sqlite3_file * file, int op, void * arg )
{
	return SQLITE_OK;
}

int spmemfileSectorSize( sqlite3_file * file )
{
	return 0;
}

int spmemfileDeviceCharacteristics( sqlite3_file * file )
{
	return 0;
}

//===========================================================================

typedef struct spmemvfs_t spmemvfs_t;
struct spmemvfs_t {
	sqlite3_vfs base;
	sqlite3_vfs * parent;
};

static int spmemvfsOpen( sqlite3_vfs * vfs, const char * path, sqlite3_file * file, int flags, int * outflags );
static int spmemvfsDelete( sqlite3_vfs * vfs, const char * path, int syncDir );
static int spmemvfsAccess( sqlite3_vfs * vfs, const char * path, int flags, int * result );
static int spmemvfsFullPathname( sqlite3_vfs * vfs, const char * path, int len, char * fullpath );
static void * spmemvfsDlOpen( sqlite3_vfs * vfs, const char * path );
static void spmemvfsDlError( sqlite3_vfs * vfs, int len, char * errmsg );
static void ( * spmemvfsDlSym ( sqlite3_vfs * vfs, void * handle, const char * symbol ) ) ( void );
static void spmemvfsDlClose( sqlite3_vfs * vfs, void * handle );
static int spmemvfsRandomness( sqlite3_vfs * vfs, int len, char * buffer );
static int spmemvfsSleep( sqlite3_vfs * vfs, int microseconds );
static int spmemvfsCurrentTime( sqlite3_vfs * vfs, double * result );

static spmemvfs_t g_spmemvfs = {
	{
		1,                                           /* iVersion */
		0,                                           /* szOsFile */
		0,                                           /* mxPathname */
		0,                                           /* pNext */
		READONLY_MEM_VFS_NAME,                       /* zName */
		0,                                           /* pAppData */
		spmemvfsOpen,                                /* xOpen */
		spmemvfsDelete,                              /* xDelete */
		spmemvfsAccess,                              /* xAccess */
		spmemvfsFullPathname,                        /* xFullPathname */
		spmemvfsDlOpen,                              /* xDlOpen */
		spmemvfsDlError,                             /* xDlError */
		spmemvfsDlSym,                               /* xDlSym */
		spmemvfsDlClose,                             /* xDlClose */
		spmemvfsRandomness,                          /* xRandomness */
		spmemvfsSleep,                               /* xSleep */
		spmemvfsCurrentTime                          /* xCurrentTime */
	}, 
	//{ 0 },
	0                                                /* pParent */
};

int spmemvfsOpen( sqlite3_vfs * vfs, const char * path, sqlite3_file * file, int flags, int * outflags )
{
	spmemvfs_t * memvfs = (spmemvfs_t*)vfs;
	spmemfile_t * memfile = (spmemfile_t*)file;

	memset( memfile, 0, sizeof( spmemfile_t ) );
	memfile->base.pMethods = &g_spmemfile_io_memthods;
	memfile->flags = flags;

	memfile->path = strdup( path );

	if( SQLITE_OPEN_MAIN_DB & memfile->flags ) {
		memfile->buffer = s_pData;
		memfile->total = s_nDataLen;
	}

	if (SQLITE_OPEN_READONLY & flags)
		return SQLITE_OK;
	else
		return SQLITE_ERROR;
}

int spmemvfsDelete( sqlite3_vfs * vfs, const char * path, int syncDir )
{
	return SQLITE_OK;
}

int spmemvfsAccess( sqlite3_vfs * vfs, const char * path, int flags, int * result )
{
	*result = 0;

	return SQLITE_OK;
}

int spmemvfsFullPathname( sqlite3_vfs * vfs, const char * path, int len, char * fullpath )
{
	strncpy( fullpath, path, len );
	fullpath[ len - 1 ] = '\0';

	return SQLITE_OK;
}

void * spmemvfsDlOpen( sqlite3_vfs * vfs, const char * path )
{
	return NULL;
}

void spmemvfsDlError( sqlite3_vfs * vfs, int len, char * errmsg )
{
	// noop
}

void ( * spmemvfsDlSym ( sqlite3_vfs * vfs, void * handle, const char * symbol ) ) ( void )
{
	return NULL;
}

void spmemvfsDlClose( sqlite3_vfs * vfs, void * handle )
{
	// noop
}

int spmemvfsRandomness( sqlite3_vfs * vfs, int len, char * buffer )
{
	return SQLITE_OK;
}

int spmemvfsSleep( sqlite3_vfs * vfs, int microseconds )
{
	return SQLITE_OK;
}

int spmemvfsCurrentTime( sqlite3_vfs * vfs, double * result )
{
	return SQLITE_OK;
}

//===========================================================================

int readonlymemvfs_init()
{
	sqlite3_vfs * parent = NULL;

	if( g_spmemvfs.parent ) return SQLITE_OK;

	parent = sqlite3_vfs_find( 0 );

	g_spmemvfs.parent = parent;

	g_spmemvfs.base.mxPathname = parent->mxPathname;
	g_spmemvfs.base.szOsFile = sizeof( spmemfile_t );

	return sqlite3_vfs_register( (sqlite3_vfs*)&g_spmemvfs, 0 );
}

//===========================================================================