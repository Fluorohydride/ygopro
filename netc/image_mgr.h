#ifndef _IMAGE_MGR_H_
#define _IMAGE_MGR_H_

#include "wx/image.h"
#include "wx/xml/xml.h"
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
        SrcImageInfo(): t_index(0), t_width(0), t_height(0) {}
        wxImage img;
        unsigned int t_index;
        unsigned int t_width;
        unsigned int t_height;
    };
    
	struct TextureInfo {
        TextureInfo(): src(nullptr), lx(0.0), ly(0.0), rx(0.0), ry(0.0) {}
		SrcImageInfo* src;
		double lx;
		double ly;
		double rx;
		double ry;
        inline unsigned int tex() {
            return src ? src->t_index : 0;
        }
	};
    
    struct CardTextureInfo {
        CardTextureInfo(): ref_count(0), is_system(false) {}
        TextureInfo ti;
        unsigned int ref_count;
        bool is_system;
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

		CardTextureInfo& GetCardTexture(unsigned int id);
		CardTextureInfo& ReloadCardTexture(unsigned int id);
        void UnloadCardTexture(unsigned int id);
		void UnloadAllCardTexture();
		void LoadTexture(SrcImageInfo& img);
		void InitTextures(bool force = false);
        
		void LoadSingleImage(const std::string& name, const wxString& file);
		bool LoadImageConfig(const wxString& file);
		
        std::unordered_map<std::string, SrcImageInfo> src_images;
        std::unordered_map<unsigned int, SrcImageInfo> card_images;
		std::unordered_map<unsigned int, CardTextureInfo> card_textures;
		std::unordered_map<std::string, TextureInfo> textures;
        std::unordered_map<std::string, wxString> single_images;
        std::vector<TextureInfo> text_texture;
		std::vector<LayoutInfo> layouts;
		std::vector<LayoutInfo> clickable;
	};

	extern ImageMgr imageMgr;

}

#endif
