#ifndef _IMAGE_MGR_H_
#define _IMAGE_MGR_H_

#include "glbase.h"
#include "zip_archive.h"

namespace ygopro
{
    
    struct CardTextureInfo {
		ti4 ti;
        uint16_t ref_block = 0;
	};
    
	class ImageMgr : public Singleton<ImageMgr> {

	public:
		ti4& GetCardTexture(uint32_t id);
        ti4& GetTexture(const std::string& name);
        glbase::Texture* LoadBigCardTexture(uint32_t id);
        glbase::Texture* GetRawMiscTexture() { return &misc_texture; }
        glbase::Texture* GetRawCardTexture() { return &card_texture; }
        glbase::Texture* GetRawBGTexture() { return &bg_texture; }
        glbase::Texture* GetRawCardImage() { return &card_image; }
        ti4& GetCharTex(wchar_t ch);
        void UnloadCardTexture(uint32_t id);
		void UnloadAllCardTexture();
        
        uint16_t AllocBlock();
        bool FreeBlock(uint16_t);
        
		void InitTextures(const std::wstring& image_path);
        void UninitTextures();
		bool LoadImageConfig(const std::wstring& file);
        
    protected:
        std::unordered_map<uint32_t, CardTextureInfo> card_textures;
        std::unordered_map<std::string, ti4> misc_textures;
        std::array<ti4, 16> char_textures;
        std::list<uint16_t> unuse_block;
        std::vector<int32_t> ref_count;
        glbase::Texture card_texture;
        glbase::Texture misc_texture;
        glbase::Texture bg_texture;
        glbase::Texture card_image;
        uint32_t frame_buffer = 0;
        uint32_t card_buffer[2] = {0, 0};
        uint32_t card_vao = 0;
        ZipArchive imageZip;
	};

}

#endif
