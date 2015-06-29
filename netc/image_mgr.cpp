#include "utils/common.h"
#include "utils/jaweson.h"
#include "utils/filesystem.h"

#include "image_mgr.h"
#include "scene_mgr.h"

namespace ygopro
{

	ti4& ImageMgr::GetCardTexture(uint32_t id) {
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
				cti.ti = misc_textures["unknown"];
                cti.ref_block = 0xffff;
			} else {
                uint16_t blockid = AllocBlock(id);
                if(blockid == 0xffff) {
                    cti.ti = misc_textures["unknown"];
                    cti.ref_block = 0xffff;
                } else {
                    base::Image img;
                    auto fileinfo = imageZip.ReadFile(file);
                    if(img.LoadMemory(fileinfo.first, (uint32_t)length)) {
                        base::v2ct frame_verts[4];
                        int32_t bx = (blockid % 20) * 100;
                        int32_t by = (blockid / 20) * 145;
                        int32_t bw = 100;
                        int32_t bh = 145;
                        cti.ti.vert[0] = {(float)(bx) / 2048, (float)(by) / 2048};
                        cti.ti.vert[1] = {(float)(bx + bw) / 2048, (float)(by) / 2048};
                        cti.ti.vert[2] = {(float)(bx) / 2048, (float)(by + bh) / 2048};
                        cti.ti.vert[3] = {(float)(bx + bw) / 2048, (float)(by + bh) / 2048};
                        cti.ref_block = blockid;
                        card_image.Load(img.GetRawData(), img.GetWidth(), img.GetHeight());
                        glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
                        glViewport(0, 0, 2048, 2048);
                        frame_verts[0].vertex = {(float)(bx) / 1024 - 1.0f, (float)(by) / 1024 - 1.0f};
                        frame_verts[1].vertex = {(float)(bx + bw) / 1024 - 1.0f, (float)(by) / 1024 - 1.0f};
                        frame_verts[2].vertex = {(float)(bx) / 1024 - 1.0f, (float)(by + bh) / 1024 - 1.0f};
                        frame_verts[3].vertex = {(float)(bx + bw) / 1024 - 1.0f, (float)(by + bh) / 1024 - 1.0f};
                        frame_verts[0].texcoord = {0.0f, 0.0f};
                        frame_verts[1].texcoord = {(float)img.GetWidth() / card_image.GetWidth(), 0.0f};
                        frame_verts[2].texcoord = {0.0f, (float)img.GetHeight() / card_image.GetHeight()};
                        frame_verts[3].texcoord = {frame_verts[1].texcoord.x, frame_verts[2].texcoord.y};
                        card_image.Bind();
                        glBindBuffer(GL_ARRAY_BUFFER, card_buffer[0]);
                        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(base::v2ct) * 4, &frame_verts);
                        glBindVertexArray(card_vao);
                        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
                        glBindVertexArray(0);
                        glBindTexture(GL_TEXTURE_2D, 0);
                        glBindFramebuffer(GL_FRAMEBUFFER, 0);
                    } else {
                        FreeBlock(blockid, false);
                        cti.ti = misc_textures["unknown"];
                        cti.ref_block = 0xffff;
                    }
                }
			}
            return cti.ti;;
		}
        IncreaseRef(iter->second.ref_block);
		return iter->second.ti;
	}
    
    ti4& ImageMgr::GetTexture(const std::string& name) {
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
    
    ti4& ImageMgr::GetCharTex(wchar_t ch) {
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
        card_texture.Load(nullptr, 2048, 2048);
        for(int16_t i = 7; i < 280; ++i)
            unused_block.push_back(i);
        ref_count.resize(280);
		std::vector<std::wstring> image_files;
		FileSystem::TraversalDir(image_path, [&image_path, &image_files](const std::wstring& name, bool isdir) {
			if(!isdir && name.find(L".zip") == (name.size() - 4)) {
                auto path = image_path;
                image_files.push_back(path.append(L"/").append(name));
            }
		});
        std::sort(image_files.begin(), image_files.end());
        imageZip.Load(image_files);
    }

    void ImageMgr::UninitTextures() {
        card_texture.Unload();
        misc_texture.Unload();
        bg_texture.Unload();
        card_image.Unload();
    }
    
	bool ImageMgr::LoadImageConfig(const std::string& name) {
        if(!FileSystem::IsFileExists(name))
            return false;
        TextFile f(name);
        rapidxml::xml_document<> doc;
        doc.parse<0>(f.Data());
        rapidxml::xml_node<>* root = doc.first_node();
        rapidxml::xml_node<>* config_node = root->first_node();
        while(config_node) {
            std::string config_name = config_node->name();
            rapidxml::xml_attribute<>* attr = config_node->first_attribute();
            if(config_name == "image") {
                std::string name = attr->value();
                attr = attr->next_attribute();
                std::string path = attr->value();
                if(FileSystem::IsFileExists(path)) {
                    base::Image img;
                    if(img.LoadFile(path)) {
                        if(name == "card")
                            card_texture.Update(img.GetRawData(), 0, 0, img.GetWidth(), img.GetHeight());
                        else if(name == "misc")
                            misc_texture.Load(img.GetRawData(), img.GetWidth(), img.GetHeight());
                        else if(name == "bg")
                            bg_texture.Load(img.GetRawData(), img.GetWidth(), img.GetHeight());
                    }
                }
            } else if(config_name == "texture") {
                base::Texture* ptex = nullptr;
                std::string src = attr->value();
                attr = attr->next_attribute();
                std::string name = attr->value();
                if(src == "card")
                    ptex = &card_texture;
                else if(src == "misc")
                    ptex = &misc_texture;
                else if(src == "bg")
                    ptex = &bg_texture;
                if(ptex) {
                    auto& ti = misc_textures[name];
                    attr = attr->next_attribute();
                    int32_t x = To<int32_t>(attr->value());
                    attr = attr->next_attribute();
                    int32_t y = To<int32_t>(attr->value());
                    attr = attr->next_attribute();
                    int32_t w = To<int32_t>(attr->value());
                    attr = attr->next_attribute();
                    int32_t h = To<int32_t>(attr->value());
                    ti.vert[0].x = (float)x / ptex->GetWidth();
                    ti.vert[0].y = (float)y / ptex->GetHeight();
                    ti.vert[1].x = (float)(x + w) / ptex->GetWidth();
                    ti.vert[1].y = (float)y / ptex->GetHeight();
                    ti.vert[2].x = (float)x / ptex->GetWidth();
                    ti.vert[2].y = (float)(y + h) / ptex->GetHeight();
                    ti.vert[3].x = (float)(x + w) / ptex->GetWidth();
                    ti.vert[3].y = (float)(y + h) / ptex->GetHeight();
                }
            } else if(config_name == "points") {
                base::Texture* ptex = nullptr;
                std::string src = attr->value();
                attr = attr->next_attribute();
                std::string name = attr->value();
                if(src == "card")
                    ptex = &card_texture;
                else if(src == "misc")
                    ptex = &misc_texture;
                else if(src == "bg")
                    ptex = &bg_texture;
                if(ptex) {
                    auto& ti = misc_textures[name];
                    int32_t val[8];
                    for(int32_t i = 0; i < 8; ++i) {
                        attr = attr->next_attribute();
                        val[i] = To<int32_t>(attr->value());
                    }
                    ti.vert[0].x = (float)val[0] / ptex->GetWidth();
                    ti.vert[0].y = (float)val[1] / ptex->GetHeight();
                    ti.vert[1].x = (float)val[2] / ptex->GetWidth();
                    ti.vert[1].y = (float)val[3] / ptex->GetHeight();
                    ti.vert[2].x = (float)val[4] / ptex->GetWidth();
                    ti.vert[2].y = (float)val[5] / ptex->GetHeight();
                    ti.vert[3].x = (float)val[6] / ptex->GetWidth();
                    ti.vert[3].y = (float)val[7] / ptex->GetHeight();
                }
            }
            config_node = config_node->next_sibling();
        }
        auto& char_tex = misc_textures["char"];
        float difx = (char_tex.vert[1].x - char_tex.vert[0].x) / 4;
        float dify = (char_tex.vert[2].y - char_tex.vert[0].y) / 4;
        for(int32_t i = 0; i < 16; ++i) {
            int32_t x = i % 4;
            int32_t y = i / 4;
            char_textures[i].vert[0] = char_tex.vert[0] + v2f{difx * x, dify * y};
            char_textures[i].vert[1] = char_tex.vert[0] + v2f{difx * (x + 1), dify * y};
            char_textures[i].vert[2] = char_tex.vert[0] + v2f{difx * x, dify * (y + 1)};
            char_textures[i].vert[3] = char_tex.vert[0] + v2f{difx * (x + 1), dify * (y + 1)};
        }
        return true;
	}

}
