// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CReadFile.h"

namespace irr
{
namespace io
{


CReadFile::CReadFile(const io::path& fileName)
: File(0), FileSize(0), Filename(fileName)
{
	#ifdef _DEBUG
	setDebugName("CReadFile");
	#endif

	openFile();
}


CReadFile::~CReadFile()
{
	if (File)
		fclose(File);
}


//! returns how much was read
s32 CReadFile::read(void* buffer, u32 sizeToRead)
{
	if (!isOpen())
		return 0;

	return (s32)fread(buffer, 1, sizeToRead, File);
}


//! changes position in file, returns true if successful
//! if relativeMovement==true, the pos is changed relative to current pos,
//! otherwise from begin of file
bool CReadFile::seek(long finalPos, bool relativeMovement)
{
	if (!isOpen())
		return false;

	return fseek(File, finalPos, relativeMovement ? SEEK_CUR : SEEK_SET) == 0;
}


//! returns size of file
long CReadFile::getSize() const
{
	return FileSize;
}


//! returns where in the file we are.
long CReadFile::getPos() const
{
	return ftell(File);
}


//! opens the file
void CReadFile::openFile()
{
	if (Filename.size() == 0) // bugfix posted by rt
	{
		File = 0;
		return;
	}

#if defined ( _IRR_WCHAR_FILESYSTEM )
	File = _wfopen(Filename.c_str(), L"rb");
#else
	File = fopen(Filename.c_str(), "rb");
#endif

	if (File)
	{
		// get FileSize

		fseek(File, 0, SEEK_END);
		FileSize = getPos();
		fseek(File, 0, SEEK_SET);
	}
}


//! returns name of file
const io::path& CReadFile::getFileName() const
{
	return Filename;
}



IReadFile* createReadFile(const io::path& fileName)
{
	CReadFile* file = new CReadFile(fileName);
	if (file->isOpen())
		return file;

	file->drop();
	return 0;
}


} // end namespace io
} // end namespace irr

