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

/*
	This class is discussed in the following forum-thread: http://irrlicht.sourceforge.net/forum/viewtopic.php?f=6&t=37296

	Changes by Michael Zeilfelder:
	- Remove ustring (otherwise I risk converting strings twice in my project as they are UCS-2 or UCS-4 already)
	- Remove const-ref in places where it makes no sense
	- Remove additional create functions and keep just one. Pass only the parameters actually needed to it.
	- Removing allocator as I get memory-leaks and don't understand what the allocater was meant for.
	- Glyph_Pages no longer mutable (doesn't seem to need that)
	- remove textscenenodes. Probably shouldn't be in this class and make understanding it harder.
	- remove createTextureFromChar (it also had a memory leak probably as it wasn't dropping pageholder)
	- remove Glyphs.set_free_when_destroyed(false). That function prevents clearing the array - it's not about pointers. Rather fix SGUITTGlyph (which didn't care about rule of three).
	- don't drop the driver (not grabbed - but I changed that stuff a lot so might not have been there originally)
	- remove a few memory re-allocations in drawing
	- CGUITTGlyphPage::updateTexture replaced dirtyflag by check for glyphs to handle
	- CGUITTGlyphPage::updateTexture works with ::texture->getSize() instead of getOriginalSize. Same result in this case, but more correct.
	- Irrlichtify code (variable naming etc)
	- Add support for outlines (lot of code got changed for that, I guess original code now barely recognizable)
	- Ensure Texture is grab()'ed. It's dangerous to remove it from driver otherwise (which might have cleared it's cache already)
	- Rename "use" functions to "get" so we have setters and getters
	- LineHeight and maximal character height now only calculated once after load
	- Using LineHeight not just for getDimension, but also for draw calls.
	- Remove unused function CGUITTFont::getDimensionUntilEndOfLine

	TODO:
	- Hinting should be one enum with explanation (have to figure it out first, results are strange currently when I enable it)
*/

#ifndef __C_GUI_TTFONT_H_INCLUDED__
#define __C_GUI_TTFONT_H_INCLUDED__

#include <ft2build.h>
#include FT_FREETYPE_H	// official way to include freetype.h correct according to freetype documentation
#include FT_STROKER_H
#include FT_BITMAP_H
#include <irrlicht.h>

// TODO: Shouldn't be in irr namespace as long as it's not in the engine (but leaving it for now in the hope that will change)
namespace irr
{
namespace gui
{
	struct SGUITTFace;
	class CGUITTFont;
	class CGUITTGlyphPage;

	// A line of Width pixels starting at X,Y.
	struct SGlyphPixelSpan
	{
		SGlyphPixelSpan() { }
		SGlyphPixelSpan(int _x, int _y, int _width, int _coverage)
		: X(_x), Y(_y), Width(_width), Coverage(_coverage) { }

		int X, Y;
		int Width;
		int Coverage;	// Alpha value, probably in range 0-255
	};

	//! Structure representing a single TrueType glyph.
	struct SGUITTGlyph
	{
		//! Preload the glyph.
		//!	The preload process occurs when the program tries to cache the glyph from FT_Library.
		//! However, it simply defines the SGUITTGlyph's properties and will only create the page
		//! textures if necessary.  The actual creation of the textures should only occur right
		//! before the batch draw call.
		void preload(CGUITTFont& font, u32 char_index, FT_Face face, u32 fontSize, float outline, FT_Int32 loadFlags);

		//! Unloads the glyph.
		void unload();

		//! If true, the glyph has been loaded.
		bool IsLoaded() const { return TopLayer.GlyphPage || OutlineLayer.GlyphPage; }

		//! Structure for layers inside SGUITTGlyph
		struct SGlyphLayer
		{
			SGlyphLayer() : GlyphPage(0) {}

			void clear()
			{
				GlyphPage = 0;
			}

			//! The page the glyph is on. Weak pointer.
			CGUITTGlyphPage* GlyphPage;

