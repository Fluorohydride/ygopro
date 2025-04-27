/*
   CGUITTFont FreeType class for Irrlicht
   Copyright (c) 2009-2010 John Norman

   This software is provided 'as-is', without any express or implied
   warranty. In no event will the authors be held liable for any
   damages arising from the use of this software.

   Permission is granted to anyone to use this software for any
   purpose, including commercial applications, and to alter it and
   redistribute it freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you
      must not claim that you wrote the original software. If you use
      this software in a product, an acknowledgment in the product
      documentation would be appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and
      must not be misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
      distribution.

   The original version of this class can be located at:
   http://irrlicht.suckerfreegames.com/

   John Norman
   john@suckerfreegames.com
*/

#include "CGUITTFont.h"

namespace irr
{
namespace gui
{

// From example2.cpp from freetype tutorials from here: https://www.freetype.org/freetype2/docs/tutorial/step3.html (code in public domain)
//
// Each time the renderer calls us back we just push another span entry on our list.
static void RasterCallback(const int y, const int count, const FT_Span * const spans, void * const user)
{
	irr::core::array<SGlyphPixelSpan> *sptr = (irr::core::array<SGlyphPixelSpan> *)user;
	for (int i = 0; i < count; ++i)

	sptr->push_back(SGlyphPixelSpan(spans[i].x, y, spans[i].len, spans[i].coverage));
}

// From example2.cpp from freetype tutorials from here: https://www.freetype.org/freetype2/docs/tutorial/step3.html (code in public domain)
//
// Set up the raster parameters and render the outline.
void RenderSpans(FT_Library &library, FT_Outline * const outline, irr::core::array<SGlyphPixelSpan> *spans)
{
	FT_Raster_Params params;
	memset(&params, 0, sizeof(params));
	params.flags = FT_RASTER_FLAG_AA | FT_RASTER_FLAG_DIRECT;
	params.gray_spans = RasterCallback;
	params.user = spans;

	FT_Outline_Render(library, outline, &params);
}



// --- SGUITTFace ---
// Manages the FT_Face cache.
struct SGUITTFace : public virtual irr::IReferenceCounted
{
	SGUITTFace() : FaceBuffer(0), FaceBufferSize(0)
	{
		memset((void*)&Face, 0, sizeof(FT_Face));
	}

	~SGUITTFace()
	{
		FT_Done_Face(Face);
		delete[] FaceBuffer;
	}

