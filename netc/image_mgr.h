#ifndef _IMAGE_MGR_H_
#define _IMAGE_MGR_H_

#include <vector>
#include <list>
#include <unordered_map>

#include <GL/glew.h>
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
        TextureInfo& LoadBigCardTexture(unsigned int id);
        TextureInfo& GetTexture(const std::string& name);
        void UnloadCardTexture(unsigned int id);
		void UnloadAllCardTexture();
        
        unsigned short AllocBlock();
        bool FreeBlock(unsigned short);
        
		void InitTextures();
        
		bool LoadImageConfig(const std::string& file);
		
    protected:
        std::unordered_map<unsigned int, CardTextureInfo> card_textures;
        std::unordered_map<std::string, TextureInfo> misc_textures;
        std::list<unsigned short> unuse_block;
        std::vector<int> ref_count;
        sf::Texture card_texture;
        sf::Texture misc_texture;
        sf::Texture bg_texture;
        sf::Texture card_big_image;
        sf::RenderTexture temp_texture;
	};

    extern ImageMgr imageMgr;

}

#endif
