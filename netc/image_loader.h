#ifndef _IMAGE_LOADER_H_
#define _IMAGE_LOADER_H_

#include <unordered_map>
#include "MyGUI.h"
#include "MyGUI_OpenGLPlatform.h"

namespace ygopro
{

	class MyImageLoader : public MyGUI::OpenGLImageLoader {
	public:
		MyImageLoader();
		~MyImageLoader();
		virtual void* loadImage(int& _width, int& _height, MyGUI::PixelFormat& _format, const std::string& _filename);
		virtual void saveImage(int _width, int _height, MyGUI::PixelFormat _format, void* _texture, const std::string& _filename);
		bool isImageExists(const std::string& _filename);

	protected:
		std::unordered_map<std::string, unsigned char*> imageLoaded;
	};

	extern MyImageLoader imageLoader;

}

#endif //_IMAGE_LOADER_H_
