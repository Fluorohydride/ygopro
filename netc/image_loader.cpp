#include "image_loader.h"
#include "../common/stb_image.h"
#include "../common/stb_image_write.h"

namespace ygopro
{

	MyImageLoader imageLoader;

	MyImageLoader::MyImageLoader() {

	}

	MyImageLoader::~MyImageLoader() {
		//	for(auto iter = imageLoaded.begin(); iter != imageLoaded.end(); ++iter)
		//		stbi_image_free(iter->second);
		imageLoaded.clear();
	}

	void* MyImageLoader::loadImage(int& _width, int& _height, MyGUI::PixelFormat& _format, const std::string& _filename) {
		int comp = 0;
		unsigned char *data = nullptr;
		if(_filename.substr(0, 8) == "./cache/")
			data = stbi_load(_filename.c_str(), &_width, &_height, &comp, 0);
		else {
			std::string resname = "./skin/";
			resname += _filename;
			data = stbi_load(resname.c_str(), &_width, &_height, &comp, 0);
		}
		if(comp == 3)
			_format = MyGUI::PixelFormat::R8G8B8;
		else if(comp == 4)
			_format = MyGUI::PixelFormat::R8G8B8A8;
		else
			_format = MyGUI::PixelFormat::Unknow;
		imageLoaded[_filename] = data;
		return data;
	}

	void MyImageLoader::saveImage(int _width, int _height, MyGUI::PixelFormat _format, void* _texture, const std::string& _filename) {

	}

	bool MyImageLoader::isImageExists(const std::string& _filename) {
		FILE* fp = fopen(_filename.c_str(), "rb");
		if(fp) {
			fclose(fp);
			return true;
		}
		return false;
	}

}
