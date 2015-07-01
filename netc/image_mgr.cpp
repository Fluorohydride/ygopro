#include "utils/common.h"
#include "utils/jaweson.h"
#include "utils/filesystem.h"

#include "config.h"

#include "image_mgr.h"
#include "scene_mgr.h"

namespace ygopro
{

	texi4& ImageMgr::GetCardTexture(uint32_t id) {
		auto iter = card_textures.find(id);
		if(iter == card_textures.end()) {
            std::string file = To<std::string>("%d.jpg", id);
			auto& cti = card_textures[id];
            size_t length = imageZip.GetFileLength(file);
            if(length == 0) {
                file = To<std::string>("%d.png", id);
                length = imageZip.GetFileLength(file);
            }
			if(length == 0) {
				cti.tex_info = misc_textures["unknown"];
                cti.ref_block = 0xffff;
			} else {
                uint16_t blockid = AllocBlock(id);
                if(blockid == 0xffff) {
                    cti.tex_info = misc_textures["unknown"];
                    cti.ref_block = 0xffff;
                } else {
                    base::Image img;
                    auto fileinfo = imageZip.ReadFile(file);
                    if(img.LoadMemory(fileinfo.first, (uint32_t)length)) {
                        int32_t bx = (blockid % 20) * 100;
                        int32_t by = (blockid / 20) * 145;
                        int32_t bw = 100;
                        int32_t bh = 145;
                        cti.tex_info.vert[0] = {bx, by};
                        cti.tex_info.vert[1] = {bx + bw, by};
                        cti.tex_info.vert[2] = {bx, by + bh};
                        cti.tex_info.vert[3] = {bx + bw, by + bh};
                        cti.ref_block = blockid;
                        card_image.Load(img.GetRawData(), img.GetWidth(), img.GetHeight());
                        image_render->AddVertices(&card_image, {bx, by, bw, bh}, {0, 0, card_image.GetImgWidth(), card_image.GetImgHeight()});
                    } else {
                        FreeBlock(blockid, false);
                        cti.tex_info = misc_textures["unknown"];
                        cti.ref_block = 0xffff;
                    }
                }
			}
            return cti.tex_info;;
		}
        IncreaseRef(iter->second.ref_block);
		return iter->second.tex_info;
	}
    
    texi4& ImageMgr::GetTexture(const std::string& name) {
        return misc_textures[name];
    }
    
    base::Texture* ImageMgr::LoadBigCardTexture(uint32_t id) {
        static uint32_t pid = 0;
        static base::Texture* pre_ret = nullptr;
        if(pid == id)
            return pre_ret;
        std::string file = To<std::string>("%d.jpg", id);
        size_t length = imageZip.GetFileLength(file);
        if(length == 0) {
            file = To<std::string>("%d.png", id);
            length = imageZip.GetFileLength(file);
        }
        if(length == 0) {
            file = "unknown.jpg";
            length = imageZip.GetFileLength(file);
        }
        if(length != 0) {
            base::Image img;
            auto imginfo = imageZip.ReadFile(file);
            if(img.LoadMemory(imginfo.first, (uint32_t)length)) {
                card_image.Load(img.GetRawData(), img.GetWidth(), img.GetHeight());
                pre_ret = &card_image;
            } else
                pre_ret = nullptr;
        } else
            pre_ret = nullptr;
        return pre_ret;
    }
    
    texi4& ImageMgr::GetCharTex(wchar_t ch) {
        if(ch < L'*' || ch > L'9')
            return char_textures[2];
        return char_textures[ch - L'*'];
    }
    
    void ImageMgr::UnloadCardTexture(uint32_t id) {
        auto iter = card_textures.find(id);
        if(iter == card_textures.end())
            return;
        auto& cti = iter->second;
        if(FreeBlock(cti.ref_block, true))
            card_textures.erase(iter);
    }
    
	void ImageMgr::UnloadAllCardTexture() {
        for(int16_t i = 7; i < 280; ++i) {
            if(ref_count[i].first != 0) {
                ref_count[i].first = 0;
                reserved_block.push_back(i);
                reserved_id[ref_count[i].second] = --reserved_block.end();
            }
        }
	}

    uint16_t ImageMgr::AllocBlock(uint32_t iid) {
        uint16_t ret = 0;
        if(unused_block.size() == 0) {
            if(reserved_block.size() == 0)
                return 0xffff;
            uint16_t ret = reserved_block.front();
            reserved_id.erase(ref_count[ret].second);
            card_textures.erase(ref_count[ret].second);
            reserved_block.pop_front();
        } else {
            ret = unused_block.front();
            unused_block.pop_front();
        }
        ref_count[ret].first = 1;
        ref_count[ret].second = iid;
        return ret;
    }
    