	FT_Face Face;
	FT_Byte* FaceBuffer;
	FT_Long FaceBufferSize;
};

// --- Static variables ---
FT_Library CGUITTFont::c_library;
core::map<io::path, SGUITTFace*> CGUITTFont::c_faces;
bool CGUITTFont::c_libraryLoaded = false;


// --- SGUITTGlyph ---

video::IImage* SGUITTGlyph::createGlyphImageFromBitmap(const irr::video::ECOLOR_FORMAT colorFormat, const FT_Bitmap& bits, video::IVideoDriver* driver) const
{
	// Determine what our texture size should be.
	// Add 1 because textures are inclusive-exclusive.
	core::dimension2du d(bits.width + 1, bits.rows + 1);
	core::dimension2du texture_size;
	//core::dimension2du texture_size(bits.width + 1, bits.rows + 1);

	// Create and load our image now.
	video::IImage* image = 0;
	switch (bits.pixel_mode)
	{
		case FT_PIXEL_MODE_MONO:
		{
			// Create a blank image and fill it with transparent pixels.
			texture_size = d.getOptimalSize(true, true);
			image = driver->createImage(colorFormat, texture_size);
			image->fill(video::SColor(0, 255, 255, 255));

			// Load the monochrome data in.
			const u32 image_pitch = image->getPitch() / sizeof(u16);
#if (IRRLICHT_VERSION_MAJOR > 1) || (IRRLICHT_VERSION_MAJOR == 1 && IRRLICHT_VERSION_MINOR >= 9)
			u16* image_data = (u16*)image->getData();
#else
			u16* image_data = (u16*)image->lock();
#endif
			u8* glyph_data = bits.buffer;
			for (unsigned int y = 0; y < bits.rows; ++y)
			{
				u16* row = image_data;
				for (unsigned int x = 0; x < bits.width; ++x)
				{
					// Monochrome bitmaps store 8 pixels per byte.  The left-most pixel is the bit 0x80.
					// So, we go through the data each bit at a time.
					if ((glyph_data[y * bits.pitch + (x / 8)] & (0x80 >> (x % 8))) != 0)
						*row = 0xFFFF;
					++row;
				}
				image_data += image_pitch;
			}
			break;
		}

		case FT_PIXEL_MODE_GRAY:
		{
			// Create our blank image.
			texture_size = d.getOptimalSize(!driver->queryFeature(video::EVDF_TEXTURE_NPOT), !driver->queryFeature(video::EVDF_TEXTURE_NSQUARE));
			image = driver->createImage(colorFormat, texture_size);
			image->fill(video::SColor(0, 255, 255, 255));

			// Load the grayscale data in.
			const float gray_count = static_cast<float>(bits.num_grays);
			const u32 image_pitch = image->getPitch() / sizeof(u32);
#if (IRRLICHT_VERSION_MAJOR > 1) || (IRRLICHT_VERSION_MAJOR == 1 && IRRLICHT_VERSION_MINOR >= 9)
			u32* image_data = (u32*)image->getData();
#else
			u32* image_data = (u32*)image->lock();
#endif
			u8* glyph_data = bits.buffer;
			for (unsigned int  y = 0; y < bits.rows; ++y)
			{
				u8* row = glyph_data;
				for (unsigned int  x = 0; x < bits.width; ++x)
				{
					image_data[y * image_pitch + x] |= static_cast<u32>(255.0f * (static_cast<float>(*row++) / gray_count)) << 24;
					//data[y * image_pitch + x] |= ((u32)(*bitsdata++) << 24);
				}
				glyph_data += bits.pitch;
			}
			break;
		}
		default:
			break;
	}
	return image;
}

video::IImage* SGUITTGlyph::createGlyphImageFromPixelSpans(const irr::video::ECOLOR_FORMAT colorFormat, video::IVideoDriver* driver, int width, int height) const
{
	// TODO: currently different colorFormat's are not supported, so when font is FT_PIXEL_MODE_MONO there will probably be trouble

	video::IImage* image = 0;

	if ( !PixelOutlineSpans.empty() )
	{
		// Create our blank image.
		core::dimension2du d(width + 1, height + 1);
		core::dimension2du texture_size = d.getOptimalSize(!driver->queryFeature(video::EVDF_TEXTURE_NPOT), !driver->queryFeature(video::EVDF_TEXTURE_NSQUARE), true, 0);
		image = driver->createImage(colorFormat, texture_size);
		image->fill(video::SColor(0, 255, 255, 255));

		// Load the grayscale data in.
		const u32 image_pitch = image->getPitch() / sizeof(u32);
#if (IRRLICHT_VERSION_MAJOR > 1) || (IRRLICHT_VERSION_MAJOR == 1 && IRRLICHT_VERSION_MINOR >= 9)
		u32* image_data = (u32*)image->getData();
#else
		u32* image_data = (u32*)image->lock();
#endif
		// draw outlines
		for (irr::u32 s = 0; s < PixelOutlineSpans.size(); ++s )
		{
			const SGlyphPixelSpan& ps = PixelOutlineSpans[s];
			for (int w = 0; w < ps.Width; ++w)
			{
				image_data[ps.Y * image_pitch + ps.X + w] = irr::video::SColor(ps.Coverage, 255, 255, 255).color;
			}
		}
	}

	return image;
}

// Based on example2.cpp from freetype tutorials from here: https://www.freetype.org/freetype2/docs/tutorial/step3.html (code in public domain)
bool SGUITTGlyph::renderSpans(FT_Library &library, FT_Face &face, float outline)
{
    // Need an outline for this to work.
    if (face->glyph->format == FT_GLYPH_FORMAT_OUTLINE)
    {
		// Render the basic glyph to a span list. (we don't need those for now as we use bitmaps for non-outlined glyphs currently)
		// RenderSpans(library, &face->glyph->outline, &PixelSpans);

		// Next the outline.
		// Set up a stroker.
		FT_Stroker stroker;
		FT_Stroker_New(library, &stroker);
		FT_Stroker_Set(stroker,
						(int)(outline*64.f),
						FT_STROKER_LINECAP_ROUND,
						FT_STROKER_LINEJOIN_ROUND,
						0);

		FT_Glyph glyph;
		if (FT_Get_Glyph(face->glyph, &glyph) == 0)
		{
			FT_Glyph_StrokeBorder(&glyph, stroker, 0, 1);
			// Again, this needs to be an outline to work.
			if (glyph->format == FT_GLYPH_FORMAT_OUTLINE)
			{
				// Render the outline spans to the span list
				FT_Outline *o =	&reinterpret_cast<FT_OutlineGlyph>(glyph)->outline;
				RenderSpans(library, o, &PixelOutlineSpans);
			}

			// Clean up afterwards.
			FT_Stroker_Done(stroker);
			FT_Done_Glyph(glyph);
			return true;
		}
	}
	return false;
}

void SGUITTGlyph::preload(CGUITTFont& font, u32 char_index, FT_Face face, u32 fontSize, float outline, FT_Int32 loadFlags)
{
	if (IsLoaded())
		return;

	// Set the size of the glyph.
	FT_Set_Pixel_Sizes(face, 0, fontSize);

	// Attempt to load the glyph.
	if (FT_Load_Glyph(face, char_index, loadFlags) != FT_Err_Ok)
	{
		// TODO: error message?
		return;
	}

	FT_GlyphSlot glyph = face->glyph;
	FT_Bitmap bits = glyph->bitmap;
	irr::video::ECOLOR_FORMAT colorFormat = bits.pixel_mode == FT_PIXEL_MODE_MONO ? video::ECF_A1R5G5B5 : video::ECF_A8R8G8B8;

	TopLayer.Offset = core::vector2di(glyph->bitmap_left, glyph->bitmap_top);
	TopLayer.Advance = glyph->advance;


	// reserve a spot on the glyph page
	TopLayer.GlyphPage = font.getGlyphPageSpot(TopLayer.SourceRect, colorFormat, bits.width, bits.rows);
	if ( TopLayer.GlyphPage )
	{
		// Create image with glyph
		irr::video::IImage* surface = createGlyphImageFromBitmap(colorFormat, bits, font.Driver);

		// Request paging
		TopLayer.GlyphPage->pushGlyphLayerToBePaged(surface, TopLayer.SourceRect);
		surface->drop();
	}


	// Outlines don't work with bitmaps bug with pixel-strips. We render those into their own Surface (as it can have another color later on)
	if ( outline != 0.f )
	{
		// Not possible to have outlines with bitmaps. Also not possible to load bitmaps and outlines the same time.
		loadFlags  = FT_LOAD_NO_BITMAP; // TODO: check if other flags can be used

		// Attempt to load the glyph.
		if (FT_Load_Glyph(face, char_index, loadFlags) != FT_Err_Ok)
		{
			// TODO: error message?
			return;
		}

		glyph = face->glyph;

		colorFormat = video::ECF_A8R8G8B8;
		int width = 0;
		int height = 0;

		OutlineLayer.Advance = glyph->advance;
		OutlineLayer.Offset = core::vector2di(0, 0);

		renderSpans(font.c_library, face, outline);

		if ( !PixelOutlineSpans.empty())
		{
			// get size needed for glyph
			irr::core::recti r( PixelOutlineSpans[0].X, PixelOutlineSpans[0].Y, PixelOutlineSpans[0].X+PixelOutlineSpans[0].Width, PixelOutlineSpans[0].Y+1 );
			for ( irr::u32 s=1; s < PixelOutlineSpans.size(); ++s )
			{
				r.addInternalPoint(PixelOutlineSpans[s].X, PixelOutlineSpans[s].Y);
				r.addInternalPoint(PixelOutlineSpans[s].X+PixelOutlineSpans[s].Width, PixelOutlineSpans[s].Y+1);
			}
			width = r.getWidth();
			height = r.getHeight();

			// We let the glyph coordinates start at 0,0 and add an Offset instead
			OutlineLayer.Offset = core::vector2di(r.UpperLeftCorner.X, r.LowerRightCorner.Y); // x=0 or r.UpperLeftCorner.X? Not sure yet

			for ( irr::u32 s=0; s < PixelOutlineSpans.size(); ++s )
			{
				PixelOutlineSpans[s].X -= r.UpperLeftCorner.X;
				PixelOutlineSpans[s].Y -= r.UpperLeftCorner.Y;
				PixelOutlineSpans[s].Y = height - 1 - PixelOutlineSpans[s].Y;
			}

			// reserve a spot on the glyph page
			OutlineLayer.GlyphPage = font.getGlyphPageSpot(OutlineLayer.SourceRect, colorFormat, width, height);
			if (OutlineLayer.GlyphPage)
			{
				// We grab the glyph bitmap here so the data won't be removed when the next glyph is loaded.
				irr::video::IImage* outlineSurface = createGlyphImageFromPixelSpans(colorFormat, font.Driver, width, height);

				// Request paging
				OutlineLayer.GlyphPage->pushGlyphLayerToBePaged(outlineSurface, OutlineLayer.SourceRect);
				outlineSurface->drop();
			}
		}
	}
}

void SGUITTGlyph::unload()
{
	TopLayer.clear();
	OutlineLayer.clear();
	PixelSpans.clear();
	PixelOutlineSpans.clear();
}


// --- CGUITTGlyphPage ---

bool CGUITTGlyphPage::createPageTexture(const irr::video::ECOLOR_FORMAT colorFormat, const core::dimension2du& textureSize)
{
	if ( Texture )
		return false;

	bool flgmip = Driver->getTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS);
	Driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);

#if (IRRLICHT_VERSION_MAJOR > 1) || (IRRLICHT_VERSION_MAJOR == 1 && IRRLICHT_VERSION_MINOR >= 9)
	bool allowMemCpy = Driver->getTextureCreationFlag(video::ETCF_ALLOW_MEMORY_COPY);
	Driver->setTextureCreationFlag(video::ETCF_ALLOW_MEMORY_COPY, true);
#endif

