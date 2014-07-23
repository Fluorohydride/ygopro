#ifndef _IMAGE_MGR_H_
#define _IMAGE_MGR_H_

#include <vector>
#include <list>
#include <unordered_map>

#include "glbase.h"
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

namespace ygopro
{
    
    template<int TCOUNT>
	struct TextureInfo {
        glbase::vector2<float> vert[TCOUNT];
	};
    
    struct CardTextureInfo {
		TextureInfo<4> ti;
        unsigned short ref_block = 0;
	};
    
	class ImageMgr {

	public:
		CardTextureInfo& GetCardTexture(unsigned int id);
        TextureInfo<4>& LoadBigCardTexture(unsigned int id);
        TextureInfo<4>& GetTexture(const std::string& name);
        void UnloadCardTexture(unsigned int id);
		void UnloadAllCardTexture();
        
        unsigned short AllocBlock();
        bool FreeBlock(unsigned short);
        
		void InitTextures();
        void UninitTextures();
        void BindTexture(int textype);
		bool LoadImageConfig(const std::wstring& file);
        
    protected:
        std::unordered_map<unsigned int, CardTextureInfo> card_textures;
        std::unordered_map<std::string, TextureInfo<4>> misc_textures;
        std::list<unsigned short> unuse_block;
        std::vector<int> ref_count;
        glbase::Texture card_texture;
        glbase::Texture misc_texture;
        glbase::Texture bg_texture;
        glbase::Texture card_image;
        unsigned int frame_buffer = 0;
        unsigned int card_buffer[2] = {0, 0};
	};

    extern ImageMgr imageMgr;

}

#endif
