#include "../common/common.h"

#include <wx/xml/xml.h>
#include <wx/wfstream.h>

#include "image_mgr.h"
#include "scene_mgr.h"

namespace ygopro
{

	ti4& ImageMgr::GetCardTexture(unsigned int id) {
		auto iter = card_textures.find(id);
		if(iter == card_textures.end()) {
            std::string file = wxString::Format("%d.jpg", id).ToUTF8().data();
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
                    delete imgbuf;
                }
			}
            return cti.ti;;
		}
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
        std::string file = wxString::Format("%d.jpg", id).ToUTF8().data();
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
            delete imgbuf;
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
    
    void ImageMgr::InitTextures(const std::wstring& image_pack) {
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
        imageZip.Load(image_pack);
    }

    void ImageMgr::UninitTextures() {
        glDeleteFramebuffers(1, &frame_buffer);
        glDeleteBuffers(2, card_buffer);
        card_texture.Unload();
        misc_texture.Unload();
        bg_texture.Unload();
        card_image.Unload();
    }
    
	bool ImageMgr::LoadImageConfig(const std::wstring& name) {
		wxXmlDocument doc;
        if(!wxFileExists(name))
            return false;
		if(!doc.Load(name, "UTF-8", wxXMLDOC_KEEP_WHITESPACE_NODES))
			return false;
        wxXmlNode* root = doc.GetRoot();
		wxXmlNode* child = root->GetChildren();
		while (child) {
            if (child->GetName() == "image") {
                wxString name = child->GetAttribute("name");
                wxString path = child->GetAttribute("path");
                if(wxFileExists(path)) {
                    glbase::Image img;
                    if(img.LoadFile(path.ToUTF8().data())) {
                        if(name == "card")
                            card_texture.Update(img.GetRawData(), 0, 0, img.GetWidth(), img.GetHeight());
                        else if(name == "misc")
                            misc_texture.Load(img.GetRawData(), img.GetWidth(), img.GetHeight());
                        else if(name == "bg")
                            bg_texture.Load(img.GetRawData(), img.GetWidth(), img.GetHeight());
                    }
                }
            } else if (child->GetName() == "texture") {
				std::string name = child->GetAttribute("name").ToUTF8().data();
                std::string src = child->GetAttribute("src").ToUTF8().data();
                glbase::Texture* ptex = nullptr;
                if(src == "card")
                    ptex = &card_texture;
                else if(src == "misc")
                    ptex = &misc_texture;
                else if(src == "bg")
                    ptex = &bg_texture;
                if(ptex) {
                    auto& ti = misc_textures[name];
                    int x = To<int>(child->GetAttribute("x").ToUTF8().data());
                    int y = To<int>(child->GetAttribute("y").ToUTF8().data());
                    int w = To<int>(child->GetAttribute("w").ToUTF8().data());
                    int h = To<int>(child->GetAttribute("h").ToUTF8().data());
                    ti.vert[0].x = (float)x / ptex->GetWidth();
                    ti.vert[0].y = (float)y / ptex->GetHeight();
                    ti.vert[1].x = (float)(x + w) / ptex->GetWidth();
                    ti.vert[1].y = (float)y / ptex->GetHeight();
                    ti.vert[2].x = (float)x / ptex->GetWidth();
                    ti.vert[2].y = (float)(y + h) / ptex->GetHeight();
                    ti.vert[3].x = (float)(x + w) / ptex->GetWidth();
                    ti.vert[3].y = (float)(y + h) / ptex->GetHeight();
                }
			} else if (child->GetName() == "points") {
				std::string name = child->GetAttribute("name").ToUTF8().data();
                std::string src = child->GetAttribute("src").ToUTF8().data();
                glbase::Texture* ptex = nullptr;
                if(src == "card")
                    ptex = &card_texture;
                else if(src == "misc")
                    ptex = &misc_texture;
                else if(src == "bg")
                    ptex = &bg_texture;
                if(ptex) {
                    auto& ti = misc_textures[name];
                    int val[8];
                    val[0] = To<int>(child->GetAttribute("x1").ToUTF8().data());
                    val[1] = To<int>(child->GetAttribute("y1").ToUTF8().data());
                    val[2] = To<int>(child->GetAttribute("x2").ToUTF8().data());
                    val[3] = To<int>(child->GetAttribute("y2").ToUTF8().data());
                    val[4] = To<int>(child->GetAttribute("x3").ToUTF8().data());
                    val[5] = To<int>(child->GetAttribute("y3").ToUTF8().data());
                    val[6] = To<int>(child->GetAttribute("x4").ToUTF8().data());
                    val[7] = To<int>(child->GetAttribute("y4").ToUTF8().data());
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
			child = child->GetNext();
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