	// Set the texture color format.
	Texture = Driver->addTexture(textureSize, Name, colorFormat);
	if (Texture)
		Texture->grab();

	// Restore texture creation flags.
#if (IRRLICHT_VERSION_MAJOR > 1) || (IRRLICHT_VERSION_MAJOR == 1 && IRRLICHT_VERSION_MINOR >= 9)
	Driver->setTextureCreationFlag(video::ETCF_ALLOW_MEMORY_COPY, allowMemCpy);
#endif
	Driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, flgmip);

	return Texture ? true : false;
}

void CGUITTGlyphPage::updateTexture()
{
	if (GlyphLayersToBePaged.empty())
		return;

	if ( Texture )
	{
		void* ptr = Texture->lock();
		video::ECOLOR_FORMAT format = Texture->getColorFormat();
		core::dimension2du size = Texture->getSize();

		video::IImage* pageholder = Driver->createImageFromData(format, size, ptr, true, false);

		for (u32 i = 0; i < GlyphLayersToBePaged.size(); ++i)
		{
			PagedGlyphTextures& pgt = GlyphLayersToBePaged[i];
			if (pgt.Surface)
			{
				pgt.Surface->copyTo(pageholder, pgt.PageRect.UpperLeftCorner);
			}
		}

		Texture->unlock();
		pageholder->drop();
	}
	GlyphLayersToBePaged.clear();
}

