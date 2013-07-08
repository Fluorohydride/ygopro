#include "image_mgr.h"

namespace ygopro
{

	ImageMgr imageMgr;

	ImageMgr::ImageMgr(): card_index(0), card_texture(0), unknown_card(nullptr) {

	}

	ImageMgr::~ImageMgr() {

	}

	void ImageMgr::loadTextures() {
		genCardMap();
		unknown_card = loadCard("./textures/unknown.jpg");
		sleeve = loadCard("./textures/sleeve.jpg");
		field_img = load("./textures/sleeve.jpg");
	}

	ImageInfo* ImageMgr::getCardImage(unsigned int id) {
		auto iter = card_images.find(id);
		if(iter == card_images.end()) {
			char file[32];
			sprintf(file, "./image/%d.jpg");
			return loadCard("file");
		}
		return iter->second;
	}

	ImageInfo* ImageMgr::reloadCardImage(unsigned int id) {
		auto iter = card_images.find(id);
		if(iter == card_images.end())
			return nullptr;
		ImageInfo* ii = iter->second;
		char file[32];
		sprintf(file, "./image/%d.jpg");
		if(card_index >= 256 || !wxFileExists(file)) {
			ii->lx = unknown_card->lx;
			ii->ly = unknown_card->ly;
			ii->rx = unknown_card->rx;
			ii->ry = unknown_card->ry;
			return ii;
		}
		ii->texture_id = 0;
		ii->image.LoadFile(file);
		ii->imagex = ii->image.GetWidth();
		ii->imagey = ii->image.GetHeight();
		unsigned int indexx = (card_index % 16);
		unsigned int indexy = card_index / 16;
		glTexSubImage2D(GL_TEXTURE_2D, 0, indexx * 256, indexy * 256, ii->imagex, ii->imagey, GL_RGB, GL_UNSIGNED_BYTE, ii->image.GetData());
		ii->lx = 1.0f / 16.0f * indexx;
		ii->ly = 1.0f / 16.0f * indexy;
		ii->rx = ii->lx + ii->imagex / 256.0f;
		ii->ry = ii->ly + ii->imagey / 256.0f;
		card_index++;
		return ii;
	}

	ImageInfo* ImageMgr::load(const std::string& file) {
		if(!wxFileExists(file))
			return nullptr;
		ImageInfo* ii = new ImageInfo;
		ii->image.LoadFile(file);
		ii->imagex = ii->image.GetWidth();
		ii->imagey = ii->image.GetHeight();
		glGenTextures(1, &ii->texture_id);
		texture_loaded.push_back(ii->texture_id);
		glBindTexture(GL_TEXTURE_2D, ii->texture_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		unsigned int tx = texlen(ii->imagex);
		unsigned int ty = texlen(ii->imagey);
		glTexImage2D(GL_TEXTURE_2D, 0, ii->image.HasAlpha() ? GL_RGBA : GL_RGB, tx, ty, 0, ii->image.HasAlpha() ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, ii->imagex, ii->imagey, ii->image.HasAlpha() ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, ii->image.GetData());
		ii->rx = (float)ii->imagex / tx;
		ii->ry = (float)ii->imagey / ty;
		return ii;
	}

	ImageInfo* ImageMgr::loadCard(const std::string& file) {
		ImageInfo* ii = new ImageInfo;
		if(card_index >= 256 || !wxFileExists(file)) {
			ii->lx = unknown_card->lx;
			ii->ly = unknown_card->ly;
			ii->rx = unknown_card->rx;
			ii->ry = unknown_card->ry;
			return ii;
		}
		ii->texture_id = 0;
		ii->image.LoadFile(file);
		ii->imagex = ii->image.GetWidth();
		ii->imagey = ii->image.GetHeight();
		unsigned int indexx = (card_index % 16);
		unsigned int indexy = card_index / 16;
		glTexSubImage2D(GL_TEXTURE_2D, 0, indexx * 256, indexy * 256, ii->imagex, ii->imagey, GL_RGB, GL_UNSIGNED_BYTE, ii->image.GetData());
		ii->lx = 1.0f / 16.0f * indexx;
		ii->ly = 1.0f / 16.0f * indexy;
		ii->rx = ii->lx + ii->imagex / 4096.0f;
		ii->ry = ii->ly + ii->imagey / 4096.0f;
		card_index++;
		return ii;
	}

	void ImageMgr::genCardMap() {
		glGenTextures(1, &card_texture);
		texture_loaded.push_back(card_texture);
		glBindTexture(GL_TEXTURE_2D, card_texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 4096, 4096, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	}

}
