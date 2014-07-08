#include "image_mgr.h"
#include "game_scene.h"
#include "../common/common.h"

#ifdef __WXMAC__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "wx/xml/xml.h"

namespace ygopro
{

	ImageMgr imageMgr;

	ImageMgr::ImageMgr() {
        
	}

	ImageMgr::~ImageMgr() {
		
	}

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
                        cti.ti.x = (blockid % 20) * 100;
                        cti.ti.y = (blockid / 20) * 145;
                        cti.ti.w = 100;
                        cti.ti.h = 145;
                        cti.ref_block = blockid;
                        sf::Texture t;
                        t.loadFromImage(img);
                        sf::Sprite sp;
                        sp.setTexture(t);
                        temp_texture.draw(sp);
                        temp_texture.display();
                        card_texture.update(temp_texture.getTexture().copyToImage(), cti.ti.x, cti.ti.y);
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
        card_texture.create(2048, 2048);
        temp_texture.create(100, 145);
        for(short i = 7; i < 280; ++i)
            unuse_block.push_back(i);
        /*
        unsigned int tid;
		glGenTextures(1, &tid);
		glBindTexture(GL_TEXTURE_2D, tid);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		unsigned char* px = new unsigned char[2048 * 2048 * 4];
		memset(px, 0, sizeof(unsigned char) * 2048 * 2048 * 4);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2048, 2048, 0, GL_RGBA, GL_UNSIGNED_BYTE, px);
        glBindTexture(GL_TEXTURE_2D, 0);
        delete[] px;
        */
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
                            card_texture.update(img, 0, 0);
                        } else if(name == "misc") {
                            misc_texture.loadFromImage(img);
                        } else if(name == "bg") {
                            bg_texture.loadFromImage(img);
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
