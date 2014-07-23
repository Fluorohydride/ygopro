#include "image_mgr.h"
#include "scene_mgr.h"
#include "../common/common.h"

#include "wx/xml/xml.h"
#include "wx/wfstream.h"

namespace ygopro
{

	ImageMgr imageMgr;

	CardTextureInfo& ImageMgr::GetCardTexture(unsigned int id) {
		auto iter = card_textures.find(id);
		if(iter == card_textures.end()) {
			wxString file = wxString::Format("%ls/%d.jpg", static_cast<const std::wstring>(commonCfg[L"image_path"]).c_str(), id);
			auto& cti = card_textures[id];
			if(!wxFileExists(file)) {
				cti.ti = misc_textures["unknown"];
                cti.ref_block = 0xffff;
			} else {
                unsigned short blockid = AllocBlock();
                if(blockid == 0xffff) {
                    cti.ti = misc_textures["unknown"];
                    cti.ref_block = 0xffff;
                } else {
                    sf::Image img;
                    if(img.loadFromFile(file.ToStdString())) {
                        glbase::VertexVCT frame_verts[4];
                        int bx = (blockid % 20) * 100;
                        int by = (blockid / 20) * 145;
                        int bw = 100;
                        int bh = 145;
                        cti.ti.vert[0] = {(float)(bx) / 2048, (float)(by) / 2048};
                        cti.ti.vert[1] = {(float)(bx + bw) / 2048, (float)(by) / 2048};
                        cti.ti.vert[2] = {(float)(bx) / 2048, (float)(by + bh) / 2048};
                        cti.ti.vert[3] = {(float)(bx + bw) / 2048, (float)(by + bh) / 2048};
                        cti.ref_block = blockid;
                        card_image.Load(img.getPixelsPtr(), img.getSize().x, img.getSize().y);
                        glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
                        glViewport(0, 0, 2048, 2048);
                        frame_verts[0].vertex = {(float)(bx) / 1024 - 1.0f, (float)(by) / 1024 - 1.0f};
                        frame_verts[1].vertex = {(float)(bx + bw) / 1024 - 1.0f, (float)(by) / 1024 - 1.0f};
                        frame_verts[2].vertex = {(float)(bx) / 1024 - 1.0f, (float)(by + bh) / 1024 - 1.0f};
                        frame_verts[3].vertex = {(float)(bx + bw) / 1024 - 1.0f, (float)(by + bh) / 1024 - 1.0f};
                        frame_verts[0].texcoord = {0.0f, 0.0f};
                        frame_verts[1].texcoord = {(float)img.getSize().x / card_image.GetWidth(), 0.0f};
                        frame_verts[2].texcoord = {0.0f, (float)img.getSize().y / card_image.GetHeight()};
                        frame_verts[3].texcoord = {frame_verts[1].texcoord.x, frame_verts[2].texcoord.y};
                        card_image.Bind();
                        glBindBuffer(GL_ARRAY_BUFFER, card_buffer[0]);
                        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glbase::VertexVCT) * 4, &frame_verts);
                        glVertexPointer(2, GL_FLOAT, sizeof(glbase::VertexVCT), 0);
                        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(glbase::VertexVCT), (const GLvoid*)(uintptr_t)glbase::VertexVCT::color_offset);
                        glTexCoordPointer(2, GL_FLOAT, sizeof(glbase::VertexVCT), (const GLvoid*)(uintptr_t)(glbase::VertexVCT::tex_offset));
                        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, card_buffer[1]);
                        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
                        glBindTexture(GL_TEXTURE_2D, 0);
                        glBindFramebuffer(GL_FRAMEBUFFER, 0);
                    } else {
                        FreeBlock(blockid);
                        cti.ti = misc_textures["unknown"];
                        cti.ref_block = 0xffff;
                    }
                }
			}
            return cti;
		}
        ref_count[iter->second.ref_block]++;
		return iter->second;
	}

    TextureInfo<4>& ImageMgr::LoadBigCardTexture(unsigned int id) {
        static TextureInfo<4> ti;
        wxString file = wxString::Format("%ls/%d.jpg", (static_cast<const std::wstring&>(commonCfg[L"image_path"])).c_str(), id);
        sf::Image img;
        if(img.loadFromFile(file.ToStdString())) {
        }
        return ti;
    }
    
    TextureInfo<4>& ImageMgr::GetTexture(const std::string& name) {
        return misc_textures[name];
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
    
    void ImageMgr::InitTextures() {
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
        glBufferData(GL_ARRAY_BUFFER, sizeof(glbase::VertexVCT) * 4, nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        unsigned short index[] = {0, 2, 1, 1, 2, 3};
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, card_buffer[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void ImageMgr::UninitTextures() {
        glDeleteFramebuffers(1, &frame_buffer);
        glDeleteBuffers(2, card_buffer);
    }
    
    void ImageMgr::BindTexture(int textype) {
        switch(textype) {
            case 0:
                bg_texture.Bind();
                break;
            case 1:
                misc_texture.Bind();
                break;
            case 2:
                card_image.Bind();
                break;
            case 3:
                card_texture.Bind();
                break;
            default:
                misc_texture.Bind();
                break;
        }
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
                    sf::Image img;
                    if(img.loadFromFile(path.ToStdString())) {
                        if(name == "cards")
                            card_texture.Update(img.getPixelsPtr(), 0, 0, img.getSize().x, img.getSize().y);
                        else if(name == "misc")
                            misc_texture.Load(img.getPixelsPtr(), img.getSize().x, img.getSize().y);
                        else if(name == "bg")
                            bg_texture.Load(img.getPixelsPtr(), img.getSize().x, img.getSize().y);
                    }
                }
            } else if (child->GetName() == "texture") {
				std::string name = child->GetAttribute("name").ToStdString();
                std::string src = child->GetAttribute("src").ToStdString();
                glbase::Texture* ptex = nullptr;
                if(src == "cards")
                    ptex = &card_texture;
                else if(src == "misc")
                    ptex = &misc_texture;
                else if(src == "bg")
                    ptex = &bg_texture;
                if(ptex) {
                    auto& ti = misc_textures[name];
                    long x, y, w, h;
                    child->GetAttribute("x").ToLong(&x);
                    child->GetAttribute("y").ToLong(&y);
                    child->GetAttribute("w").ToLong(&w);
                    child->GetAttribute("h").ToLong(&h);
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
				std::string name = child->GetAttribute("name").ToStdString();
                std::string src = child->GetAttribute("src").ToStdString();
                glbase::Texture* ptex = nullptr;
                if(src == "cards")
                    ptex = &card_texture;
                else if(src == "misc")
                    ptex = &misc_texture;
                else if(src == "bg")
                    ptex = &bg_texture;
                if(ptex) {
                    auto& ti = misc_textures[name];
                    long val[8];
                    child->GetAttribute("x1").ToLong(&val[0]);
                    child->GetAttribute("y1").ToLong(&val[1]);
                    child->GetAttribute("x2").ToLong(&val[2]);
                    child->GetAttribute("y2").ToLong(&val[3]);
                    child->GetAttribute("x3").ToLong(&val[4]);
                    child->GetAttribute("y3").ToLong(&val[5]);
                    child->GetAttribute("x4").ToLong(&val[6]);
                    child->GetAttribute("y4").ToLong(&val[7]);
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
        return true;
	}

}
