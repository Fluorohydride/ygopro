// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef _C_GUI_IMAGE_BUTTON_H_
#define _C_GUI_IMAGE_BUTTON_H_

#include <IrrCompileConfig.h>
#ifdef _IRR_COMPILE_WITH_GUI_

#include <IGUIButton.h>
#include <SColor.h>

namespace irr {
namespace video {
class IVideoDriver;
class ITexture;
}
namespace gui {
class IGUISpriteBank;

void Draw2DImageRotation(video::IVideoDriver* driver, video::ITexture* image, core::rect<s32> sourceRect,
                         core::position2d<s32> position, core::position2d<s32> rotationPoint, f32 rotation = 0.0f,
                         core::vector2df scale = core::vector2df(1.0, 1.0), bool useAlphaChannel = true, video::SColor color = 0xffffffff);
void Draw2DImageQuad(video::IVideoDriver* driver, video::ITexture* image, core::rect<s32> sourceRect,
                     core::position2d<s32> corner[4], bool useAlphaChannel = true, video::SColor color = 0xffffffff);

class CGUIImageButton : public IGUIButton {
public:
	static CGUIImageButton* addImageButton(IGUIEnvironment* env, const core::rect<s32>& rectangle, IGUIElement* parent, s32 id);
	
	//! constructor
	CGUIImageButton(IGUIEnvironment* environment, IGUIElement* parent,
		s32 id, core::rect<s32> rectangle, bool noclip = false);
	
	//! destructor
	~CGUIImageButton() override;

	//! called if an event happened.
	bool OnEvent(const SEvent& event) override;

	//! draws the element and its children
	void draw() override;

	//! sets another skin independent font. if this is set to zero, the button uses the font of the skin.
	void setOverrideFont(IGUIFont* font = nullptr) override;

	//! Gets the override font (if any)
	IGUIFont* getOverrideFont() const override;

	//! Get the font which is used right now for drawing
	IGUIFont* getActiveFont() const override;

	//! Sets an image which should be displayed on the button when it is in normal state.
	void setImage(video::ITexture* image = nullptr) override;

	//! Sets an image which should be displayed on the button when it is in normal state.
	void setImage(video::ITexture* image, const core::rect<s32>& pos) override;

	//! Sets an image which should be displayed on the button when it is in pressed state.
	void setPressedImage(video::ITexture* image = nullptr) override;

	//! Sets an image which should be displayed on the button when it is in pressed state.
	void setPressedImage(video::ITexture* image, const core::rect<s32>& pos) override;

	//! Sets the sprite bank used by the button
	void setSpriteBank(IGUISpriteBank* bank = nullptr) override;

	//! Sets the animated sprite for a specific button state
	/** \param index: Number of the sprite within the sprite bank, use -1 for no sprite
	\param state: State of the button to set the sprite for
	\param index: The sprite number from the current sprite bank
	\param color: The color of the sprite
	*/
	void setSprite(EGUI_BUTTON_STATE state, s32 index,
		video::SColor color = video::SColor(255, 255, 255, 255), bool loop = false) override;

	//! Sets if the button should behave like a push button. Which means it
	//! can be in two states: Normal or Pressed. With a click on the button,
	//! the user can change the state of the button.
	void setIsPushButton(bool isPushButton = true) override;

	//! Checks whether the button is a push button
	bool isPushButton() const override;

	//! Sets the pressed state of the button if this is a pushbutton
	void setPressed(bool pressed = true) override;

	//! Returns if the button is currently pressed
	bool isPressed() const override;

	//! Sets if the button should use the skin to draw its border
	void setDrawBorder(bool border = true) override;

	//! Checks if the button face and border are being drawn
	bool isDrawingBorder() const override;

	//! Sets if the alpha channel should be used for drawing images on the button (default is false)
	void setUseAlphaChannel(bool useAlphaChannel = true) override;

	//! Checks if the alpha channel should be used for drawing images on the button
	bool isAlphaChannelUsed() const override;

	//! Sets if the button should scale the button images to fit
	void setScaleImage(bool scaleImage = true) override;

	//! Checks whether the button scales the used images
	bool isScalingImage() const override;

	//! Writes attributes of the element.
	void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const override;

	//! Reads attributes of the element
	void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options) override;

	// from ygopro
	void setDrawImage(bool b);
	void setImageRotation(f32 r);
	void setImageScale(core::vector2df s);
	void setImageSize(core::dimension2di s);

protected:
	struct ButtonSprite
	{
		s32 Index;
		video::SColor Color;
		bool Loop;
	};

	ButtonSprite ButtonSprites[EGBS_COUNT];

	IGUISpriteBank* SpriteBank;
	IGUIFont* OverrideFont;

	video::ITexture* Image;
	video::ITexture* PressedImage;

	core::rect<s32> ImageRect;
	core::rect<s32> PressedImageRect;

	bool IsPushButton;
	bool Pressed;
	bool UseAlphaChannel;
	bool DrawBorder;
	bool ScaleImage;
	
	// from ygopro
	bool isDrawImage;
	bool isFixedSize;
	f32 imageRotation;
	core::vector2df imageScale;
	core::dimension2di imageSize;
};

}
}

#endif // _IRR_COMPILE_WITH_GUI_

#endif //_C_GUI_IMAGE_BUTTON_H_
