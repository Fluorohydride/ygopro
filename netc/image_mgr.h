#ifndef _IMAGE_MGR_H_
#define _IMAGE_MGR_H_

#include "base/render_base.h"
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
        base::Texture* LoadBigCardTexture(uint32_t id);
        base::Texture* GetRawMiscTexture() { return &misc_texture; }
        base::Texture* GetRawCardTexture() { return &card_texture; }
        base::Texture* GetRawBGTexture() { return &bg_texture; }
        base::Texture* GetRawCardImage() { return &card_image; }
        ti4& GetCharTex(wchar_t ch);
        void UnloadCardTexture(uint32_t id);
		void UnloadAllCardTexture();
        
        uint16_t AllocBlock(uint32_t iid);
        bool FreeBlock(uint16_t, bool reserve);
        void IncreaseRef(uint16_t);
        
		void InitTextures(const std::wstring& image_path);
        void UninitTextures();
		bool LoadImageConfig(const std::wstring& file);
        
    protected:
        std::unordered_map<uint32_t, CardTextureInfo> card_textures;
        std::unordered_map<std::string, ti4> misc_textures;
        std::array<ti4, 16> char_textures;
        std::list<uint16_t> unused_block;
        std::list<uint16_t> reserved_block;
        std::vector<std::pair<int32_t, uint32_t>> ref_count;
        std::unordered_map<uint32_t, std::list<uint16_t>::iterator> reserved_id;
        base::Texture card_texture;
        base::Texture misc_texture;
        base::Texture bg_texture;
        base::Texture card_image;
        uint32_t frame_buffer = 0;
        uint32_t card_buffer[2] = {0, 0};
        uint32_t card_vao = 0;
        ZipArchive imageZip;
	};

}

#endif