bool CGUITTGlyphPage::reserveGlyphSpace(irr::core::recti& rect, u32 width, u32 height)
{
	if ( Texture )
	{
		core::dimension2du texSize = Texture->getSize();

		// Fits into current last row?
		if (	LastRow.LowerRightCorner.X + width <= texSize.Width
			&&	LastRow.UpperLeftCorner.Y + height <= texSize.Height )
		{
			rect.UpperLeftCorner.X = LastRow.LowerRightCorner.X;
			rect.LowerRightCorner.X = rect.UpperLeftCorner.X + width;
			rect.UpperLeftCorner.Y = LastRow.UpperLeftCorner.Y;
			rect.LowerRightCorner.Y = rect.UpperLeftCorner.Y + height;

			LastRow.LowerRightCorner.X = rect.LowerRightCorner.X;
			LastRow.LowerRightCorner.Y = irr::core::max_(LastRow.LowerRightCorner.Y, rect.LowerRightCorner.Y);
			return true;
		}
		// Can we create a new row?
		if (	width <= texSize.Width
			&&  LastRow.LowerRightCorner.Y + height <= texSize.Height )
		{
			rect.UpperLeftCorner.X = 0;
			rect.LowerRightCorner.X = rect.UpperLeftCorner.X + width;
			rect.UpperLeftCorner.Y = LastRow.LowerRightCorner.Y;
			rect.LowerRightCorner.Y = rect.UpperLeftCorner.Y + height;

			LastRow = rect;
			return true;
		}
	}

	return false;
}



// --- CGUITTFont ---

CGUITTFont* CGUITTFont::createTTFont(irr::video::IVideoDriver* driver, irr::io::IFileSystem* fileSystem, const io::path& filename, u32 size, bool antialias, bool transparency, float outline, irr::ILogger* logger)
{
	if ( !driver )
		return 0;

	if (!c_libraryLoaded)
	{
		if (FT_Init_FreeType(&c_library))
			return 0;
		c_libraryLoaded = true;
		if (logger)
			logger->log(L"CGUITTFont", L"Freetype got initialized", irr::ELL_INFORMATION);
	}

	CGUITTFont* font = new CGUITTFont(driver, fileSystem);
	font->Logger = logger;
	bool ret = font->load(filename, size, antialias, transparency, outline);
	if (!ret)
	{
		font->drop();
		return 0;
	}

	return font;
}

//! Constructor.
CGUITTFont::CGUITTFont(irr::video::IVideoDriver* driver, irr::io::IFileSystem* fileSystem)
: UseMonochrome(false), UseTransparency(true), UseHinting(true), UseAutoHinting(false)
, Outline(0.f), OutlineColor(255, 255, 255, 255)
, Size(0), MaxFontHeight(0), LineHeight(0)
, BatchLoadSize(1), Driver(driver), FileSystem(fileSystem), Logger(0), GlobalKerningWidth(0), GlobalKerningHeight(0)
{
	#ifdef _DEBUG
	setDebugName("CGUITTFont");
	#endif
}

