#ifndef _IMAGE_MGR_H_
#define _IMAGE_MGR_H_

#include "GL/glew.h"
#include "wx/image.h"
#include <vector>
#include <unordered_map>

namespace ygopro
{
	struct ImageInfo {
		wxImage image;
		unsigned int texture_id;
		unsigned int imagex;
		unsigned int imagey;
		float lx;
		float ly;
		float rx;
		float ry;
	};

	class ImageMgr {

	public:
		ImageMgr();
		~ImageMgr();
		void loadTextures();
		ImageInfo* getCardImage(unsigned int id);
		ImageInfo* reloadCardImage(unsigned int id);
		inline unsigned int texlen(unsigned int len) {
			len = len - 1;
			len = len | (len >> 1);
			len = len | (len >> 2);
			len = len | (len >> 4);
			len = len | (len >> 8);
			len = len | (len >> 16);
			return len + 1;
		}
		ImageInfo* load(const std::string& file);
		ImageInfo* loadCard(const std::string& file);
		void genCardMap();

		unsigned int card_index;
		unsigned int card_texture;
		ImageInfo* unknown_card;
		ImageInfo* sleeve;
		ImageInfo* negate;
		ImageInfo* background;
		ImageInfo* field_img;

	private:
		std::unordered_map<unsigned int, ImageInfo*> card_images;
		std::vector<unsigned int> texture_loaded;
	};

	extern ImageMgr imageMgr;

}

#endif
