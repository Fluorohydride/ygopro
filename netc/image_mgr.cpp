#include "image_mgr.h"
#include "game_frame.h"
#include "../common/common.h"

#ifdef __WXMAC__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

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
				cti.ti = textures["unknown"];
                cti.is_system = true;
			} else {
                auto& card_image = card_images[id];
                if(card_image.img.IsOk() || card_image.img.LoadFile(file)) {
                    LoadTexture(card_image);
                    cti.ti.src = &card_image;
                    cti.ti.lx = 0;
                    cti.ti.ly = 0;
                    cti.ti.rx = (double)card_image.img.GetWidth() / (double)card_image.t_width;
                    cti.ti.ry = (double)card_image.img.GetHeight() / (double)card_image.t_height;
                    cti.is_system = false;
                } else {
                    card_images.erase(id);
                    cti.ti = textures["unknown"];
                    cti.is_system = true;
                }
			}
            cti.ref_count++;
            return cti;
		}
        iter->second.ref_count++;
		return iter->second;
	}

	CardTextureInfo& ImageMgr::ReloadCardTexture(unsigned int id) {
        auto& cti = card_textures[id];
        auto iter = card_images.find(id);
        if(iter != card_images.end() && iter->second.t_index)
            glDeleteTextures(1, &iter->second.t_index);
        card_images.erase(id);
        wxString file = wxString::Format("%s/%d.jpg", ((const std::string&)commonCfg["image_path"]).c_str(), id);
        if(!wxFileExists(file)) {
            cti.ti = textures["unknown"];
            cti.is_system = true;
        } else {
            auto& card_image = card_images[id];
            if(card_image.img.LoadFile(file)) {
                LoadTexture(card_image);
                cti.ti.src = &card_image;
                cti.ti.lx = 0;
                cti.ti.ly = 0;
                cti.ti.rx = (double)card_image.img.GetWidth() / (double)card_image.t_width;
                cti.ti.ry = (double)card_image.img.GetHeight() / (double)card_image.t_height;
                cti.is_system = false;
            } else {
                card_images.erase(id);
                cti.ti = textures["unknown"];
                cti.is_system = true;
            }
        }
        return cti;
	}

    void ImageMgr::UnloadCardTexture(unsigned int id) {
        auto iter = card_textures.find(id);
        if(iter == card_textures.end())
            return;
        auto& cti = iter->second;
        if(cti.ref_count == 0)
            return;
        cti.ref_count--;
        if(!cti.is_system && cti.ref_count == 0 && cti.ti.src->t_index) {
            glDeleteTextures(1, &cti.ti.src->t_index);
            cti.ti.src->t_index = 0;
        }
        card_textures.erase(iter);
		card_images.erase(id);
    }
    
	void ImageMgr::UnloadAllCardTexture() {
        for(auto& card_image : card_images) {
            if(card_image.second.t_index) {
                glDeleteTextures(1, &card_image.second.t_index);
                card_image.second.t_index = 0;
            }
        }
        card_textures.clear();
        card_images.clear();
	}

	void ImageMgr::LoadTexture(SrcImageInfo& img_info) {
        wxImage& img = img_info.img;
		unsigned int tid;
		glGenTextures(1, &tid);
		glBindTexture(GL_TEXTURE_2D, tid);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		unsigned int imagex = img.GetWidth();
		unsigned int imagey = img.GetHeight();
		unsigned int tx = texlen(img.GetWidth());
		unsigned int ty = texlen(img.GetHeight());
		unsigned char* px = new unsigned char[tx * ty * 4];
		unsigned char* pxdata = img.GetData();
		unsigned char* apdata = img.GetAlpha();	
		memset(px, 0, sizeof(unsigned char) * tx * ty * 4);
		for(unsigned int y = 0; y < imagey; ++y) {
			for(unsigned int x = 0; x < imagex; ++x) {
				px[(x + y * tx) * 4 + 0] = pxdata[(x + y * imagex) * 3 + 0];
				px[(x + y * tx) * 4 + 1] = pxdata[(x + y * imagex) * 3 + 1];
				px[(x + y * tx) * 4 + 2] = pxdata[(x + y * imagex) * 3 + 2];
				if(apdata)
					px[(x + y * tx) * 4 + 3] = 255 - apdata[x + y * imagex];
			}
		}
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tx, ty, 0, GL_RGBA, GL_UNSIGNED_BYTE, px);
        glBindTexture(GL_TEXTURE_2D, 0);
		delete[] px;
        img_info.t_index = tid;
        img_info.t_width = tx;
        img_info.t_height = ty;
	}

    void ImageMgr::InitTextures(bool force) {
        static bool first_time = true;
        if(!first_time && !force)
            return;
        first_time = false;
        for(auto& src : src_images) {
            if(src.second.t_index)
                glDeleteTextures(1, &src.second.t_index);
            LoadTexture(src.second);
        }
    }
    
	void ImageMgr::LoadSingleImage(const std::string& name, const wxString& file) {
        if(!wxFileExists(file))
            return;
        auto& image = src_images[name];
        if(image.img.LoadFile(file))
        {
            single_images[name] = file;
            image.t_index = 0;
            image.t_width = texlen(image.img.GetWidth());
            image.t_height = texlen(image.img.GetWidth());
            auto& ti = textures[name];
            ti.src = &image;
            ti.lx = 0;
            ti.ly = 0;
            ti.rx = image.img.GetWidth() / (double)image.t_width;
            ti.ry = image.img.GetHeight() / (double)image.t_height;
        } else
            src_images.erase(name);
	}

	bool ImageMgr::LoadImageConfig(const wxString& name) {
		wxXmlDocument doc;
        if(!wxFileExists(name))
            return false;
		if(!doc.Load(name, wxT("UTF-8"), wxXMLDOC_KEEP_WHITESPACE_NODES))
			return false;
        wxXmlNode* root = doc.GetRoot();
		wxXmlNode* child = root->GetChildren();
		while (child) {
            if (child->GetName() == wxT("image")) {
                std::string name = child->GetAttribute("name").ToStdString();
                wxString path = child->GetAttribute("path");
                if(wxFileExists(path)) {
                    auto& src = src_images[name];
                    if(src.img.LoadFile(path))
                    {
                        src.t_index = 0;
                        src.t_width = texlen(src.img.GetWidth());
                        src.t_height = texlen(src.img.GetHeight());
                    } else
                        src_images.erase(name);
                }
            } else if (child->GetName() == wxT("texture")) {
				long x, y, w, h;
                std::string src = child->GetAttribute("src").ToStdString();
				std::string name = child->GetAttribute("name").ToStdString();
				child->GetAttribute("x").ToLong(&x);
				child->GetAttribute("y").ToLong(&y);
				child->GetAttribute("w").ToLong(&w);
				child->GetAttribute("h").ToLong(&h);
                auto iter = src_images.find(src);
                if(iter != src_images.end()) {
                    TextureInfo& ti = textures[name];
                    ti.src = &iter->second;
                    ti.lx = x / (double)iter->second.t_width;
                    ti.ly = y / (double)iter->second.t_height;
                    ti.rx = ti.lx + w / (double)iter->second.t_width;
                    ti.ry = ti.ly + h / (double)iter->second.t_height;
                }
			} else if(child->GetName() == wxT("layout")) {
				LayoutInfo li;
				std::string name = child->GetAttribute("name").ToStdString();
				wxString style = child->GetAttribute("style");
				li.click = child->GetAttribute("click");
				std::string texture = child->GetAttribute("texture").ToStdString();
				child->GetAttribute("x1").ToDouble(&li.x1);
				child->GetAttribute("x2").ToDouble(&li.x2);
				child->GetAttribute("x3").ToDouble(&li.x3);
				child->GetAttribute("x4").ToDouble(&li.x4);
				child->GetAttribute("y1").ToDouble(&li.y1);
				child->GetAttribute("y2").ToDouble(&li.y2);
				child->GetAttribute("y3").ToDouble(&li.y3);
				child->GetAttribute("y4").ToDouble(&li.y4);
				if(style == "button")
					li.style = LAYOUT_BUTTON;
				else if(style == "lp")
					li.style = LAYOUT_LP;
				else if(style == "text")
					li.style = LAYOUT_TEXT;
				else if(style == "phase")
					li.style = LAYOUT_PHASE;
				else
					li.style = LAYOUT_STATIC;
				if(texture == "")
					li.ptex = nullptr;
				else
					li.ptex = &textures[texture];
				layouts.push_back(li);
				if(li.click != "")
					clickable.push_back(li);
			}
			child = child->GetNext();
		}
		TextureInfo& ti = textures["number"];
		float w = (ti.rx - ti.lx) / 4;
		float h = (ti.ry - ti.ly) / 4;
		for(int i = 0; i < 16; ++i) {
			TextureInfo nti;
            nti.src = ti.src;
			nti.lx = ti.lx + (i % 4) * w;
			nti.ly = ti.ly + (i / 4) * h;
			nti.rx = nti.lx + w;
			nti.ry = nti.ly + h;
			text_texture.push_back(nti);
		}
        return true;
	}

}
