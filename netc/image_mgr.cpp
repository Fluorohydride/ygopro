#include "image_mgr.h"
#include <tuple>

namespace ygopro
{

	ImageMgr imageMgr;

	ImageMgr::ImageMgr() {
		for(int i = 0; i < 32; ++i)
			system_texture[i] = nullptr;
	}

	ImageMgr::~ImageMgr() {
		for(auto iter : card_images)
			delete iter.second;
	}

	void ImageMgr::InitTextures() {
		
	}

	TextureInfo& ImageMgr::GetCardTexture(unsigned int id) {
		auto iter = card_textures.find(id);
		if(iter == card_textures.end()) {
			wxString file = wxString::Format("./image/%d.jpg", id);
			TextureInfo& ti = card_textures[id];
			if(!wxFileExists(file)) {
				ti = card_unknown;
				return ti;
			} else {
				wxImage* img = new wxImage;
				img->LoadFile(file);
				ti = LoadCard(*img);
				card_images[id] = img;
				return ti;
			}
		}
		return iter->second;
	}

	TextureInfo& ImageMgr::ReloadCardTexture(unsigned int id) {
		wxString file = wxString::Format("./image/%d.jpg", id);
		UnloadCardTexture(id);
		TextureInfo& ti = card_textures[id];
		if(!wxFileExists(file)) {
			ti = card_unknown;
			return ti;
		} else {
			if(card_images.find(id) == card_images.end()) {
				wxImage* img = new wxImage;
				img->LoadFile(file);
				ti = LoadCard(*img);
				card_images[id] = img;
			} else {
				wxImage* img = card_images[id];
				img->LoadFile(file);
				ti = LoadCard(*img);
			}
		}
		return ti;
	}

	void ImageMgr::UnloadCardTexture(unsigned int id) {
		auto iter = card_textures.find(id);
		if(iter == card_textures.end())
			return;
		unsigned index = iter->second.index;
		glDeleteTextures(1, &index);
	}