bool CGUITTFont::load(const io::path& filename, u32 size, bool antialias, bool transparency, float outline)
{
	// Some sanity checks.
	if (Driver == 0) return false;
	if (size == 0) return false;
	if (filename.size() == 0) return false;

	this->Size = size;
	this->Filename = filename;

	// Update the font loading flags when the font is first loaded.
	this->UseMonochrome = !antialias;
	this->UseTransparency = transparency;
	this->Outline = outline;
	update_load_flags();

	// Log.
	if (Logger)
		Logger->log(L"CGUITTFont", core::stringw(core::stringw(L"Creating new font: ") + core::stringw(filename) + L" " + core::stringc(size) + L"pt " + (antialias ? L"+antialias " : L"-antialias ") + (transparency ? L"+transparency" : L"-transparency")).c_str(), irr::ELL_INFORMATION);

	// Grab the face.
	SGUITTFace* face = 0;
	core::map<io::path, SGUITTFace*>::Node* node = c_faces.find(filename);
	if (node == 0)
	{
		face = new SGUITTFace();
		c_faces.set(filename, face);

		if (FileSystem)
		{
			// Read in the file data.
			io::IReadFile* file = FileSystem->createAndOpenFile(filename);
			if (!file)
			{
				if (Logger)
					Logger->log(L"CGUITTFont - failed to open the file.", core::stringw(filename).c_str(), irr::ELL_INFORMATION);

				c_faces.remove(filename);
				delete face;
				face = 0;
				return false;
			}
			face->FaceBuffer = new FT_Byte[file->getSize()];
			file->read(face->FaceBuffer, file->getSize());
			face->FaceBufferSize = file->getSize();
			file->drop();

			// Create the face.
			if (FT_New_Memory_Face(c_library, face->FaceBuffer, face->FaceBufferSize, 0, &face->Face))
			{
				if (Logger)
					Logger->log(L"CGUITTFont", L"FT_New_Memory_Face failed.", irr::ELL_INFORMATION);

				c_faces.remove(filename);
				delete face;
				face = 0;
				return false;
			}
		}
		else
		{
			core::stringc converter(filename);
			if (FT_New_Face(c_library, converter.c_str(), 0, &face->Face))
			{
				if (Logger)
					Logger->log(L"CGUITTFont", L"FT_New_Face failed.", irr::ELL_INFORMATION);

				c_faces.remove(filename);
				delete face;
				face = 0;
				return false;
			}
		}
	}
	else
	{
		// Using another instance of this face.
		face = node->getValue();
		face->grab();
	}

	// Store our face.
	TTface = face->Face;
	if ( !TTface )
	{
		// memory corruption?
		if (Logger)
			Logger->log(L"CGUITTFont", L"Got a face but TTface is suddenly 0.", irr::ELL_INFORMATION);
		return false;
	}

	// Store font metrics.
	FT_Set_Pixel_Sizes(TTface, Size, 0);
	FontMetrics = TTface->size->metrics;

	// Allocate our glyphs.
	Glyphs.set_used(0);
	Glyphs.reallocate(TTface->num_glyphs);
	for (FT_Long i = 0; i < TTface->num_glyphs; ++i)
	{
		Glyphs.push_back(SGUITTGlyph());
	}

	// Cache the first 127 ascii characters.
	u32 old_size = BatchLoadSize;
	BatchLoadSize = 127;
	getGlyphIndexByChar((wchar_t)0);
	BatchLoadSize = old_size;

	// Get the maximum line height.  Unfortunately, we have to do this hack as
	// Irrlicht will draw things wrong.  In FreeType, the font size is the
	// maximum size for a single glyph, but that glyph may hang "under" the
	// draw line, increasing the total font height to beyond the set size.
	// Irrlicht does not understand this concept when drawing fonts.  Also, I
	// add +1 to give it a 1 pixel blank border.  This makes things like
	// tooltips look nicer.
	MaxFontHeight = getHeightFromCharacter(L'g') + 1;
	MaxFontHeight = core::max_(MaxFontHeight, getHeightFromCharacter(L'j') + 1);
	MaxFontHeight = core::max_(MaxFontHeight, getHeightFromCharacter(L'_') + 1);
	LineHeight = core::max_(FontMetrics.ascender, FontMetrics.height) / 64;
	LineHeight = core::max_(LineHeight, MaxFontHeight);

	return true;
}

CGUITTFont::~CGUITTFont()
{
	// Delete the glyphs and glyph pages.
	reset_images();

	// We aren't using this face anymore.
	core::map<io::path, SGUITTFace*>::Node* n = c_faces.find(Filename);
	if (n)
	{
		SGUITTFace* f = n->getValue();

		// Drop our face.  If this was the last face, the destructor will clean up.
		if (f->drop())
			c_faces.remove(Filename);

		// If there are no more faces referenced by FreeType, clean up.
		if (c_faces.size() == 0)
		{
			FT_Done_FreeType(c_library);
			c_libraryLoaded = false;

			if (Logger)
				Logger->log(L"CGUITTFont", L"Freetype got released", irr::ELL_INFORMATION);
		}
	}
}

void CGUITTFont::reset_images()
{
	// Delete the glyphs.
	for (u32 i = 0; i != Glyphs.size(); ++i)
		Glyphs[i].unload();

	// Unload the glyph pages from video memory.
	for (u32 i = 0; i != GlyphPages.size(); ++i)
		delete GlyphPages[i];
	GlyphPages.clear();

	// Always update the internal FreeType loading flags after resetting.
	update_load_flags();
}

void CGUITTFont::update_glyph_pages() const
{
	for (u32 i = 0; i != GlyphPages.size(); ++i)
	{
		GlyphPages[i]->updateTexture();
	}
}

