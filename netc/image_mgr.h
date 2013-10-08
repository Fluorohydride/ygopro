#ifndef _IMAGE_MGR_H_
#define _IMAGE_MGR_H_

#include "GL/glew.h"
#include "wx/image.h"
#include "wx/xml/xml.h"
#include <array>
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
#define TEXINDEX_EQUIP		9
#define TEXINDEX_TARGET		10
#define TEXINDEX_SCISSORS	11
#define TEXINDEX_ROCK		12
#define TEXINDEX_PAPER		13
#define TEXINDEX_SOCG		14
#define TEXINDEX_STCG		15
#define TEXINDEX_SCTM		16
#define TEXINDEX_STST		17
#define TEXINDEX_BUILDBG    18
#define TEXINDEX_SLEEVE1    19
#define TEXINDEX_AVATAR1    20
#define TEXINDEX_LPFRAME1	21
#define TEXINDEX_LPBAR1		22
#define TEXINDEX_SLEEVE2    23
#define TEXINDEX_AVATAR2    24
#define TEXINDEX_LPFRAME2	25
#define TEXINDEX_LPBAR2		26

#define LAYOUT_STATIC	0
#define LAYOUT_LP		1
#define LAYOUT_TEXT		2
#define LAYOUT_PHASE	3
#define LAYOUT_BUTTON	4

#define CLICK_NONE		0
#define CLICK_AVATAR0	1
#define CLICK_AVATAR1	2
#define CLICK_BP		3
#define CLICK_M2		4
#define CLICK_EP		5
#define CLICK_MENU		6
#define CLICK_SURRENDER	7
#define CLICK_CONFIRM	8

namespace ygopro
{
	struct TextureInfo {
		TextureInfo() : index(0), lx(0.0), ly(0.0), rx(0.0), ry(0.0) {}
		unsigned int index;
		double lx;
		double ly;
		double rx;
		double ry;
	};

    struct CardImageInfo {
        unsigned int ref_count;
        wxImage* card_image;
        TextureInfo texture;
    };
    
	struct LayoutInfo {
		int style;
		int click;
		double x1;
		double y1;
		double x2;
		double y2;
		double x3;
		double y3;
		double x4;
		double y4;
		TextureInfo* ptex;
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
		void UnloadCardTexture(unsigned int id);
		unsigned int LoadTexture(const wxImage& img);
		TextureInfo LoadCard(const wxImage& img);
		
		void LoadSingleImage(wxImage& img, const wxString& file);
		void LoadImageConfig(const wxString& file);
		void LoadLayoutConfig(const wxString& file);

        std::vector<wxImage*> image_texture;
        std::vector<unsigned int> textures_id;
		TextureInfo background;
		TextureInfo card_unknown;
		TextureInfo card_sleeve1;
		TextureInfo card_sleeve2;
		std::array<TextureInfo*, 32> system_texture;
		std::vector<TextureInfo> textures;
		std::vector<TextureInfo> text_texture;
		std::vector<LayoutInfo> layouts;
		std::vector<LayoutInfo> clickable;

	private:
		std::unordered_map<unsigned int, TextureInfo> card_textures;
		std::unordered_map<unsigned int, wxImage*> card_images;
		std::unordered_map<std::string, TextureInfo*> texture_infos;
	};

	extern ImageMgr imageMgr;

}

#endif
