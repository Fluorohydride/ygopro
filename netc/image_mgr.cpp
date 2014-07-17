#include "image_mgr.h"
#include "game_scene.h"
#include "../common/common.h"

#include "wx/xml/xml.h"

namespace ygopro
{

	ImageMgr imageMgr;

	CardTextureInfo& ImageMgr::GetCardTexture(unsigned int id) {
		auto iter = card_textures.find(id);
		if(iter == card_textures.end()) {
			wxString file = wxString::Format("%s/%d.jpg", ((const std::string&)commonCfg["image_path"]).c_str(), id);
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
                        cti.ti.x = (blockid % 20) * 100;
                        cti.ti.y = (blockid / 20) * 145;
                        cti.ti.w = 100;
                        cti.ti.h = 145;
                        cti.ref_block = blockid;
                        card_image.Load(img.getPixelsPtr(), img.getSize().x, img.getSize().y);
                        glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
                        glViewport(0, 0, 2048, 2048);
                        frame_verts[0].vertex[0] = float(cti.ti.x) / 1024 - 1;
                        frame_verts[0].vertex[1] = float(cti.ti.y) / 1024 - 1;
                        frame_verts[1].vertex[0] = float(cti.ti.x + cti.ti.w) / 1024 - 1;
                        frame_verts[1].vertex[1] = float(cti.ti.y) / 1024 - 1;
                        frame_verts[2].vertex[0] = float(cti.ti.x) / 1024 - 1;
                        frame_verts[2].vertex[1] = float(cti.ti.y + cti.ti.h) / 1024 - 1;
                        frame_verts[3].vertex[0] = float(cti.ti.x + cti.ti.w) / 1024 - 1;
                        frame_verts[3].vertex[1] = float(cti.ti.y + cti.ti.h) / 1024 - 1;
                        frame_verts[0].texcoord[0] = 0.0f;
                        frame_verts[0].texcoord[1] = 0.0f;
                        frame_verts[1].texcoord[0] = (float)img.getSize().x / card_image.GetWidth();
                        frame_verts[1].texcoord[1] = 0.0f;
                        frame_verts[2].texcoord[0] = 0.0f;
                        frame_verts[2].texcoord[1] = (float)img.getSize().y / card_image.GetHeight();
                        frame_verts[3].texcoord[0] = frame_verts[1].texcoord[0];
                        frame_verts[3].texcoord[1] = frame_verts[2].texcoord[1];
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

    TextureInfo& ImageMgr::LoadBigCardTexture(unsigned int id) {
        wxString file = wxString::Format("%s/%d.jpg", ((const std::string&)commonCfg["image_path"]).c_str(), id);
    }
    
    TextureInfo& ImageMgr::GetTexture(const std::string& name) {
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
        for(short i = 7; i < 280; ++i)
            unuse_block.push_back(i);
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
        }
    }
    
    glbase::Texture& ImageMgr::GetTexInfo(int textype) {
        switch(textype) {
            case 0:
                return bg_texture;
            case 1:
                return misc_texture;
            case 2:
                return card_image;
            case 3:
                return card_texture;
            default:
                return misc_texture;
        }
    }
    
	bool ImageMgr::LoadImageConfig(const std::string& name) {
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
                        if(name == "cards") {
                            card_texture.Update(img.getPixelsPtr(), 0, 0, img.getSize().x, img.getSize().y);
                        } else if(name == "misc") {
                            misc_texture.Load(img.getPixelsPtr(), img.getSize().x, img.getSize().y);
                        } else if(name == "bg") {
                            bg_texture.Load(img.getPixelsPtr(), img.getSize().x, img.getSize().y);
                        }
                    }
                }
            } else if (child->GetName() == "texture") {
				TextureInfo ti;
				std::string name = child->GetAttribute("name").ToStdString();
				child->GetAttribute("x").ToLong(&ti.x);
				child->GetAttribute("y").ToLong(&ti.y);
				child->GetAttribute("w").ToLong(&ti.w);
				child->GetAttribute("h").ToLong(&ti.h);
                misc_textures[name] = ti;
			}
			child = child->GetNext();
		}
        return true;
	}

}
