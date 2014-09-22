#ifndef _IMAGE_MGR_H_
#define _IMAGE_MGR_H_

#include "glbase.h"
#include "zip_archive.h"

namespace ygopro
{
    
    struct CardTextureInfo {
		ti4 ti;
        unsigned short ref_block = 0;
	};
    
	class ImageMgr : public Singleton<ImageMgr> {

	public:
		ti4& GetCardTexture(unsigned int id);
        ti4& GetTexture(const std::string& name);
        glbase::Texture* LoadBigCardTexture(unsigned int id);
        glbase::Texture* GetRawMiscTexture() { return &misc_texture; }
        glbase::Texture* GetRawCardTexture() { return &card_texture; }
        glbase::Texture* GetRawBGTexture() { return &bg_texture; }
        glbase::Texture* GetRawCardImage() { return &card_image; }
        ti4& GetCharTex(wchar_t ch);
        void UnloadCardTexture(unsigned int id);
		void UnloadAllCardTexture();
        
        unsigned short AllocBlock();
        bool FreeBlock(unsigned short);
        
		void InitTextures(const std::wstring& image_path);
        void UninitTextures();
		bool LoadImageConfig(const std::wstring& file);
        
    protected:
        std::unordered_map<unsigned int, CardTextureInfo> card_textures;
        std::unordered_map<std::string, ti4> misc_textures;
        std::array<ti4, 16> char_textures;
        std::list<unsigned short> unuse_block;
        std::vector<int> ref_count;
        glbase::Texture card_texture;
        glbase::Texture misc_texture;
        glbase::Texture bg_texture;
        glbase::Texture card_image;
        unsigned int frame_buffer = 0;
        unsigned int card_buffer[2] = {0, 0};
        unsigned int card_vao = 0;
        ZipArchive imageZip;
	};

}

#endif
