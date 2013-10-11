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
#define TEXINDEX_DUELBG     18
#define TEXINDEX_BUILDBG    19
#define TEXINDEX_UNKNOWN    20
#define TEXINDEX_SLEEVE1    21
#define TEXINDEX_SLEEVE2    22
#define TEXINDEX_AVATAR1    23
#define TEXINDEX_AVATAR2    24
#define TEXINDEX_LPFRAME1	25
#define TEXINDEX_LPFRAME2	26
#define TEXINDEX_LPBAR1		27
#define TEXINDEX_LPBAR2		28


#define LAYOUT_STATIC	0
#define LAYOUT_LP		1
#define LAYOUT_TEXT		2
#define LAYOUT_PHASE	3
#define LAYOUT_BUTTON	4

namespace ygopro
{
    
    struct SrcImageInfo {
        SrcImageInfo() : t_index(0), t_width(0), t_height(0) {}
        wxImage img;
        unsigned int t_index;
        unsigned int t_width;
        unsigned int t_height;
    };
    
	struct TextureInfo {
		TextureInfo() : index(0), lx(0.0), ly(0.0), rx(0.0), ry(0.0) {}
		unsigned int index;
		double lx;
		double ly;
		double rx;
		double ry;
	};
    
	struct LayoutInfo {
		int style;
		double x1;
		double y1;
		double x2;
		double y2;
		double x3;
		double y3;
		double x4;
		double y4;
		wxString click;
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
		void UnloadAllCardTexture();
		void LoadTexture(SrcImageInfo& img);
		
		void LoadSingleImage(const std::string& name, const wxString& file);
		void LoadImageConfig(const wxString& file);
		
        std::unordered_map<std::string, SrcImageInfo> src_images;
        std::unordered_map<unsigned int, SrcImageInfo> card_images;
		std::unordered_map<unsigned int, TextureInfo> card_textures;
		std::unordered_map<std::string, TextureInfo> textures;
        std::vector<TextureInfo> text_texture;
		std::vector<LayoutInfo> layouts;
		std::vector<LayoutInfo> clickable;
	};

	extern ImageMgr imageMgr;

}

#endif