    bool ImageMgr::FreeBlock(uint16_t id, bool reserve) {
        if(id >= 280)
            return false;
        if(ref_count[id].first == 0)
            return false;
        ref_count[id].first--;
        if(ref_count[id].first == 0) {
            if(reserve) {
                reserved_block.push_back(id);
                reserved_id[ref_count[id].second] = --reserved_block.end();
                return false;
            } else {
                unused_block.push_back(id);
                return true;
            }
        }
        return false;
    }
    
    void ImageMgr::IncreaseRef(uint16_t blockid) {
        if(blockid >= 280)
            return;
        if(ref_count[blockid].first == 0) {
            auto iter = reserved_id.find(ref_count[blockid].second);
            if(iter != reserved_id.end()) {
                reserved_block.erase(iter->second);
                reserved_id.erase(iter);
            }
        }
        ref_count[blockid].first++;
    }
    
    void ImageMgr::InitTextures(const std::string& image_path) {
        for(int16_t i = 7; i < 280; ++i)
            unused_block.push_back(i);
        ref_count.resize(280);
		std::vector<std::string> image_files;
		FileSystem::TraversalDir(image_path, [&image_path, &image_files](const std::string& name, bool isdir) {
			if(!isdir && name.find(".zip") == (name.size() - 4)) {
                auto path = image_path;
                image_files.push_back(path.append("/").append(name));
            }
		});
        std::sort(image_files.begin(), image_files.end());
        imageZip.Load(image_files);
        image_render = std::make_shared<base::SimpleTextureRenderer>();
        image_render->SetFlip(true);
        image_render->SetScreenSize({2048, 2048});
        image_render->SetShader(&base::Shader::GetDefaultShader());
        SetRenderObject(image_render.get());
    }

    void ImageMgr::UninitTextures() {
        misc_texture.Unload();
        bg_texture.Unload();
        card_image.Unload();
    }
    
	bool ImageMgr::LoadImageConfig() {
        textureCfg["image"].for_each([this](const std::string& name, jaweson::JsonNode<>& node) {
            std::string path = node.to_string();
            if(FileSystem::IsFileExists(path)) {
                base::Image img;
                if(img.LoadFile(path)) {
                    if(name == "card")
                        render_tex->Update(img.GetRawData(), 0, 0, img.GetWidth(), img.GetHeight());
                    else if(name == "misc")
                        misc_texture.Load(img.GetRawData(), img.GetWidth(), img.GetHeight());
                    else if(name == "bg")
                        bg_texture.Load(img.GetRawData(), img.GetWidth(), img.GetHeight());
                }
            }
        });
        textureCfg["texture"].for_each([this](const std::string& name, jaweson::JsonNode<>& node) {
            base::Texture* ptex = nullptr;
            std::string src = node[0].to_string();
            if(src == "card")
                ptex = render_tex;
            else if(src == "misc")
                ptex = &misc_texture;
            else if(src == "bg")
                ptex = &bg_texture;
            if(ptex) {
                auto& ti = misc_textures[name];
                int32_t x = (int32_t)node[1].to_integer();
                int32_t y = (int32_t)node[2].to_integer();
                int32_t w = (int32_t)node[3].to_integer();
                int32_t h = (int32_t)node[4].to_integer();
                ti.vert[0] = {x, y};
                ti.vert[1] = {x + w, y};
                ti.vert[2] = {x, y + h};
                ti.vert[3] = {x + w, y + h};
            }
        });
        textureCfg["points"].for_each([this](const std::string& name, jaweson::JsonNode<>& node) {
            base::Texture* ptex = nullptr;
            std::string src = node[0].to_string();
            if(src == "card")
                ptex = render_tex;
            else if(src == "misc")
                ptex = &misc_texture;
            else if(src == "bg")
                ptex = &bg_texture;
            if(ptex) {
                auto& ti = misc_textures[name];
                int32_t val[8];
                for(int32_t i = 0; i < 8; ++i)
                    val[i] = (int32_t)node[i + 1].to_integer();
                ti.vert[0] = {val[0], val[1]};
                ti.vert[1] = {val[2], val[3]};
                ti.vert[2] = {val[4], val[5]};
                ti.vert[3] = {val[6], val[7]};
            }
        });
        auto& char_tex = misc_textures["char"];
        int32_t difx = (char_tex.vert[1].x - char_tex.vert[0].x) / 4;
        int32_t dify = (char_tex.vert[2].y - char_tex.vert[0].y) / 4;
        for(int32_t i = 0; i < 16; ++i) {
            int32_t x = i % 4;
            int32_t y = i / 4;
            char_textures[i].vert[0] = char_tex.vert[0] + v2i{difx * x, dify * y};
            char_textures[i].vert[1] = char_tex.vert[0] + v2i{difx * (x + 1), dify * y};
            char_textures[i].vert[2] = char_tex.vert[0] + v2i{difx * x, dify * (y + 1)};
            char_textures[i].vert[3] = char_tex.vert[0] + v2i{difx * (x + 1), dify * (y + 1)};
        }
        return true;
	}

}
