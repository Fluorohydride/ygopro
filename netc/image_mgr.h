#ifndef _IMAGE_MGR_H_
#define _IMAGE_MGR_H_

#include "GL/glew.h"
#include "wx/image.h"
#include "wx/xml/xml.h"
#include <vector>
#include <unordered_map>

#define TEXINDEX_FIELD		0
#define TEXINDEX_ATTACK		1
#define TEXINDEX_ACTIVATE	2
#define TEXINDEX_CHAIN		3
#define TEXINDEX_MASK		4
#define TEXINDEX_NEGATED	5
#define TEXINDEX_LIMIT0		6
#define TEXINDEX_LIMIT1		7
#define TEXINDEX_LIMIT2		8
#define TEXINDEX_LPFRAME	9
#define TEXINDEX_LPBAR		10
#define TEXINDEX_EQUIP		11
#define TEXINDEX_TARGET		12
#define TEXINDEX_SCISSORS	13
#define TEXINDEX_ROCK		14
#define TEXINDEX_PAPER		15
#define TEXINDEX_NUMBERS	16

namespace ygopro
{
	struct TextureInfo {
		TextureInfo() : lx(0.0f), ly(0.0f), rx(0.0f), ry(0.0f) {}
		float lx;
		float ly;
		float rx;
		float ry;
	};

	struct LayoutInfo {
		bool show;
		float x1;
		float y1;
		float x2;
		float y2;
		float x3;
		float y3;
		float x4;
		float y4;
		
	};

	class ImageMgr {

	public:
		ImageMgr();
		~ImageMgr();

		inline unsigned int texlen(unsigned int len) {
			len = len - 1;
			len = len | (len >> 1);
			len = len | (len >> 2);
			len = len | (len >> 4);
			len = len | (len >> 8);
			len = len | (len >> 16);
			return len + 1;
		}

		void InitTextures();
		TextureInfo& GetCardTexture(unsigned int id);
		TextureInfo& ReloadCardTexture(unsigned int id);
		unsigned int LoadTexture(const wxImage& img);
		TextureInfo LoadCard(const wxImage& img);
		
		void LoadSingleImage(wxImage& img, const wxString& file);
		void LoadImageConfig(const wxString& file);
		void LoadLayoutConfig(const wxString& file);

		wxImage image_texture;
		wxImage image_unknown;
		wxImage image_sleeve1;
		wxImage image_sleeve2;
		wxImage image_bg;
		unsigned int textureid_all;
		unsigned int textureid_card;
		unsigned int textureid_bg;

		unsigned int card_index;

		TextureInfo background;
		TextureInfo card_unknown;
		TextureInfo card_sleeve1;
		TextureInfo card_sleeve2;

		unsigned int system_texture[32];
		TextureInfo textures[64];
	private:
		std::unordered_map<unsigned int, TextureInfo> card_textures;
		std::unordered_map<unsigned int, wxImage*> card_images;
	};

	extern ImageMgr imageMgr;

}

#endif