			//! The source rectangle for the glyph.
			core::recti SourceRect;

			//! The offset of glyph when drawn.
			core::vector2di Offset;

			//! Glyph advance information.
			FT_Vector Advance;
		};

		SGlyphLayer TopLayer;
		SGlyphLayer OutlineLayer;

	protected:
		//! Creates the IImage object from the FT_Bitmap.
		video::IImage* createGlyphImageFromBitmap(const irr::video::ECOLOR_FORMAT colorFormat, const FT_Bitmap& bits, video::IVideoDriver* driver) const;
		video::IImage* createGlyphImageFromPixelSpans(const irr::video::ECOLOR_FORMAT colorFormat, video::IVideoDriver* driver, int width, int height) const;

		bool renderSpans(FT_Library &library, FT_Face &face, float outline);

	private:
		//! description of glyph using pixel spans
		irr::core::array<SGlyphPixelSpan> PixelSpans;
		irr::core::array<SGlyphPixelSpan> PixelOutlineSpans;

	};

	//! Holds a sheet of glyphs rendered to a texture
	class CGUITTGlyphPage
	{
		private:
			struct PagedGlyphTextures
			{
				PagedGlyphTextures(video::IImage* surface, const irr::core::recti& pageRect)
					: Surface(surface), PageRect(pageRect)
				{
					if ( Surface )
						Surface->grab();
				}

				PagedGlyphTextures(const PagedGlyphTextures& other) : Surface(0)
				{
					*this = other;
				}

				~PagedGlyphTextures()
				{
					if ( Surface )
						Surface->drop();
				}

				PagedGlyphTextures& operator=(const PagedGlyphTextures& other)
				{
					if ( this != &other )
					{
						PageRect = other.PageRect;
						if ( Surface != other.Surface )
						{
							if ( Surface )
								Surface->drop();
							Surface = other.Surface;
							if ( Surface )
								Surface->grab();
						}
					}

					return *this;
				}

				video::IImage* Surface;	// Temporary holder, drop after glyph is paged
				irr::core::recti PageRect;
			};


		public:
			CGUITTGlyphPage(video::IVideoDriver* driver, const io::path& textureName)
				: Texture(0), Driver(driver), Name(textureName)
			{
			}

			~CGUITTGlyphPage()
			{
				if (Texture)
				{
					if (Driver)
						Driver->removeTexture(Texture);
					Texture->drop();
				}
			}

			//! Create the actual page texture,
			bool createPageTexture(const irr::video::ECOLOR_FORMAT colorFormat, const core::dimension2du& textureSize);

			//! Add the glyphlayer to a list of textures to be paged.
			//! This collection will be cleared after updateTexture is called.
			//\param surface Should contain an image with the glyph (for this layer)
			//\param rect Target rectangle to be used on this page
			void pushGlyphLayerToBePaged(video::IImage* surface, const irr::core::recti& pageRect)
			{
				if ( surface )
				{
					GlyphLayersToBePaged.push_back(CGUITTGlyphPage::PagedGlyphTextures(surface, pageRect));
				}
			}

			//! Updates the texture atlas with new glyphs.
			void updateTexture();

			//! Request space to place a glyph with given width/height. Returns reserved space in rect.
			//\return true when reserving worked out, false when there was no space left on this page
			bool reserveGlyphSpace(irr::core::recti& rect, u32 width, u32 height);

			video::ITexture* Texture;	// contains bitmaps of all glyphs on this page

			// Glyphs which will be drawn on next draw call
			core::array<core::vector2di> RenderPositions;
			core::array<core::recti> RenderSourceRects;
			core::array<core::vector2di> OutlineRenderPositions;
			core::array<core::recti> OutlineRenderSourceRects;

		private:

			core::array<PagedGlyphTextures> GlyphLayersToBePaged;
			video::IVideoDriver* Driver;
			io::path Name;	// texture-name
			core::recti LastRow; // Contains glyph space reserved in the row which was last used to reserve glyphs (to figure out where to reserve space for next glyph)
	};

