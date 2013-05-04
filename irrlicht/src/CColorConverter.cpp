// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CColorConverter.h"
#include "SColor.h"
#include "os.h"
#include "irrString.h"

namespace irr
{
namespace video
{

//! converts a monochrome bitmap to A1R5G5B5 data
void CColorConverter::convert1BitTo16Bit(const u8* in, s16* out, s32 width, s32 height, s32 linepad, bool flip)
{
	if (!in || !out)
		return;

	if (flip)
		out += width * height;

	for (s32 y=0; y<height; ++y)
	{
		s32 shift = 7;
		if (flip)
			out -= width;

		for (s32 x=0; x<width; ++x)
		{
			out[x] = *in>>shift & 0x01 ? (s16)0xffff : (s16)0x8000;

			if ((--shift)<0) // 8 pixel done
			{
				shift=7;
				++in;
			}
		}

		if (shift != 7) // width did not fill last byte
			++in;

		if (!flip)
			out += width;
		in += linepad;
	}
}



//! converts a 4 bit palettized image to A1R5G5B5
void CColorConverter::convert4BitTo16Bit(const u8* in, s16* out, s32 width, s32 height, const s32* palette, s32 linepad, bool flip)
{
	if (!in || !out || !palette)
		return;

	if (flip)
		out += width*height;

	for (s32 y=0; y<height; ++y)
	{
		s32 shift = 4;
		if (flip)
			out -= width;

		for (s32 x=0; x<width; ++x)
		{
			out[x] = X8R8G8B8toA1R5G5B5(palette[(u8)((*in >> shift) & 0xf)]);

			if (shift==0)
			{
				shift = 4;
				++in;
			}
			else
				shift = 0;
		}

		if (shift == 0) // odd width
			++in;

		if (!flip)
			out += width;
		in += linepad;
	}
}



//! converts a 8 bit palettized image into A1R5G5B5
void CColorConverter::convert8BitTo16Bit(const u8* in, s16* out, s32 width, s32 height, const s32* palette, s32 linepad, bool flip)
{
	if (!in || !out || !palette)
		return;

	if (flip)
		out += width * height;

	for (s32 y=0; y<height; ++y)
	{
		if (flip)
			out -= width; // one line back
		for (s32 x=0; x<width; ++x)
		{
			out[x] = X8R8G8B8toA1R5G5B5(palette[(u8)(*in)]);
			++in;
		}
		if (!flip)
			out += width;
		in += linepad;
	}
}

//! converts a 8 bit palettized or non palettized image (A8) into R8G8B8
void CColorConverter::convert8BitTo24Bit(const u8* in, u8* out, s32 width, s32 height, const u8* palette, s32 linepad, bool flip)
{
	if (!in || !out )
		return;

	const s32 lineWidth = 3 * width;
	if (flip)
		out += lineWidth * height;

	for (s32 y=0; y<height; ++y)
	{
		if (flip)
			out -= lineWidth; // one line back
		for (s32 x=0; x< lineWidth; x += 3)
		{
			if ( palette )
			{
#ifdef __BIG_ENDIAN__
				out[x+0] = palette[ (in[0] << 2 ) + 0];
				out[x+1] = palette[ (in[0] << 2 ) + 1];
				out[x+2] = palette[ (in[0] << 2 ) + 2];
#else
				out[x+0] = palette[ (in[0] << 2 ) + 2];
				out[x+1] = palette[ (in[0] << 2 ) + 1];
				out[x+2] = palette[ (in[0] << 2 ) + 0];
#endif
			}
			else
			{
				out[x+0] = in[0];
				out[x+1] = in[0];
				out[x+2] = in[0];
			}
			++in;
		}
		if (!flip)
			out += lineWidth;
		in += linepad;
	}
}

//! converts a 8 bit palettized or non palettized image (A8) into R8G8B8
void CColorConverter::convert8BitTo32Bit(const u8* in, u8* out, s32 width, s32 height, const u8* palette, s32 linepad, bool flip)
{
	if (!in || !out )
		return;

	const u32 lineWidth = 4 * width;
	if (flip)
		out += lineWidth * height;

	u32 x;
	register u32 c;
	for (u32 y=0; y < (u32) height; ++y)
	{
		if (flip)
			out -= lineWidth; // one line back

		if ( palette )
		{
			for (x=0; x < (u32) width; x += 1)
			{
				c = in[x];
				((u32*)out)[x] = ((u32*)palette)[ c ];
			}
		}
		else
		{
			for (x=0; x < (u32) width; x += 1)
			{
				c = in[x];
#ifdef __BIG_ENDIAN__
				((u32*)out)[x] = c << 24 | c << 16 | c << 8 | 0x000000FF;
#else
				((u32*)out)[x] = 0xFF000000 | c << 16 | c << 8 | c;
#endif
			}

		}

		if (!flip)
			out += lineWidth;
		in += width + linepad;
	}
}



//! converts 16bit data to 16bit data
void CColorConverter::convert16BitTo16Bit(const s16* in, s16* out, s32 width, s32 height, s32 linepad, bool flip)
{
	if (!in || !out)
		return;

	if (flip)
		out += width * height;

	for (s32 y=0; y<height; ++y)
	{
		if (flip)
			out -= width;
#ifdef __BIG_ENDIAN__
		for (s32 x=0; x<width; ++x)
			out[x]=os::Byteswap::byteswap(in[x]);
#else
		memcpy(out, in, width*sizeof(s16));
#endif
		if (!flip)
			out += width;
		in += width;
		in += linepad;
	}
}



//! copies R8G8B8 24bit data to 24bit data
void CColorConverter::convert24BitTo24Bit(const u8* in, u8* out, s32 width, s32 height, s32 linepad, bool flip, bool bgr)
{
	if (!in || !out)
		return;

	const s32 lineWidth = 3 * width;
	if (flip)
		out += lineWidth * height;

	for (s32 y=0; y<height; ++y)
	{
		if (flip)
			out -= lineWidth;
		if (bgr)
		{
			for (s32 x=0; x<lineWidth; x+=3)
			{
				out[x+0] = in[x+2];
				out[x+1] = in[x+1];
				out[x+2] = in[x+0];
			}
		}
		else
		{
			memcpy(out,in,lineWidth);
		}
		if (!flip)
			out += lineWidth;
		in += lineWidth;
		in += linepad;
	}
}



//! Resizes the surface to a new size and converts it at the same time
//! to an A8R8G8B8 format, returning the pointer to the new buffer.
void CColorConverter::convert16bitToA8R8G8B8andResize(const s16* in, s32* out, s32 newWidth, s32 newHeight, s32 currentWidth, s32 currentHeight)
{
	if (!newWidth || !newHeight)
		return;

	// note: this is very very slow. (i didn't want to write a fast version.
	// but hopefully, nobody wants to convert surfaces every frame.	

	f32 sourceXStep = (f32)currentWidth / (f32)newWidth;
	f32 sourceYStep = (f32)currentHeight / (f32)newHeight;
	f32 sy;
	s32 t;

	for (s32 x=0; x<newWidth; ++x)
	{
		sy = 0.0f;

		for (s32 y=0; y<newHeight; ++y)
		{
			t = in[(s32)(((s32)sy)*currentWidth + x*sourceXStep)];
			t = (((t >> 15)&0x1)<<31) |	(((t >> 10)&0x1F)<<19) |
				(((t >> 5)&0x1F)<<11) |	(t&0x1F)<<3;
			out[(s32)(y*newWidth + x)] = t;

			sy+=sourceYStep;
		}
	}
}



//! copies X8R8G8B8 32 bit data
void CColorConverter::convert32BitTo32Bit(const s32* in, s32* out, s32 width, s32 height, s32 linepad, bool flip)
{
	if (!in || !out)
		return;

	if (flip)
		out += width * height;

	for (s32 y=0; y<height; ++y)
	{
		if (flip)
			out -= width;
#ifdef __BIG_ENDIAN__
		for (s32 x=0; x<width; ++x)
			out[x]=os::Byteswap::byteswap(in[x]);
#else
		memcpy(out, in, width*sizeof(s32));
#endif
		if (!flip)
			out += width;
		in += width;
		in += linepad;
	}
}



void CColorConverter::convert_A1R5G5B5toR8G8B8(const void* sP, s32 sN, void* dP)
{
	u16* sB = (u16*)sP;
	u8 * dB = (u8 *)dP;

	for (s32 x = 0; x < sN; ++x)
	{
		dB[2] = (*sB & 0x7c00) >> 7;
		dB[1] = (*sB & 0x03e0) >> 2;
		dB[0] = (*sB & 0x1f) << 3;

		sB += 1;
		dB += 3;
	}
}

void CColorConverter::convert_A1R5G5B5toB8G8R8(const void* sP, s32 sN, void* dP)
{
	u16* sB = (u16*)sP;
	u8 * dB = (u8 *)dP;

	for (s32 x = 0; x < sN; ++x)
	{
		dB[0] = (*sB & 0x7c00) >> 7;
		dB[1] = (*sB & 0x03e0) >> 2;
		dB[2] = (*sB & 0x1f) << 3;

		sB += 1;
		dB += 3;
	}
}

void CColorConverter::convert_A1R5G5B5toA8R8G8B8(const void* sP, s32 sN, void* dP)
{
	u16* sB = (u16*)sP;
	u32* dB = (u32*)dP;

	for (s32 x = 0; x < sN; ++x)
		*dB++ = A1R5G5B5toA8R8G8B8(*sB++);
}

void CColorConverter::convert_A1R5G5B5toA1R5G5B5(const void* sP, s32 sN, void* dP)
{
	memcpy(dP, sP, sN * 2);
}

void CColorConverter::convert_A1R5G5B5toR5G6B5(const void* sP, s32 sN, void* dP)
{
	u16* sB = (u16*)sP;
	u16* dB = (u16*)dP;

	for (s32 x = 0; x < sN; ++x)
		*dB++ = A1R5G5B5toR5G6B5(*sB++);
}

void CColorConverter::convert_A8R8G8B8toR8G8B8(const void* sP, s32 sN, void* dP)
{
	u8* sB = (u8*)sP;
	u8* dB = (u8*)dP;

	for (s32 x = 0; x < sN; ++x)
	{
		// sB[3] is alpha
		dB[0] = sB[2];
		dB[1] = sB[1];
		dB[2] = sB[0];

		sB += 4;
		dB += 3;
	}
}

void CColorConverter::convert_A8R8G8B8toB8G8R8(const void* sP, s32 sN, void* dP)
{
	u8* sB = (u8*)sP;
	u8* dB = (u8*)dP;

	for (s32 x = 0; x < sN; ++x)
	{
		// sB[3] is alpha
		dB[0] = sB[0];
		dB[1] = sB[1];
		dB[2] = sB[2];

		sB += 4;
		dB += 3;
	}
}

void CColorConverter::convert_A8R8G8B8toA8R8G8B8(const void* sP, s32 sN, void* dP)
{
	memcpy(dP, sP, sN * 4);
}

void CColorConverter::convert_A8R8G8B8toA1R5G5B5(const void* sP, s32 sN, void* dP)
{
	u32* sB = (u32*)sP;
	u16* dB = (u16*)dP;

	for (s32 x = 0; x < sN; ++x)
		*dB++ = A8R8G8B8toA1R5G5B5(*sB++);
}

void CColorConverter::convert_A8R8G8B8toR5G6B5(const void* sP, s32 sN, void* dP)
{
	u8 * sB = (u8 *)sP;
	u16* dB = (u16*)dP;

	for (s32 x = 0; x < sN; ++x)
	{
		s32 r = sB[2] >> 3;
		s32 g = sB[1] >> 2;
		s32 b = sB[0] >> 3;

		dB[0] = (r << 11) | (g << 5) | (b);

		sB += 4;
		dB += 1;
	}
}

void CColorConverter::convert_A8R8G8B8toR3G3B2(const void* sP, s32 sN, void* dP)
{
	u8* sB = (u8*)sP;
	u8* dB = (u8*)dP;

	for (s32 x = 0; x < sN; ++x)
	{
		u8 r = sB[2] & 0xe0;
		u8 g = (sB[1] & 0xe0) >> 3;
		u8 b = (sB[0] & 0xc0) >> 6;

		dB[0] = (r | g | b);

		sB += 4;
		dB += 1;
	}
}

void CColorConverter::convert_R8G8B8toR8G8B8(const void* sP, s32 sN, void* dP)
{
	memcpy(dP, sP, sN * 3);
}

void CColorConverter::convert_R8G8B8toA8R8G8B8(const void* sP, s32 sN, void* dP)
{
	u8*  sB = (u8* )sP;
	u32* dB = (u32*)dP;

	for (s32 x = 0; x < sN; ++x)
	{
		*dB = 0xff000000 | (sB[0]<<16) | (sB[1]<<8) | sB[2];

		sB += 3;
		++dB;
	}
}

void CColorConverter::convert_R8G8B8toA1R5G5B5(const void* sP, s32 sN, void* dP)
{
	u8 * sB = (u8 *)sP;
	u16* dB = (u16*)dP;

	for (s32 x = 0; x < sN; ++x)
	{
		s32 r = sB[0] >> 3;
		s32 g = sB[1] >> 3;
		s32 b = sB[2] >> 3;

		dB[0] = (0x8000) | (r << 10) | (g << 5) | (b);

		sB += 3;
		dB += 1;
	}
}

void CColorConverter::convert_B8G8R8toA8R8G8B8(const void* sP, s32 sN, void* dP)
{
	u8*  sB = (u8* )sP;
	u32* dB = (u32*)dP;

	for (s32 x = 0; x < sN; ++x)
	{
		*dB = 0xff000000 | (sB[2]<<16) | (sB[1]<<8) | sB[0];

		sB += 3;
		++dB;
	}
}

void CColorConverter::convert_B8G8R8A8toA8R8G8B8(const void* sP, s32 sN, void* dP)
{
	u8* sB = (u8*)sP;
	u8* dB = (u8*)dP;

	for (s32 x = 0; x < sN; ++x)
	{
		dB[0] = sB[3];
		dB[1] = sB[2];
		dB[2] = sB[1];
		dB[3] = sB[0];

		sB += 4;
		dB += 4;
	}

}

void CColorConverter::convert_R8G8B8toR5G6B5(const void* sP, s32 sN, void* dP)
{
	u8 * sB = (u8 *)sP;
	u16* dB = (u16*)dP;

	for (s32 x = 0; x < sN; ++x)
	{
		s32 r = sB[0] >> 3;
		s32 g = sB[1] >> 2;
		s32 b = sB[2] >> 3;

		dB[0] = (r << 11) | (g << 5) | (b);

		sB += 3;
		dB += 1;
	}
}

void CColorConverter::convert_R5G6B5toR5G6B5(const void* sP, s32 sN, void* dP)
{
	memcpy(dP, sP, sN * 2);
}

void CColorConverter::convert_R5G6B5toR8G8B8(const void* sP, s32 sN, void* dP)
{
	u16* sB = (u16*)sP;
	u8 * dB = (u8 *)dP;

	for (s32 x = 0; x < sN; ++x)
	{
		dB[0] = (*sB & 0xf800) >> 8;
		dB[1] = (*sB & 0x07e0) >> 3;
		dB[2] = (*sB & 0x001f) << 3;

		sB += 1;
		dB += 3;
	}
}

void CColorConverter::convert_R5G6B5toB8G8R8(const void* sP, s32 sN, void* dP)
{
	u16* sB = (u16*)sP;
	u8 * dB = (u8 *)dP;

	for (s32 x = 0; x < sN; ++x)
	{
		dB[2] = (*sB & 0xf800) >> 8;
		dB[1] = (*sB & 0x07e0) >> 3;
		dB[0] = (*sB & 0x001f) << 3;

		sB += 1;
		dB += 3;
	}
}

void CColorConverter::convert_R5G6B5toA8R8G8B8(const void* sP, s32 sN, void* dP)
{
	u16* sB = (u16*)sP;
	u32* dB = (u32*)dP;

	for (s32 x = 0; x < sN; ++x)
		*dB++ = R5G6B5toA8R8G8B8(*sB++);
}

void CColorConverter::convert_R5G6B5toA1R5G5B5(const void* sP, s32 sN, void* dP)
{
	u16* sB = (u16*)sP;
	u16* dB = (u16*)dP;

	for (s32 x = 0; x < sN; ++x)
		*dB++ = R5G6B5toA1R5G5B5(*sB++);
}


void CColorConverter::convert_viaFormat(const void* sP, ECOLOR_FORMAT sF, s32 sN,
				void* dP, ECOLOR_FORMAT dF)
{
	switch (sF)
	{
		case ECF_A1R5G5B5:
			switch (dF)
			{
				case ECF_A1R5G5B5:
					convert_A1R5G5B5toA1R5G5B5(sP, sN, dP);
				break;
				case ECF_R5G6B5:
					convert_A1R5G5B5toR5G6B5(sP, sN, dP);
				break;
				case ECF_A8R8G8B8:
					convert_A1R5G5B5toA8R8G8B8(sP, sN, dP);
				break;
				case ECF_R8G8B8:
					convert_A1R5G5B5toR8G8B8(sP, sN, dP);
				break;
#ifndef _DEBUG
				default:
					break;
#endif
			}
		break;
		case ECF_R5G6B5:
			switch (dF)
			{
				case ECF_A1R5G5B5:
					convert_R5G6B5toA1R5G5B5(sP, sN, dP);
				break;
				case ECF_R5G6B5:
					convert_R5G6B5toR5G6B5(sP, sN, dP);
				break;
				case ECF_A8R8G8B8:
					convert_R5G6B5toA8R8G8B8(sP, sN, dP);
				break;
				case ECF_R8G8B8:
					convert_R5G6B5toR8G8B8(sP, sN, dP);
				break;
#ifndef _DEBUG
				default:
					break;
#endif
			}
		break;
		case ECF_A8R8G8B8:
			switch (dF)
			{
				case ECF_A1R5G5B5:
					convert_A8R8G8B8toA1R5G5B5(sP, sN, dP);
				break;
				case ECF_R5G6B5:
					convert_A8R8G8B8toR5G6B5(sP, sN, dP);
				break;
				case ECF_A8R8G8B8:
					convert_A8R8G8B8toA8R8G8B8(sP, sN, dP);
				break;
				case ECF_R8G8B8:
					convert_A8R8G8B8toR8G8B8(sP, sN, dP);
				break;
#ifndef _DEBUG
				default:
					break;
#endif
			}
		break;
		case ECF_R8G8B8:
			switch (dF)
			{
				case ECF_A1R5G5B5:
					convert_R8G8B8toA1R5G5B5(sP, sN, dP);
				break;
				case ECF_R5G6B5:
					convert_R8G8B8toR5G6B5(sP, sN, dP);
				break;
				case ECF_A8R8G8B8:
					convert_R8G8B8toA8R8G8B8(sP, sN, dP);
				break;
				case ECF_R8G8B8:
					convert_R8G8B8toR8G8B8(sP, sN, dP);
				break;
#ifndef _DEBUG
				default:
					break;
#endif
			}
		break;
	}
}


} // end namespace video
} // end namespace irr
