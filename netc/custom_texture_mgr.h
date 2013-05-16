#ifndef _CUSTOM_TEXTURE_MGR_H
#define _CUSTOM_TEXTURE_MGR_H

#include <string>
#include <unordered_map>
#include "MyGUI.h"
#include "MyGUI_OpenGLPlatform.h"

namespace ygopro
{

	struct CustomTextureInfo {
		MyGUI::ITexture* ptexture;
		unsigned int ref_count;
	};

	class CustomTextureMgr {

	public:
		virtual void LoadTexture(const std::string& name) = 0;
		virtual void DestroyTexture(const std::string& name) = 0;

	protected:
		std::unordered_map<std::string, CustomTextureInfo> custom_textures;

	};

}

#endif