	//! Class representing a TrueType font.
	class CGUITTFont : public irr::gui::IGUIFont
	{
		friend struct SGUITTGlyph;

		public:
			//! Creates a new TrueType font and returns a pointer to it.  The pointer must be drop()'ed when finished.
			//! \param driver Irrlicht video driver
			//! \param fileSystem Irrlicht filesystem
			//! \param filename The filename of the font.
			//! \param size The size of the font glyphs in pixels.  Since this is the size of the individual glyphs, the true height of the font may change depending on the characters used.
			//! \param antialias set the use_monochrome (opposite to antialias) flag
			//! \param transparency Control if alpha-value of font-color is used or ignored.
			//! \param invisibleChars Set characters which don't need drawing (speed optimization)
			//! \param logger Irrlicht logging, for printing out additional warnings/errors
			//! \param outline Render an outline with a different color (default white) behind the text
			//! \return Returns a pointer to a CGUITTFont.  Will return 0 if the font failed to load.
			static CGUITTFont* createTTFont(irr::video::IVideoDriver* driver, irr::io::IFileSystem* fileSystem, const io::path& filename, u32 size, bool antialias = true, bool transparency = true, float outline = 0.f, irr::ILogger* logger = 0);

			//! Destructor
			virtual ~CGUITTFont();


			//! Draws some text and clips it to the specified rectangle if wanted.
			virtual void draw(const core::stringw& text, const core::rect<s32>& position,
				video::SColor color, bool hcenter=false, bool vcenter=false,
				const core::rect<s32>* clip=0);

			//! Returns the dimension of a character produced by this font.
			virtual core::dimension2d<u32> getCharDimension(const wchar_t ch) const;

			//! Returns the dimension of a text string.
			virtual core::dimension2d<u32> getDimension(const wchar_t* text) const;

			//! Calculates the index of the character in the text which is on a specific position.
			virtual s32 getCharacterFromPos(const wchar_t* text, s32 pixel_x) const;

			//! Sets global kerning width for the font.
			virtual void setKerningWidth(s32 kerning);

			//! Sets global kerning height for the font.
			virtual void setKerningHeight(s32 kerning);

			//! Gets kerning values (distance between letters) for the font. If no parameters are provided,
			virtual s32 getKerningWidth(const wchar_t* thisLetter=0, const wchar_t* previousLetter=0) const;

			//! Returns the distance between letters
			virtual s32 getKerningHeight() const;

			//! Define which characters should not be drawn by the font.
			/** This is a speed optimization. For example spaces don't draw anything in most fonts.
			So making them invisible save the render-time for those. Instead an empty space with
			their width is added to the output. */
			virtual void setInvisibleCharacters(const wchar_t *s);



			//! Sets the amount of glyphs to batch load.
			void setBatchLoadSize(u32 batch_size) { BatchLoadSize = batch_size; }

			//! Sets the maximum texture size for a page of glyphs.
			void setMaxPageTextureSize(const core::dimension2du& texture_size) { MaxPageTextureSize = texture_size; }

			//! Get the font size.
			u32 getFontSize() const { return Size; }

			//! Check if font uses or ignores the alpha-channel of the color in draw calls
			bool getTransparency() const { return UseTransparency; }

			//! Check if the font auto-hinting is enabled.
			//! Auto-hinting is FreeType's built-in font hinting engine.
			bool getAutoHinting() const { return UseAutoHinting; }

			//! Check if the font hinting is enabled.
			bool getHinting()	 const { return UseHinting; }

			//! Check if the font is being loaded as a monochrome font.
			//! The font can either be a 256 color grayscale font, or a 2 color monochrome font.
			bool getMonochrome()  const { return UseMonochrome; }

			//! Tells the font to allow transparency when rendering.
			//! Default: true.
			//! \param flag If true, the font draws using transparency.
			void setTransparency(const bool flag);

			//! Tells the font to use monochrome rendering.
			//! Default: false.
			//! \param flag If true, the font draws using a monochrome image.  If false, the font uses a grayscale image.
			void setMonochrome(const bool flag);