CGUITTGlyphPage* CGUITTFont::createGlyphPage(const irr::video::ECOLOR_FORMAT colorFormat)
{
	// Name of our page.
	io::path name("TTFGlyphPage_");
	name += TTface->family_name;
	name += ".";
	name += TTface->style_name;
	name += ".";
	name += Size;
	name += ".";
	name += (int)(Outline*64.f),
	name += "_";
	name += GlyphPages.size(); // The newly created page will be at the end of the collection.

	// Create the new page.
	CGUITTGlyphPage* page = new CGUITTGlyphPage(Driver, name);

	// Determine our maximum texture size.
	// If we keep getting 0, set it to 1024x1024, as that number is pretty safe.
	core::dimension2du max_texture_size = MaxPageTextureSize;
	if (max_texture_size.Width == 0 || max_texture_size.Height == 0)
		max_texture_size = Driver->getMaxTextureSize();
	if (max_texture_size.Width == 0 || max_texture_size.Height == 0)
		max_texture_size = core::dimension2du(1024, 1024);

	// We want to try to put at least 144 glyphs on a single texture.
	core::dimension2du page_texture_size;
	if (Size <= 21) page_texture_size = core::dimension2du(256, 256);
	else if (Size <= 42) page_texture_size = core::dimension2du(512, 512);
	else if (Size <= 84) page_texture_size = core::dimension2du(1024, 1024);
	else if (Size <= 168) page_texture_size = core::dimension2du(2048, 2048);
	else page_texture_size = core::dimension2du(4096, 4096);

	if (page_texture_size.Width > max_texture_size.Width || page_texture_size.Height > max_texture_size.Height)
		page_texture_size = max_texture_size;

	if (!page->createPageTexture(colorFormat, page_texture_size))
	{
		delete page;
		return 0;
	}

	GlyphPages.push_back(page);

	return page;
}

CGUITTGlyphPage* CGUITTFont::getGlyphPageSpot(irr::core::recti& spotRect, const irr::video::ECOLOR_FORMAT colorFormat, u32 width, u32 height)
{
	// Get the last page
	CGUITTGlyphPage* page = GlyphPages.empty() ? 0 : GlyphPages.getLast();

	// Try to get some space there
	if ( page && page->reserveGlyphSpace(spotRect, width, height) )
	{
		return page;
	}

	// No space on last page (or no page yet), so create a new one
	page = createGlyphPage(colorFormat);

	// Try again to get some space there
	if ( page && page->reserveGlyphSpace(spotRect, width, height) )
	{
		return page;
	}

	return 0;
}

void CGUITTFont::setTransparency(const bool flag)
{
	if ( flag != UseTransparency )
	{
		UseTransparency = flag;
		reset_images();
	}
}

void CGUITTFont::setMonochrome(const bool flag)
{
	if ( flag != UseMonochrome )
	{
		UseMonochrome = flag;
		reset_images();
	}
}

void CGUITTFont::setFontHinting(const bool enable, const bool enable_auto_hinting)
{
	if ( UseHinting != enable || UseAutoHinting !=enable_auto_hinting )
	{
		UseHinting = enable;
		UseAutoHinting = enable_auto_hinting;
		reset_images();
	}
}

void CGUITTFont::setOutline(float outline)
{
	if ( !core::equalsByUlp(Outline, outline, 1) )
	{
		Outline = outline;
		reset_images();
	}
}

