#include "image_mgr.h"

namespace ygopro
{

	ImageMgr imageMgr;

	ImageMgr::ImageMgr(): card_index(0), card_texture(0), unknown_card(nullptr) {

	}

	ImageMgr::~ImageMgr() {

	}

	ImageInfo* ImageMgr::getCardImage(unsigned int id) {
		auto iter = card_images.find(id);
		if(iter == card_images.end()) {
			return loadCard("", card_index);
		}
		return iter->second;
	}

	void ImageMgr::loadTextures() {
		genCardMap();
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

	ImageInfo* ImageMgr::loadCard(const std::string& file, unsigned int index) {
		ImageInfo* ii = new ImageInfo;
		if(index >= 256 || !wxFileExists(file)) {
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
		unsigned int indexx = (index % 16);
		unsigned int indexy = index / 16;
		glTexSubImage2D(GL_TEXTURE_2D, 0, indexx * 256, indexy * 256, indexx * 256 + ii->imagex, indexy * 256 + ii->imagey, GL_RGB, GL_UNSIGNED_BYTE, ii->image.GetData());
		ii->lx = 1.0f / 16.0f * indexx;
		ii->ly = 1.0f / 16.0f * indexy;
		ii->rx = ii->lx + ii->imagex / 256.0f;
		ii->ry = ii->ly + ii->imagey / 256.0f;
		index++;
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
