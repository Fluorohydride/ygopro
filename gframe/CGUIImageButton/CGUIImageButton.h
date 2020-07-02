// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_GUI_BUTTON_H_INCLUDED__
#define __C_GUI_BUTTON_H_INCLUDED__

#include <IrrCompileConfig.h>
#ifndef _IRR_OVERRIDE_
#define _IRR_OVERRIDE_
#endif
#ifdef _IRR_COMPILE_WITH_GUI_

#include <IGUIButton.h>
#include <SColor.h>

namespace irr {
namespace video {
class IVideoDriver;
class ITexture;
};
namespace gui {
class IGUISpriteBank;

void Draw2DImageRotation(video::IVideoDriver* driver, video::ITexture* image, core::rect<s32> sourceRect,
						 core::position2d<s32> position, core::position2d<s32> rotationPoint, f32 rotation = 0.0f,
						 core::vector2df scale = core::vector2df(1.0, 1.0), bool useAlphaChannel = true, video::SColor color = 0xffffffff);
void Draw2DImageQuad(video::IVideoDriver* driver, video::ITexture* image, core::rect<s32> sourceRect,
					 core::position2d<s32> corner[4], bool useAlphaChannel = true, video::SColor color = 0xffffffff);

class CGUIImageButton : public IGUIButton {
public:

	static CGUIImageButton* addImageButton(IGUIEnvironment *env, const core::rect<s32>& rectangle, IGUIElement* parent, s32 id);

	//! constructor
	CGUIImageButton(IGUIEnvironment* environment, IGUIElement* parent,
					s32 id, core::rect<s32> rectangle, bool noclip = false);

	//! destructor
	virtual ~CGUIImageButton();

	//! called if an event happened.
	virtual bool OnEvent(const SEvent& event) _IRR_OVERRIDE_;

	//! draws the element and its children
	virtual void draw() _IRR_OVERRIDE_;

	//! sets another skin independent font. if this is set to zero, the button uses the font of the skin.
	virtual void setOverrideFont(IGUIFont* font = 0) _IRR_OVERRIDE_;

	//! Gets the override font (if any)
	virtual IGUIFont* getOverrideFont() const _IRR_OVERRIDE_;

	//! Get the font which is used right now for drawing
	virtual IGUIFont* getActiveFont() const _IRR_OVERRIDE_;

#if IRRLICHT_VERSION_MAJOR==1 && IRRLICHT_VERSION_MINOR==9
	//! Sets another color for the button text.
	virtual void setOverrideColor(video::SColor color)  _IRR_OVERRIDE_ {};

	//! Sets an image which should be displayed on the button when it is in the given state.
	virtual void setImage(EGUI_BUTTON_IMAGE_STATE state, video::ITexture* image = 0, const core::rect<s32>& sourceRect = core::rect<s32>(0, 0, 0, 0))  _IRR_OVERRIDE_;
	//! Gets the override color
	virtual video::SColor getOverrideColor(void) const  _IRR_OVERRIDE_ { return 0; };

	//! Sets if the button text should use the override color or the color in the gui skin.
	virtual void enableOverrideColor(bool enable)  _IRR_OVERRIDE_ {};

	//! Checks if an override color is enabled
	virtual bool isOverrideColorEnabled(void) const  _IRR_OVERRIDE_ { return false; };

	virtual bool getClickControlState() const _IRR_OVERRIDE_ {
		return false;
	}

#endif
	//! Get if the shift key was pressed in last EGET_BUTTON_CLICKED event
	virtual bool getClickShiftState() const	_IRR_OVERRIDE_ {
		return ClickShiftState;
	}

	//! Sets an image which should be displayed on the button when it is in normal state.
	virtual void setImage(video::ITexture* image = 0);

	//! Sets an image which should be displayed on the button when it is in normal state.
	virtual void setImage(video::ITexture* image, const core::rect<s32>& pos);

	//! Sets an image which should be displayed on the button when it is in pressed state.
	virtual void setPressedImage(video::ITexture* image = 0);

	//! Sets an image which should be displayed on the button when it is in pressed state.
	virtual void setPressedImage(video::ITexture* image, const core::rect<s32>& pos);

