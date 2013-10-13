#ifndef _IMAGE_MGR_H_
#define _IMAGE_MGR_H_

#include <GL/glew.h>
#include "wx/image.h"
#include "wx/xml/xml.h"
#include <array>
#include <vector>
#include <unordered_map>

#define LAYOUT_STATIC	0
#define LAYOUT_LP		1
#define LAYOUT_TEXT		2
#define LAYOUT_PHASE	3
#define LAYOUT_BUTTON	4

namespace ygopro
{
    
    struct SrcImageInfo {
        wxImage img;
        unsigned int t_index;
        unsigned int t_width;
        unsigned int t_height;
    };
    
	struct TextureInfo {
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