	unsigned int ImageMgr::LoadTexture(const wxImage& img) {
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
					px[(x + y * tx) * 4 + 3] = apdata[x + y * imagex];
			}
		}
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tx, ty, 0, GL_RGBA, GL_UNSIGNED_BYTE, px);
		delete px;
		return tid;
	}

	TextureInfo ImageMgr::LoadCard(const wxImage& img) {

		TextureInfo ti;
		ti.index = LoadTexture(img);
		ti.lx = 0.0f;
		ti.ly = 0.0f;
		ti.rx = img.GetWidth();
		ti.ry = img.GetHeight();

		return ti;
	}

	void ImageMgr::LoadSingleImage(unsigned int index, const wxString& file) {

	}

	void ImageMgr::LoadImageConfig(const wxString& name) {
		wxXmlDocument doc;
		if(!doc.Load(name, wxT("UTF-8"), wxXMLDOC_KEEP_WHITESPACE_NODES))
			return;
		wxXmlNode* child = root->GetChildren();
		while (child) {
            if (child->GetName() == wxT("image")) {
                SrcImageInfo sii;
                long id;
                child->GetAttribute("id").ToLong(&id);
                wxString path = child->GetAttribute("path");
                if(wxFileExists(path)) {
                    auto& src = src_images[id];
                    src.img.LoadFile(path);
                    LoadTexture(src.img);
                }
            } else if (child->GetName() == wxT("texture")) {
				long x, y, w, h, srcid;
				TextureInfo ti;
				std::string name = child->GetAttribute("name").ToStdString();
                child->GetAttribute("x").ToLong(&srcid);
				child->GetAttribute("x").ToLong(&x);
				child->GetAttribute("y").ToLong(&y);
				child->GetAttribute("w").ToLong(&w);
				child->GetAttribute("h").ToLong(&h);
				ti.lx = x / all_width;
				ti.ly = y / all_height;
				ti.rx = ti.lx + w / all_width;
				ti.ry = ti.ly + h / all_height;
				textures.push_back(ti);
				texture_infos[name] = &textures[textures.size() - 1];
			}
			child = child->GetNext();
		}
		
		system_texture[TEXINDEX_FIELD] = texture_infos["field"];
		system_texture[TEXINDEX_ATTACK] = texture_infos["attack"];
		system_texture[TEXINDEX_ACTIVATE] = texture_infos["activate"];
		system_texture[TEXINDEX_CHAIN] = texture_infos["chain"];
		system_texture[TEXINDEX_MASK] = texture_infos["mask"];
		system_texture[TEXINDEX_NEGATED] = texture_infos["negated"];
		system_texture[TEXINDEX_LIMIT0] = texture_infos["limit0"];
		system_texture[TEXINDEX_LIMIT1] = texture_infos["limit1"];
		system_texture[TEXINDEX_LIMIT2] = texture_infos["limit2"];
		system_texture[TEXINDEX_LPFRAME] = texture_infos["lpframe"];
		system_texture[TEXINDEX_LPBAR] = texture_infos["lpbar"];
		system_texture[TEXINDEX_EQUIP] = texture_infos["equip"];
		system_texture[TEXINDEX_TARGET] = texture_infos["target"];
		system_texture[TEXINDEX_SCISSORS] = texture_infos["scissors"];
		system_texture[TEXINDEX_ROCK] = texture_infos["rock"];
		system_texture[TEXINDEX_PAPER] = texture_infos["paper"];
		system_texture[TEXINDEX_SOCG] = texture_infos["symbol_ocg"];
		system_texture[TEXINDEX_STCG] = texture_infos["symbol_tcg"];
		system_texture[TEXINDEX_SCTM] = texture_infos["symbol_ctm"];
		system_texture[TEXINDEX_STST] = texture_infos["symbol_tst"];
		TextureInfo& ti = *texture_infos["number"];
		float w = (ti.rx - ti.lx) / 4;
		float h = (ti.ry - ti.ly) / 4;
		for(int i = 0; i < 16; ++i) {
			TextureInfo nti;
			nti.lx = ti.lx + (i % 4) * w;
			nti.ly = ti.ly + (i / 4) * h;
			nti.rx = nti.lx + w;
			nti.ry = nti.ly + h;
			text_texture.push_back(nti);
		}
	}

	void ImageMgr::LoadLayoutConfig(const wxString& name) {
		wxXmlDocument doc;
		if(!doc.Load(name, wxT("UTF-8"), wxXMLDOC_KEEP_WHITESPACE_NODES))
			return;
		wxXmlNode* root = doc.GetRoot();
		wxXmlNode* child = root->GetChildren();
		while (child) {
			if (child->GetName() == wxT("layout")) {
				LayoutInfo li;
				std::string name = child->GetAttribute("name").ToStdString();
				wxString style = child->GetAttribute("style");
				wxString click = child->GetAttribute("click");
				std::string texture = child->GetAttribute("texture").ToStdString();
				child->GetAttribute("x1").ToDouble(&li.x1);
				child->GetAttribute("x2").ToDouble(&li.x2);
				child->GetAttribute("x3").ToDouble(&li.x3);
				child->GetAttribute("x4").ToDouble(&li.x4);
				child->GetAttribute("y1").ToDouble(&li.y1);
				child->GetAttribute("y2").ToDouble(&li.y2);
				child->GetAttribute("y3").ToDouble(&li.y3);
				child->GetAttribute("y4").ToDouble(&li.y4);
				if(style == "static")
					li.style = LAYOUT_STATIC;
				else if(style == "lp")
					li.style = LAYOUT_LP;
				else if(style == "text")
					li.style = LAYOUT_TEXT;
				else if(style == "phase")
					li.style = LAYOUT_PHASE;
				else
					li.style = LAYOUT_BUTTON;
				if(click == "")
					li.click = CLICK_NONE;
				else if(click == "avatar0")
					li.click = CLICK_AVATAR0;
				else if(click == "avatar1")
					li.click = CLICK_AVATAR1;
				else if(click == "bp")
					li.click = CLICK_BP;
				else if(click == "m2")
					li.click = CLICK_M2;
				else if(click == "ep")
					li.click = CLICK_EP;
				else if(click == "menu")
					li.click = CLICK_MENU;
				else if(click == "surrender")
					li.click = CLICK_SURRENDER;
				else if(click == "confirm")
					li.click = CLICK_CONFIRM;
				else
					li.click = CLICK_NONE;
				if(texture == "")
					li.ptex = nullptr;
				else
					li.ptex = texture_infos[texture];
				layouts.push_back(li);
				if(li.click)
					clickable.push_back(li);
			}
			child = child->GetNext();
		}
	}

}