	//! Sets the sprite bank used by the button
	virtual void setSpriteBank(IGUISpriteBank* bank = 0) _IRR_OVERRIDE_;

#if IRRLICHT_VERSION_MAJOR==1 && IRRLICHT_VERSION_MINOR==9
	//! Sets the animated sprite for a specific button state
	/** \param index: Number of the sprite within the sprite bank, use -1 for no sprite
	\param state: State of the button to set the sprite for
	\param index: The sprite number from the current sprite bank
	\param color: The color of the sprite
	*/
	virtual void setSprite(EGUI_BUTTON_STATE state, s32 index,
						   video::SColor color = video::SColor(255, 255, 255, 255),
						   bool loop = false, bool scale = false) _IRR_OVERRIDE_;
#else
	//! Sets the animated sprite for a specific button state
	/** \param index: Number of the sprite within the sprite bank, use -1 for no sprite
	\param state: State of the button to set the sprite for
	\param index: The sprite number from the current sprite bank
	\param color: The color of the sprite
	\param loop: True if the animation should loop, false if not
	*/
	virtual void setSprite(EGUI_BUTTON_STATE state, s32 index,
						   video::SColor color = video::SColor(255, 255, 255, 255), bool loop = false) _IRR_OVERRIDE_;
#endif

	//! Get the sprite-index for the given state or -1 when no sprite is set
	virtual s32 getSpriteIndex(EGUI_BUTTON_STATE state) const _IRR_OVERRIDE_;

	//! Get the sprite color for the given state. Color is only used when a sprite is set.
	virtual video::SColor getSpriteColor(EGUI_BUTTON_STATE state) const _IRR_OVERRIDE_;

	//! Returns if the sprite in the given state does loop
	virtual bool getSpriteLoop(EGUI_BUTTON_STATE state) const _IRR_OVERRIDE_;

	//! Returns if the sprite in the given state is scaled
	virtual bool getSpriteScale(EGUI_BUTTON_STATE state) const _IRR_OVERRIDE_;

	//! Sets if the button should behave like a push button. Which means it
	//! can be in two states: Normal or Pressed. With a click on the button,
	//! the user can change the state of the button.
	virtual void setIsPushButton(bool isPushButton = true) _IRR_OVERRIDE_;

	//! Checks whether the button is a push button
	virtual bool isPushButton() const _IRR_OVERRIDE_;

	//! Sets the pressed state of the button if this is a pushbutton
	virtual void setPressed(bool pressed = true) _IRR_OVERRIDE_;

	//! Returns if the button is currently pressed
	virtual bool isPressed() const _IRR_OVERRIDE_;

	//! Sets if the button should use the skin to draw its border
	virtual void setDrawBorder(bool border = true) _IRR_OVERRIDE_;

	//! Checks if the button face and border are being drawn
	virtual bool isDrawingBorder() const _IRR_OVERRIDE_;

	//! Sets if the alpha channel should be used for drawing images on the button (default is false)
	virtual void setUseAlphaChannel(bool useAlphaChannel = true) _IRR_OVERRIDE_;

	//! Checks if the alpha channel should be used for drawing images on the button
	virtual bool isAlphaChannelUsed() const _IRR_OVERRIDE_;

	//! Sets if the button should scale the button images to fit
	virtual void setScaleImage(bool scaleImage = true) _IRR_OVERRIDE_;

	//! Checks whether the button scales the used images
	virtual bool isScalingImage() const _IRR_OVERRIDE_;

	//! Writes attributes of the element.
	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const _IRR_OVERRIDE_;

	//! Reads attributes of the element
	virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options) _IRR_OVERRIDE_;

	virtual void setDrawImage(bool b);

	virtual void setImageRotation(f32 r);

	virtual void setImageScale(core::vector2df s);

	virtual void setImageSize(core::dimension2di s);

protected:
#if IRRLICHT_VERSION_MAJOR==1 && IRRLICHT_VERSION_MINOR==9
	void drawSprite(EGUI_BUTTON_STATE state, u32 startTime, const core::position2di& center);
#endif
	struct ButtonSprite {
		ButtonSprite() : Index(-1), Loop(false), Scale(false) {
		}

		bool operator==(const ButtonSprite& other) const {
			return Index == other.Index && Color == other.Color && Loop == other.Loop && Scale == other.Scale;
		}

		s32 Index;
		video::SColor Color;
		bool Loop;
		bool Scale;
	};

	ButtonSprite ButtonSprites[EGBS_COUNT];

	IGUISpriteBank* SpriteBank;
	IGUIFont* OverrideFont;

	video::ITexture* Image;
	video::ITexture* PressedImage;

	core::rect<s32> ImageRect;
	core::rect<s32> PressedImageRect;

	u32 ClickTime, HoverTime, FocusTime;

	bool IsPushButton;
	bool Pressed;
	bool UseAlphaChannel;
	bool DrawBorder;
	bool ScaleImage;

	bool ClickShiftState;

	bool isDrawImage;
	bool isFixedSize;
	f32 imageRotation;
	core::vector2df imageScale;
	core::dimension2di imageSize;
};

} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

#endif // __C_GUI_BUTTON_H_INCLUDED__

