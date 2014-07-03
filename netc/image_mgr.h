#ifndef _IMAGE_MGR_H_
#define _IMAGE_MGR_H_

#include "wx/image.h"
#include "wx/xml/xml.h"
#include <vector>
#include <list>
#include <unordered_map>

namespace ygopro
{
    
    struct SrcImageInfo {
        wxImage img;
        unsigned int t_index = 0;
        unsigned int t_width = 0;
        unsigned int t_height = 0;
    };
    
	struct TextureInfo {
		SrcImageInfo* src = nullptr;
		double lx = 0.0;
		double ly = 0.0;
		double rx = 0.0;
		double ry = 0.0;
        inline unsigned int tex() {
            return src ? src->t_index : 0;
        }
	};
    
    struct CardTextureInfo {
        TextureInfo ti;
        unsigned int ref_count = 0;
        bool is_system = false;
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
	};

    extern ImageMgr imageMgr;

}

#endif