void CGUITTFont::draw(const core::stringw& text, const core::rect<s32>& position, video::SColor color, bool hcenter, bool vcenter, const core::rect<s32>* clip)
{
	if (!Driver)
		return;

	// Clear the glyph pages of their render information.
	for (u32 i = 0; i < GlyphPages.size(); ++i)
	{
		GlyphPages[i]->RenderPositions.set_used(0);
		GlyphPages[i]->RenderSourceRects.set_used(0);
		GlyphPages[i]->OutlineRenderPositions.set_used(0);
		GlyphPages[i]->OutlineRenderSourceRects.set_used(0);
	}

	// Set up some variables.
	core::dimension2d<s32> textDimension;
	core::position2d<s32> offset = position.UpperLeftCorner;

	// Determine offset positions.
	if (hcenter || vcenter)
	{
		textDimension = getDimension(text.c_str());

		if (hcenter)
			offset.X = ((position.getWidth() - textDimension.Width) >> 1) + offset.X;

		if (vcenter)
			offset.Y = ((position.getHeight() - textDimension.Height) >> 1) + offset.Y;
	}

	// Start parsing characters.
	wchar_t previousChar = 0;
	const wchar_t * iter = text.c_str();
	while (*iter)
	{
		wchar_t currentChar = *iter;
		u32 n = getGlyphIndexByChar(currentChar);
		bool visible = (Invisible.findFirst(currentChar) == -1);
		if (visible)
		{
			bool lineBreak=false;
			if (currentChar == L'\r') // Mac or Windows breaks
			{
				lineBreak = true;
				if (*(iter + 1) == L'\n')	// Windows line breaks.
					currentChar = *(++iter);
			}
			else if (currentChar == L'\n') // Unix breaks
			{
				lineBreak = true;
			}

			if (lineBreak)
			{
				previousChar = 0;
				offset.Y += LineHeight;
				offset.X = position.UpperLeftCorner.X;

				if (hcenter)
					offset.X += (position.getWidth() - textDimension.Width) >> 1;
				++iter;
				continue;
			}

			if ( n > 0 )
			{
				SGUITTGlyph& glyph = Glyphs[n-1];

				// Apply kerning.
				core::vector2di k = getKerning(currentChar, previousChar);
				offset.X += k.X;
				offset.Y += k.Y;

				if ( glyph.TopLayer.GlyphPage )
				{
					// Calculate the glyph offset.
					s32 offx = glyph.TopLayer.Offset.X;
					s32 offy = (FontMetrics.ascender / 64) - glyph.TopLayer.Offset.Y;

					// Determine rendering information.
					glyph.TopLayer.GlyphPage->RenderPositions.push_back(core::position2di(offset.X + offx, offset.Y + offy));
					glyph.TopLayer.GlyphPage->RenderSourceRects.push_back(glyph.TopLayer.SourceRect);
				}

				if ( Outline != 0.f && glyph.OutlineLayer.GlyphPage )
				{
					// Calculate the glyph offset.
					s32 offx = glyph.OutlineLayer.Offset.X;
					s32 offy = (FontMetrics.ascender / 64) - glyph.OutlineLayer.Offset.Y;

					// Determine rendering information.
					glyph.OutlineLayer.GlyphPage->OutlineRenderPositions.push_back(core::position2di(offset.X + offx, offset.Y + offy));
					glyph.OutlineLayer.GlyphPage->OutlineRenderSourceRects.push_back(glyph.OutlineLayer.SourceRect);
				}
			}
		}
		offset.X += getWidthFromCharacter(currentChar);

		previousChar = currentChar;
		++iter;
	}

	// some glyphs might be changed, so update texture pages
	update_glyph_pages();

	if (!UseTransparency)
		color.color |= 0xff000000;

	// draw outline if we have some
	if ( Outline != 0.f )
	{
		for (u32 i = 0; i < GlyphPages.size(); ++i)
		{
			CGUITTGlyphPage* page = GlyphPages[i];

			if ( !page->OutlineRenderPositions.empty() )
			{
				Driver->draw2DImageBatch(page->Texture, page->OutlineRenderPositions, page->OutlineRenderSourceRects, clip, OutlineColor, true);
			}
		}
	}

	// draw normal glyph
	for (u32 i = 0; i < GlyphPages.size(); ++i)
	{
		CGUITTGlyphPage* page = GlyphPages[i];

		if ( !page->RenderPositions.empty() )
		{
			Driver->draw2DImageBatch(page->Texture, page->RenderPositions, page->RenderSourceRects, clip, color, true);
		}
	}
}

core::dimension2d<u32> CGUITTFont::getCharDimension(const wchar_t ch) const
{
	return core::dimension2d<u32>(getWidthFromCharacter(ch), getHeightFromCharacter(ch));
}

core::dimension2d<u32> CGUITTFont::getDimension(const wchar_t* text) const
{
	core::dimension2d<u32> text_dimension(0, MaxFontHeight);
	core::dimension2d<u32> line(0, LineHeight);

	wchar_t previousChar = 0;
	for(const wchar_t* iter = text; *iter; ++iter)
	{
		bool lineBreak = false;
		if (*iter == L'\r')	// Mac or Windows line breaks.
		{
			lineBreak = true;
			if (*(iter + 1) == L'\n')
			{
				++iter;
			}
		}
		else if (*iter == L'\n')	// Unix line breaks.
		{
			lineBreak = true;
		}

		if (lineBreak)
		{
			previousChar = 0;
			text_dimension.Height += line.Height;
			if (text_dimension.Width < line.Width)
				text_dimension.Width = line.Width;
			line.Width = 0;
		}
		else
		{
			// Kerning.
			core::vector2di k = getKerning(*iter, previousChar);
			line.Width += k.X;
			line.Width += getWidthFromCharacter(*iter);
			previousChar = *iter;
		}
	}
	if (text_dimension.Width < line.Width)
		text_dimension.Width = line.Width;

	return text_dimension;
}

inline u32 CGUITTFont::getWidthFromCharacter(wchar_t c) const
{
	// Set the size of the face.
	// This is because we cache faces and the face may have been set to a different size.

	u32 n = getGlyphIndexByChar(c);
	if (n > 0)
	{
		SGUITTGlyph& glyph = Glyphs[n-1];
		s32 w1 = glyph.TopLayer.GlyphPage ? glyph.TopLayer.Advance.x : 0;
		s32 w2 = glyph.OutlineLayer.GlyphPage ? glyph.OutlineLayer.Advance.x : 0;
		int w = irr::core::max_(w1, w2) / 64;
		return w;
	}
	if (c >= 0x2000)
		return (FontMetrics.ascender / 64);
	else
		return (FontMetrics.ascender / 64) / 2;
}

