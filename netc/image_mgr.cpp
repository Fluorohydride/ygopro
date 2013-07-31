#include "image_mgr.h"
#include <tuple>

namespace ygopro
{

	ImageMgr imageMgr;

	ImageMgr::ImageMgr(): textureid_all(0), textureid_card(0), textureid_bg(0), card_index(0) {
		for(int i = 0; i < 16; ++i)
			system_texture[i] = nullptr;
	}

	ImageMgr::~ImageMgr() {
		for(auto iter : card_images)
			delete iter.second;
	}

	void ImageMgr::InitTextures() {
		glGenTextures(1, &textureid_card);
		glBindTexture(GL_TEXTURE_2D, textureid_card);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 4096, 2048, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		if(image_unknown.IsOk())
			card_unknown = LoadCard(image_unknown);
		if(image_sleeve1.IsOk())
			card_sleeve1 = LoadCard(image_sleeve1);
		if(image_sleeve2.IsOk())
			card_sleeve2 = LoadCard(image_sleeve2);
		if(image_texture.IsOk())
			textureid_all = LoadTexture(image_texture);
		if(image_bg.IsOk())
			textureid_bg = LoadTexture(image_bg);
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
		if(card_index >= 184)
			return card_unknown;
		TextureInfo ti;
		unsigned int indexx = (card_index % 23);
		unsigned int indexy = card_index / 23;
		unsigned int imagex = img.GetWidth();
		unsigned int imagey = img.GetHeight();
		unsigned char * pxdata = img.GetData();
		unsigned char * px = new unsigned char[imagex * imagey * 4];
		memset(px, 0, sizeof(unsigned char) * imagex * imagey * 4);
		for(unsigned int y = 0; y < imagey; y++) {
			for(unsigned int x = 0; x < imagex; ++x) {
				px[(x + y * imagex) * 4 + 0] = pxdata[(x + y * imagex) * 3 + 0];
				px[(x + y * imagex) * 4 + 1] = pxdata[(x + y * imagex) * 3 + 1];
				px[(x + y * imagex) * 4 + 2] = pxdata[(x + y * imagex) * 3 + 2];
			}
		}
		glTexSubImage2D(GL_TEXTURE_2D, 0, indexx * 178, indexy * 256, imagex, imagey, GL_RGBA, GL_UNSIGNED_BYTE, px);
		delete[] px;
		ti.lx = 178.0f / 4096.0f * indexx;
		ti.ly = 1.0f / 16.0f * indexy;
		ti.rx = ti.lx + imagex / 4096.0f;
		ti.ry = ti.ly + imagey / 2048.0f;
		card_index++;
		return ti;
	}

	void ImageMgr::LoadSingleImage(wxImage& img, const wxString& file) {
		if(!wxFileExists(file))
			return;
		img.LoadFile(file);
	}

	void ImageMgr::LoadImageConfig(const wxString& name) {
		wxXmlDocument doc;
		if(!doc.Load(name, wxT("UTF-8"), wxXMLDOC_KEEP_WHITESPACE_NODES))
			return;
		wxXmlNode* root = doc.GetRoot();
		wxString texture_file = root->GetAttribute("all");
		wxString sleeve_file = root->GetAttribute("sleeve");
		wxString unknown_file = root->GetAttribute("unknown");
		wxString bg_file = root->GetAttribute("background");
		if(wxFileExists(texture_file))
			image_texture.LoadFile(texture_file);
		if(wxFileExists(sleeve_file)) {
			image_sleeve1.LoadFile(sleeve_file);
			image_sleeve2.LoadFile(sleeve_file);
		}
		if(wxFileExists(unknown_file))
			image_unknown.LoadFile(unknown_file);
		if(wxFileExists(bg_file)) {
			image_bg.LoadFile(bg_file);
			background.lx = 0;
			background.ly = 0;
			background.rx = (float)image_bg.GetWidth() / texlen(image_bg.GetWidth());
			background.ry = (float)image_bg.GetHeight() / texlen(image_bg.GetHeight());
		}
		wxXmlNode* child = root->GetChildren();
		float all_width = texlen(image_texture.GetWidth());
		float all_height = texlen(image_texture.GetHeight());
		while (child) {
			if (child->GetName() == wxT("texture")) {
				std::string name = child->GetAttribute("name").ToStdString();
				wxString sx = child->GetAttribute("x");
				wxString sy = child->GetAttribute("y");
				wxString sw = child->GetAttribute("w");
				wxString sh = child->GetAttribute("h");
				wxString sid = child->GetAttribute("id");
				long x, y, w, h;
				TextureInfo ti;
				sx.ToLong(&x);
				sy.ToLong(&y);
				sw.ToLong(&w);
				sh.ToLong(&h);
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

	}

}
