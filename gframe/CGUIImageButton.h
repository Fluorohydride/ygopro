#ifndef _C_GUI_IMAGE_BUTTON_H_
#define _C_GUI_IMAGE_BUTTON_H_

#include <irrlicht.h>
#include "CGUIButton.h"

namespace irr {
namespace gui {

void Draw2DImageRotation(video::IVideoDriver* driver, video::ITexture* image, core::rect<s32> sourceRect,
                         core::position2d<s32> position, core::position2d<s32> rotationPoint, f32 rotation = 0.0f,
                         core::vector2df scale = core::vector2df(1.0, 1.0), bool useAlphaChannel = true, video::SColor color = 0xffffffff);
void Draw2DImageQuad(video::IVideoDriver* driver, video::ITexture* image, core::rect<s32> sourceRect,
                     core::position2d<s32> corner[4], bool useAlphaChannel = true, video::SColor color = 0xffffffff);

class CGUIImageButton : public CGUIButton {
public:
	static CGUIImageButton* addImageButton(IGUIEnvironment* env, const core::rect<s32>& rectangle, IGUIElement* parent, s32 id);
	
	CGUIImageButton(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle);
	virtual void draw();
	virtual void setImage(video::ITexture* image = 0);
	virtual IGUIFont* getOverrideFont(void) const;
	virtual IGUIFont* getActiveFont() const;

	// from ygopro
	void setDrawImage(bool b);
	void setImageRotation(f32 r);
	void setImageScale(core::vector2df s);
	void setImageSize(core::dimension2di s);

private:
	// from ygopro
	bool isDrawImage;
	bool isFixedSize;
	f32 imageRotation;
	core::vector2df imageScale;
	core::dimension2di imageSize;
};

}
}

#endif //_C_GUI_IMAGE_BUTTON_H_
