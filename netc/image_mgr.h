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
    
	struct TextureInfo {
		long x = 0;
		long y = 0;
		long w = 0;
		long h = 0;
	};
    
    struct CardTextureInfo {
		TextureInfo ti;
        unsigned short ref_block = 0;
	};
    
	class ImageMgr {

	public:
		CardTextureInfo& GetCardTexture(unsigned int id);
        TextureInfo& LoadBigCardTexture(unsigned int id);
        TextureInfo& GetTexture(const std::string& name);
        void UnloadCardTexture(unsigned int id);
		void UnloadAllCardTexture();
        
        unsigned short AllocBlock();
        bool FreeBlock(unsigned short);
        
		void InitTextures();
        void UninitTextures();
        void BindTexture(int textype);
		bool LoadImageConfig(const std::string& file);
        glbase::Texture& GetTexInfo(int textype);
        
    protected:
        std::unordered_map<unsigned int, CardTextureInfo> card_textures;
        std::unordered_map<std::string, TextureInfo> misc_textures;
        std::list<unsigned short> unuse_block;
        std::vector<int> ref_count;
        glbase::Texture card_texture;
        glbase::Texture misc_texture;
        glbase::Texture bg_texture;
        glbase::Texture card_image;
        unsigned int frame_buffer = 0;
        unsigned short frame_index[6] = {0, 2, 1, 1, 2, 3};
	};

    extern ImageMgr imageMgr;

}

#endif