			//! Enables or disables font hinting.
			//! Default: Hinting and auto-hinting true.
			//! \param enable If false, font hinting is turned off. If true, font hinting is turned on.
			//! \param enable_auto_hinting If true, FreeType uses its own auto-hinting algorithm.  If false, it tries to use the algorithm specified by the font.
			void setFontHinting(const bool enable, const bool enable_auto_hinting = true);

			void setOutline(float outline);
			float getOutline() const { return Outline; }

			void setOutlineColor(video::SColor color) { OutlineColor = color; }
			video::SColor getOutlineColor() const { return OutlineColor; }

			//! This function is for debugging mostly. If the page doesn't exist it returns zero.
			//! \param page_index Simply return the texture handle of a given page index.
			video::ITexture* getPageTextureByIndex(u32 page_index) const;

			//! Return the number of texture pages used internally
			irr::u32 getNumGlyphPages() const { return GlyphPages.size(); }

		protected:
			//! Create a new glyph page texture.
			//! \param pixel_mode the pixel mode defined by FT_Pixel_Mode
			//should be better typed. fix later.
			CGUITTGlyphPage* createGlyphPage(const irr::video::ECOLOR_FORMAT colorFormat);

			// Reserve a rectangle on a glyph page
			//\param spotRect Will return the target rectangle reserved on the page
			CGUITTGlyphPage* getGlyphPageSpot(irr::core::recti& spotRect, const irr::video::ECOLOR_FORMAT colorFormat, u32 width, u32 height);


		private:
			CGUITTFont(irr::video::IVideoDriver* driver, irr::io::IFileSystem* fileSystem);

			bool load(const io::path& filename, u32 size, bool antialias, bool transparency, float outline);
			void reset_images();
			void update_glyph_pages() const;
			void update_load_flags()
			{
				// Set up our loading flags.
				LoadFlags = FT_LOAD_DEFAULT | FT_LOAD_RENDER;
				if (!getHinting())
					LoadFlags |= FT_LOAD_NO_HINTING;
				if (!getAutoHinting())
					LoadFlags |= FT_LOAD_NO_AUTOHINT;
				if (getMonochrome())
					LoadFlags |= FT_LOAD_MONOCHROME | FT_LOAD_TARGET_MONO | FT_RENDER_MODE_MONO;
				else
					LoadFlags |= FT_LOAD_TARGET_NORMAL;
			}
			u32 getWidthFromCharacter(wchar_t c) const;
			u32 getHeightFromCharacter(wchar_t c) const;
			u32 getGlyphIndexByChar(wchar_t c) const;
			core::vector2di getKerning(const wchar_t thisLetter, const wchar_t previousLetter) const;

			// Manages the FreeType library.
			static FT_Library c_library;
			static core::map<io::path, SGUITTFace*> c_faces;
			static bool c_libraryLoaded;


			bool UseMonochrome;
			bool UseTransparency;
			bool UseHinting;
			bool UseAutoHinting;
			float Outline;
			video::SColor OutlineColor;
			u32 Size;
			u32 MaxFontHeight; // in pixels from sampling a few characters
			u32 LineHeight;	// in pixels and different from Size and MaxFontHeight
			u32 BatchLoadSize;
			core::dimension2du MaxPageTextureSize;

			irr::video::IVideoDriver* Driver;
			irr::io::IFileSystem* FileSystem;
			irr::ILogger* Logger;

			io::path Filename;
			FT_Face TTface;
			FT_Size_Metrics FontMetrics;
			FT_Int32 LoadFlags;

			core::array<CGUITTGlyphPage*> GlyphPages;
			mutable core::array<SGUITTGlyph> Glyphs;

			s32 GlobalKerningWidth;
			s32 GlobalKerningHeight;
			core::stringw Invisible;
	};

} // end namespace gui
} // end namespace irr

#endif // __C_GUI_TTFONT_H_INCLUDED__