inline u32 CGUITTFont::getHeightFromCharacter(wchar_t c) const
{
	// Set the size of the face.
	// This is because we cache faces and the face may have been set to a different size.

	u32 n = getGlyphIndexByChar(c);
	if (n > 0)
	{
		// Grab the true height of the character, taking into account underhanging glyphs.
		SGUITTGlyph& glyph = Glyphs[n-1];
		s32 hg1 = glyph.TopLayer.GlyphPage ? -glyph.TopLayer.Offset.Y + glyph.TopLayer.SourceRect.getHeight() : 0;
		s32 hg2 = glyph.OutlineLayer.GlyphPage ? -glyph.OutlineLayer.Offset.Y + glyph.OutlineLayer.SourceRect.getHeight() : 0;
		s32 height = (FontMetrics.ascender / 64) + irr::core::max_(hg1, hg2);
		return height;
	}
	if (c >= 0x2000)
		return (FontMetrics.ascender / 64);
	else
		return (FontMetrics.ascender / 64) / 2;
}

u32 CGUITTFont::getGlyphIndexByChar(wchar_t c) const
{
	// Get the glyph.
	u32 glyphIndex = FT_Get_Char_Index(TTface, c);

	// If our glyph is already loaded, don't bother doing any batch loading code.
	if (glyphIndex != 0 && Glyphs[glyphIndex - 1].IsLoaded())
		return glyphIndex;

	// Determine our batch loading positions.
	u32 half_size = (BatchLoadSize / 2);
	u32 start_pos = 0;
	if ((u32)c > half_size)
		start_pos = c - half_size;
	u32 end_pos = start_pos + BatchLoadSize;

	// Load all our characters.
	do
	{
		// Get the character we are going to load.
		u32 char_index = FT_Get_Char_Index(TTface, start_pos);

		// If the glyph hasn't been loaded yet, do it now.
		if (char_index)
		{
			SGUITTGlyph& glyph = Glyphs[char_index - 1];
			if (!glyph.IsLoaded())
			{
				// Breaking const here is ugly. But getGlyphIndexByChar is used by functions which must be const to work with that interface, so we need to hack a little.
				// It's basically about caching.
				glyph.preload(const_cast<irr::gui::CGUITTFont&>(*this), char_index, TTface, Size, Outline, LoadFlags);
			}
		}
	}
	while (++start_pos < end_pos);

	// Return our original character.
	return glyphIndex;
}

s32 CGUITTFont::getCharacterFromPos(const wchar_t* text, s32 pixel_x) const
{
	s32 x = 0;

	u32 character = 0;
	wchar_t previousChar = 0;
	while (text[character])
	{
		wchar_t c = text[character];
		x += getWidthFromCharacter(c);

		// Kerning.
		core::vector2di k = getKerning(c, previousChar);
		x += k.X;

		if (x >= pixel_x)
			return character;

		previousChar = c;
		++character;
	}

	return -1;
}

void CGUITTFont::setKerningWidth(s32 kerning)
{
	GlobalKerningWidth = kerning;
}

void CGUITTFont::setKerningHeight(s32 kerning)
{
	GlobalKerningHeight = kerning;
}

s32 CGUITTFont::getKerningWidth(const wchar_t* thisLetter, const wchar_t* previousLetter) const
{
	if (TTface == 0)
		return GlobalKerningWidth;
	if (thisLetter == 0 || previousLetter == 0)
		return 0;

	// Return only the kerning width.
	return getKerning(*thisLetter, *previousLetter).X;
}

s32 CGUITTFont::getKerningHeight() const
{
	// FreeType 2 currently doesn't return any height kerning information.
	return GlobalKerningHeight;
}

core::vector2di CGUITTFont::getKerning(const wchar_t thisLetter, const wchar_t previousLetter) const
{
	if (TTface == 0 || thisLetter == 0 || previousLetter == 0)
		return core::vector2di();

	// Set the size of the face.
	// This is because we cache faces and the face may have been set to a different size.
	FT_Set_Pixel_Sizes(TTface, 0, Size);

	core::vector2di ret(GlobalKerningWidth, GlobalKerningHeight);

	// If we don't have kerning, no point in continuing.
	if (!FT_HAS_KERNING(TTface))
		return ret;

	// Get the kerning information.
	FT_Vector v;
	FT_Get_Kerning(TTface, getGlyphIndexByChar(previousLetter), getGlyphIndexByChar(thisLetter), FT_KERNING_DEFAULT, &v);

	// If we have a scalable font, the return value will be in font points.
	if (FT_IS_SCALABLE(TTface))
	{
		// Font points, so divide by 64.
		ret.X += (v.x / 64);
		ret.Y += (v.y / 64);
	}
	else
	{
		// Pixel units.
		ret.X += v.x;
		ret.Y += v.y;
	}
	return ret;
}

void CGUITTFont::setInvisibleCharacters(const wchar_t *s)
{
	Invisible = s;
}

video::ITexture* CGUITTFont::getPageTextureByIndex(u32 page_index) const
{
	if (page_index < GlyphPages.size())
		return GlyphPages[page_index]->Texture;
	return 0;
}

} // end namespace gui
} // end namespace irr
