#ifndef _IMAGE_MGR_H_
#define _IMAGE_MGR_H_

#include "wx/image.h"
#include "GL/glew.h"

namespace ygopro
{
	struct ImageInfo {
		wxImage image;
		int texture_id;
		int imagex;
		int imagey;
		int texturex;
		int texturey;

		ImageInfo(): texture_id(0) {

		}

		~ImageInfo() {
			if(texture_id)
				;
		}

		wxImage& getBitmap() {
			return image;
		}

		int getTexture() {
			if(texture_id)
				return texture_id;

			image.Rescale(256, 256);
  
			glGenTextures(1, &texture_id);

			// Setup the new texture
			glBindTexture(GL_TEXTURE_2D, texture_id);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// Load the data into the texture buffer
			glTexImage2D(GL_TEXTURE_2D, 0, image.HasAlpha() ? 4 : 3, m_resolution, m_resolution, 0, m_img.HasAlpha() ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, image.GetData());

			return texture_id;
		}

		void load(const std::string& file) {
			if(!wxFileExists(path))
				return;
			image.LoadFile(file);
			imagex = image.GetWidth();
			imagey = image.GetHeight();
		}

	};

	class ImageMgr {
	public:
		ImageInfo* getBitmap(unsigned int id);
		ImageInfo* getBitmap(const std::string& name);


	};

}

#endif
