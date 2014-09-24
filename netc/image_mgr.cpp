#include "../common/common.h"
#include "../common/filesystem.h"

#include "../buildin/rapidxml.hpp"
#include "../buildin/rapidxml_print.hpp"

#include "image_mgr.h"
#include "scene_mgr.h"

namespace ygopro
{

	ti4& ImageMgr::GetCardTexture(unsigned int id) {
		auto iter = card_textures.find(id);
		if(iter == card_textures.end()) {
            std::string file = To<std::string>(id).append(".jpg");
			auto& cti = card_textures[id];
            int length = imageZip.GetFileLength(file);
			if(length == 0) {
				cti.ti = misc_textures["unknown"];
                cti.ref_block = 0xffff;
			} else {
                unsigned short blockid = AllocBlock();
                if(blockid == 0xffff) {
                    cti.ti = misc_textures["unknown"];
                    cti.ref_block = 0xffff;
                } else {
                    glbase::Image img;
                    unsigned char* imgbuf = new unsigned char[length];
                    imageZip.ReadFile(file, imgbuf);
                    if(img.LoadMemory(imgbuf, length)) {
                        glbase::v2ct frame_verts[4];
                        int bx = (blockid % 20) * 100;
                        int by = (blockid / 20) * 145;
                        int bw = 100;
                        int bh = 145;
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
                        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glbase::v2ct) * 4, &frame_verts);
                        glBindVertexArray(card_vao);
                        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
                        glBindVertexArray(0);
                        glBindTexture(GL_TEXTURE_2D, 0);
                        glBindFramebuffer(GL_FRAMEBUFFER, 0);
                    } else {
                        FreeBlock(blockid);
                        cti.ti = misc_textures["unknown"];
                        cti.ref_block = 0xffff;
                    }
                    delete[] imgbuf;
                }
			}
            return cti.ti;;
		}
		if(iter->second.ref_block < 280)
			ref_count[iter->second.ref_block]++;
		return iter->second.ti;
	}
    
    ti4& ImageMgr::GetTexture(const std::string& name) {
        return misc_textures[name];
    }
    
    glbase::Texture* ImageMgr::LoadBigCardTexture(unsigned int id) {
        static unsigned int pid = 0;
        static glbase::Texture* pre_ret = nullptr;
        if(pid == id)
            return pre_ret;
        std::string file = To<std::string>("%d.jpg", id);
        int length = imageZip.GetFileLength(file);
        if(length == 0) {
            file = "unknown.jpg";
            length = imageZip.GetFileLength(file);
        }
        if(length != 0) {
            glbase::Image img;
            unsigned char* imgbuf = new unsigned char[length];
            imageZip.ReadFile(file, imgbuf);
            if(img.LoadMemory(imgbuf, length)) {
                card_image.Load(img.GetRawData(), img.GetWidth(), img.GetHeight());
                pre_ret = &card_image;
            } else
                pre_ret = nullptr;
            delete[] imgbuf;
        } else
            pre_ret = nullptr;
        return pre_ret;
    }
    
    ti4& ImageMgr::GetCharTex(wchar_t ch) {
        if(ch < L'*' || ch > L'9')
            return char_textures[2];
        return char_textures[ch - L'*'];
    }
    
    void ImageMgr::UnloadCardTexture(unsigned int id) {
        auto iter = card_textures.find(id);
        if(iter == card_textures.end())
            return;
        auto& cti = iter->second;
        if(FreeBlock(cti.ref_block))
            card_textures.erase(iter);
    }
    
	void ImageMgr::UnloadAllCardTexture() {
        card_textures.clear();
        unuse_block.clear();
        for(short i = 7; i < 280; ++i) {
            ref_count[i] = 0;
            unuse_block.push_back(i);
        }
	}

    unsigned short ImageMgr::AllocBlock() {
        if(unuse_block.size() == 0)
            return 0xffff;
        short ret = unuse_block.front();
        unuse_block.pop_front();
        ref_count[ret]++;
        return ret;
    }
    
    bool ImageMgr::FreeBlock(unsigned short id) {
        if(id >= 280)
            return false;
        if(ref_count[id] == 0)
            return false;
        ref_count[id]--;
        if(ref_count[id] == 0) {
            unuse_block.push_back(id);
            return true;
        }
        return false;
    }
    
    void ImageMgr::InitTextures(const std::wstring& image_path) {
        card_texture.Load(nullptr, 2048, 2048);
        for(short i = 7; i < 280; ++i)
            unuse_block.push_back(i);
        ref_count.resize(280);
        glGenFramebuffers(1, &frame_buffer);
        glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, card_texture.GetTextureId(), 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glGenBuffers(2, card_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, card_buffer[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glbase::v2ct) * 4, nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        unsigned short index[] = {0, 2, 1, 1, 2, 3};
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, card_buffer[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glGenVertexArrays(1, &card_vao);
        glBindVertexArray(card_vao);
        glBindBuffer(GL_ARRAY_BUFFER, card_buffer[0]);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glbase::v2ct), 0);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(glbase::v2ct), (const GLvoid*)glbase::v2ct::color_offset);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glbase::v2ct), (const GLvoid*)glbase::v2ct::tex_offset);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, card_buffer[1]);
        glBindVertexArray(0);
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
        glDeleteFramebuffers(1, &frame_buffer);
        glDeleteBuffers(2, card_buffer);
        glDeleteVertexArrays(1, &card_vao);
        card_texture.Unload();
        misc_texture.Unload();
        bg_texture.Unload();
        card_image.Unload();
    }
    
	bool ImageMgr::LoadImageConfig(const std::wstring& name) {
        if(!FileSystem::IsFileExists(name))
            return false;
        TextFile f(To<std::string>(name));
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
                    glbase::Image img;
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
                glbase::Texture* ptex = nullptr;
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
                    int x = To<int>(attr->value());
                    attr = attr->next_attribute();
                    int y = To<int>(attr->value());
                    attr = attr->next_attribute();
                    int w = To<int>(attr->value());
                    attr = attr->next_attribute();
                    int h = To<int>(attr->value());
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
                glbase::Texture* ptex = nullptr;
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
                    int val[8];
                    for(int i = 0; i < 8; ++i) {
                        attr = attr->next_attribute();
                        val[i] = To<int>(attr->value());
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
        for(int i = 0; i < 16; ++i) {
            int x = i % 4;
            int y = i / 4;
            char_textures[i].vert[0] = char_tex.vert[0] + v2f{difx * x, dify * y};
            char_textures[i].vert[1] = char_tex.vert[0] + v2f{difx * (x + 1), dify * y};
            char_textures[i].vert[2] = char_tex.vert[0] + v2f{difx * x, dify * (y + 1)};
            char_textures[i].vert[3] = char_tex.vert[0] + v2f{difx * (x + 1), dify * (y + 1)};
        }
        return true;
	}

}
