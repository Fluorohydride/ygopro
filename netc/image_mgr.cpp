#include "image_mgr.h"
#include <tuple>

namespace ygopro
{

	ImageMgr imageMgr;

	ImageMgr::ImageMgr(): texture_all(0), texture_card(0), texture_bg(0), card_index(0) {

	}

	ImageMgr::~ImageMgr() {
		for(auto iter : card_images)
			delete iter.second;
	}

	void ImageMgr::InitTextures() {
		glGenTextures(1, &texture_card);
		glBindTexture(GL_TEXTURE_2D, texture_card);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 4096, 2048, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		unsigned int tx = texlen(img.GetWidth());
		unsigned int ty = texlen(img.GetHeight());
		unsigned char* px = new unsigned char[tx * ty * 4];
		unsigned char* pxdata = img.GetData();
		unsigned char* apdata = img.GetAlpha();
		memset(px, 0, sizeof(unsigned char) * tx * ty * 4);
		for(unsigned int y = 0; y < tx; ++y) {
			for(unsigned int x = 0; x < tx; ++x) {
				px[(x + y * tx) * 4 + 0] = pxdata[(x + y * tx) * 3 + 0];
				px[(x + y * tx) * 4 + 1] = pxdata[(x + y * tx) * 3 + 1];
				px[(x + y * tx) * 4 + 2] = pxdata[(x + y * tx) * 3 + 2];
				if(apdata)
					px[(x + y * tx) * 4 + 3] = apdata[(x + y * tx) * 3];
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

	void ImageMgr::LoadSleeve(const wxString& file) {

	}

	void ImageMgr::LoadBackground(const wxString& file) {
	}

	void ImageMgr::LoadConfig(const wxString& name) {
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
			image_sleeve1.LoadFile(texture_file);
			image_sleeve2.LoadFile(texture_file);
		}
		if(wxFileExists(unknown_file))
			image_unknown.LoadFile(unknown_file);
		if(wxFileExists(bg_file))
			image_bg.LoadFile(bg_file);
		wxXmlNode* child = root->GetChildren();
		std::unordered_map<std::string, std::tuple<long, long, long, long>> infos;
		while (child) {
			if (child->GetName() == wxT("texture")) {
				std::string name = child->GetAttribute("name").ToStdString();
				wxString sx = child->GetAttribute("x");
				wxString sy = child->GetAttribute("y");
				wxString sw = child->GetAttribute("w");
				wxString sh = child->GetAttribute("h");
				long x, y, w, h;
				sx.ToLong(&x);
				sy.ToLong(&y);
				sw.ToLong(&w);
				sh.ToLong(&h);
				infos[name] = std::make_tuple(x, y, w, h);
			}
			child = child->GetNext();
		}
	}

	void ImageMgr::SaveConfig(const wxString& name) {
	
	}

}
