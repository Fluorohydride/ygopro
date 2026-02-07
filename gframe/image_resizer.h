#ifndef IMAGE_RESIZER_H
#define IMAGE_RESIZER_H

#include <irrlicht.h>

namespace ygo {

class ImageResizer {
private:
	bool imageScaleSTB(irr::video::IImage* src, irr::video::IImage* dest);
	void imageScaleNNAA(irr::video::IImage* src, irr::video::IImage* dest, bool use_threading);
public:
	void resize(irr::video::IImage* src, irr::video::IImage* dest, bool use_threading);
};

extern ImageResizer imageResizer;

} // namespace ygo

#endif // IMAGE_RESIZER_H
