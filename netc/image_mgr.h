#ifndef _IMAGE_MGR_H_
#define _IMAGE_MGR_H_

#include "GL/glew.h"
#include "wx/image.h"
#include "xml_config.h"
#include <vector>
#include <unordered_map>

namespace ygopro
{
	struct TextureInfo {
		float lx;
		float ly;
		float rx;
		float ry;
	};

	class ImageMgr : XMLConfig {

	public:
		ImageMgr();
		~ImageMgr();

		inline unsigned int texlen(unsigned int len) {
			len = len - 1;
			len = len | (len >> 1);
			len = len | (len >> 2);
			len = len | (len >> 4);
			len = len | (len >> 8);
			len = len | (len >> 16);
			return len + 1;
		}

		void InitTextures();
		TextureInfo& GetCardTexture(unsigned int id);
		TextureInfo& ReloadCardTexture(unsigned int id);
		unsigned int LoadTexture(const wxImage& img);
		TextureInfo LoadCard(const wxImage& img);
		
		void LoadSleeve(const wxString& file);
		void LoadBackground(const wxString& file);
		virtual void LoadConfig(const wxString& file);
		virtual void SaveConfig(const wxString& file);

		wxImage image_texture;
		wxImage image_unknown;
		wxImage image_sleeve1;
		wxImage image_sleeve2;
		wxImage image_bg;
		unsigned int texture_all;
		unsigned int texture_card;
		unsigned int texture_bg;

		unsigned int card_index;

		TextureInfo card_unknown;
		TextureInfo card_sleeve;
		TextureInfo txtrure_negate;
		TextureInfo texture_field;

	private:
		std::unordered_map<unsigned int, TextureInfo> card_textures;
		std::unordered_map<unsigned int, wxImage*> card_images;
	};

	extern ImageMgr imageMgr;

}

#endif
