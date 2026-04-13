#ifndef IMAGE_UTILITY_H
#define IMAGE_UTILITY_H

#include <irrlicht.h>

namespace ygo {

class ImageUtility {
private:
	static bool imageScaleSTB(irr::video::IImage* src, irr::video::IImage* dest);
	static void imageScaleNNAA(irr::video::IImage* src, irr::video::IImage* dest, bool use_threading);
public:
	static void Resize(irr::video::IImage* src, irr::video::IImage* dest, bool use_threading);
	static irr::video::IImage* RotateImageCCW90(irr::video::IVideoDriver* driver, irr::video::IImage* src);
	static irr::video::IImage* LoadJpegImage(irr::video::IVideoDriver* driver, irr::io::IReadFile* reader, irr::s32 targetWidth, irr::s32 targetHeight);
};

} // namespace ygo

#endif // IMAGE_UTILITY_H
