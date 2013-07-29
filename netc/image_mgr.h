#ifndef _IMAGE_MGR_H_
#define _IMAGE_MGR_H_

#include "GL/glew.h"
#include "wx/image.h"
#include "wx/xml/xml.h"
#include <vector>
#include <unordered_map>

namespace ygopro
{
	struct TextureInfo {
		TextureInfo() : lx(0.0f), ly(0.0f), rx(0.0f), ry(0.0f) {}
		float lx;
		float ly;
		float rx;
		float ry;
	};

	struct LayoutInfo {
		bool show;
		float x1;
		float y1;
		float x2;
		float y2;
		float x3;
		float y3;
		float x4;
		float y4;
	};

	class ImageMgr {

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
		
		void LoadSingleImage(wxImage& img, const wxString& file);
		void LoadImageConfig(const wxString& file);
		void LoadLayoutConfig(const wxString& file);

		wxImage image_texture;
		wxImage image_unknown;
		wxImage image_sleeve1;
		wxImage image_sleeve2;
		wxImage image_bg;
		unsigned int textureid_all;
		unsigned int textureid_card;
		unsigned int textureid_bg;

		unsigned int card_index;

		TextureInfo background;
		TextureInfo card_unknown;
		TextureInfo card_sleeve1;
		TextureInfo card_sleeve2;
		TextureInfo texture_field;
		TextureInfo texture_number;
		TextureInfo texture_attack;
		TextureInfo texture_activate;
		TextureInfo texture_chain;
		TextureInfo texture_mask;
		TextureInfo texture_negated;
		TextureInfo texture_limit0;
		TextureInfo texture_limit1;
		TextureInfo texture_limit2;
		TextureInfo texture_lpframe;
		TextureInfo texture_lpbar;
		TextureInfo texture_equip;
		TextureInfo texture_target;
		TextureInfo texture_scissors;
		TextureInfo texture_rock;
		TextureInfo texture_paper;

	private:
		std::unordered_map<unsigned int, TextureInfo> card_textures;
		std::unordered_map<unsigned int, wxImage*> card_images;
	};

	extern ImageMgr imageMgr;

}

#endif
