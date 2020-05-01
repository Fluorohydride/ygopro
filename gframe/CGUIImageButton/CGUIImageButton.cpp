// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CGUIImageButton.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include <IGUISkin.h>
#include <IGUISpriteBank.h>
#include <IGUIEnvironment.h>
#include <IVideoDriver.h>
#include <IGUIFont.h>
#if IRRLICHT_VERSION_MAJOR==1 && IRRLICHT_VERSION_MINOR==9
#include "../IrrlichtCommonIncludes1.9/os.h"
#else
#include "../IrrlichtCommonIncludes/os.h"
#endif
#ifdef __ANDROID__
#include "../game.h"
#endif

namespace irr {
namespace gui {

void Draw2DImageRotation(video::IVideoDriver* driver, video::ITexture* image, core::rect<s32> sourceRect,
						 core::position2d<s32> position, core::position2d<s32> rotationPoint, f32 rotation, core::vector2df scale, bool useAlphaChannel, video::SColor color) {
	irr::video::SMaterial material;
	irr::core::matrix4 oldProjMat = driver->getTransform(irr::video::ETS_PROJECTION);
	driver->setTransform(irr::video::ETS_PROJECTION, irr::core::matrix4());
	irr::core::matrix4 oldViewMat = driver->getTransform(irr::video::ETS_VIEW);
	driver->setTransform(irr::video::ETS_VIEW, irr::core::matrix4());
	irr::core::vector2df corner[4];
	corner[0] = irr::core::vector2df(position.X, position.Y);
	corner[1] = irr::core::vector2df(position.X + sourceRect.getWidth() * scale.X, position.Y);
	corner[2] = irr::core::vector2df(position.X, position.Y + sourceRect.getHeight() * scale.Y);
	corner[3] = irr::core::vector2df(position.X + sourceRect.getWidth() * scale.X, position.Y + sourceRect.getHeight() * scale.Y);
	if(rotation != 0.0f) {
		for(int x = 0; x < 4; x++)
			corner[x].rotateBy(rotation, irr::core::vector2df(rotationPoint.X, rotationPoint.Y));
	}
	irr::core::vector2df uvCorner[4];
	uvCorner[0] = irr::core::vector2df(sourceRect.UpperLeftCorner.X, sourceRect.UpperLeftCorner.Y);
	uvCorner[1] = irr::core::vector2df(sourceRect.LowerRightCorner.X, sourceRect.UpperLeftCorner.Y);
	uvCorner[2] = irr::core::vector2df(sourceRect.UpperLeftCorner.X, sourceRect.LowerRightCorner.Y);
	uvCorner[3] = irr::core::vector2df(sourceRect.LowerRightCorner.X, sourceRect.LowerRightCorner.Y);
	for(int x = 0; x < 4; x++) {
		float uvX = uvCorner[x].X / (float)image->getOriginalSize().Width;
		float uvY = uvCorner[x].Y / (float)image->getOriginalSize().Height;
		uvCorner[x] = irr::core::vector2df(uvX, uvY);
	}
	irr::video::S3DVertex vertices[4];
	irr::u16 indices[6] = { 0, 1, 2, 3, 2, 1 };
	float screenWidth = driver->getScreenSize().Width;
	float screenHeight = driver->getScreenSize().Height;
	for(int x = 0; x < 4; x++) {
		float screenPosX = ((corner[x].X / screenWidth) - 0.5f) * 2.0f;
		float screenPosY = ((corner[x].Y / screenHeight) - 0.5f) * -2.0f;
		vertices[x].Pos = irr::core::vector3df(screenPosX, screenPosY, 1);
		vertices[x].TCoords = uvCorner[x];
		vertices[x].Color = color;
	}
	material.Lighting = false;
	material.ZWriteEnable = false;
	material.TextureLayer[0].Texture = image;
#if defined(__ANDROID__)
	if(!ygo::mainGame->isNPOTSupported) {
		material.TextureLayer[0].TextureWrapU = video::ETC_CLAMP_TO_EDGE;
		material.TextureLayer[0].TextureWrapV = video::ETC_CLAMP_TO_EDGE;
	}
	if(useAlphaChannel)
		material.MaterialType = (video::E_MATERIAL_TYPE)ygo::mainGame->ogles2TrasparentAlpha;
	else material.MaterialType = (video::E_MATERIAL_TYPE)ygo::mainGame->ogles2Solid;
#else
	if(useAlphaChannel)
		material.MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL;
	else material.MaterialType = irr::video::EMT_SOLID;
#endif

	driver->setMaterial(material);
	driver->drawIndexedTriangleList(&vertices[0], 4, &indices[0], 2);
	driver->setTransform(irr::video::ETS_PROJECTION, oldProjMat);
	driver->setTransform(irr::video::ETS_VIEW, oldViewMat);
}
void Draw2DImageQuad(video::IVideoDriver* driver, video::ITexture* image, core::rect<s32> sourceRect,
					 core::position2d<s32> corner[4], bool useAlphaChannel, video::SColor color) {
	irr::video::SMaterial material;
	irr::core::matrix4 oldProjMat = driver->getTransform(irr::video::ETS_PROJECTION);
	driver->setTransform(irr::video::ETS_PROJECTION, irr::core::matrix4());
	irr::core::matrix4 oldViewMat = driver->getTransform(irr::video::ETS_VIEW);
	driver->setTransform(irr::video::ETS_VIEW, irr::core::matrix4());
	irr::core::vector2df uvCorner[4];
	uvCorner[0] = irr::core::vector2df(sourceRect.UpperLeftCorner.X, sourceRect.UpperLeftCorner.Y);
	uvCorner[1] = irr::core::vector2df(sourceRect.LowerRightCorner.X, sourceRect.UpperLeftCorner.Y);
	uvCorner[2] = irr::core::vector2df(sourceRect.UpperLeftCorner.X, sourceRect.LowerRightCorner.Y);
	uvCorner[3] = irr::core::vector2df(sourceRect.LowerRightCorner.X, sourceRect.LowerRightCorner.Y);
	for(int x = 0; x < 4; x++) {
		float uvX = uvCorner[x].X / (float)image->getOriginalSize().Width;
		float uvY = uvCorner[x].Y / (float)image->getOriginalSize().Height;
		uvCorner[x] = irr::core::vector2df(uvX, uvY);
	}
	irr::video::S3DVertex vertices[4];
	irr::u16 indices[6] = { 0, 1, 2, 3, 2, 1 };
	float screenWidth = driver->getScreenSize().Width;
	float screenHeight = driver->getScreenSize().Height;
	for(int x = 0; x < 4; x++) {
		float screenPosX = ((corner[x].X / screenWidth) - 0.5f) * 2.0f;
		float screenPosY = ((corner[x].Y / screenHeight) - 0.5f) * -2.0f;
		vertices[x].Pos = irr::core::vector3df(screenPosX, screenPosY, 1);
		vertices[x].TCoords = uvCorner[x];
		vertices[x].Color = color;
	}
	material.Lighting = false;
	material.ZWriteEnable = false;
	material.TextureLayer[0].Texture = image;
#if defined(__ANDROID__)
	if(!ygo::mainGame->isNPOTSupported) {
		material.TextureLayer[0].TextureWrapU = video::ETC_CLAMP_TO_EDGE;
		material.TextureLayer[0].TextureWrapV = video::ETC_CLAMP_TO_EDGE;
	}
	if(useAlphaChannel)
		material.MaterialType = (video::E_MATERIAL_TYPE)ygo::mainGame->ogles2TrasparentAlpha;
	else material.MaterialType = (video::E_MATERIAL_TYPE)ygo::mainGame->ogles2Solid;
#else
	if(useAlphaChannel)
		material.MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL;
	else material.MaterialType = irr::video::EMT_SOLID;
#endif
	driver->setMaterial(material);
	driver->drawIndexedTriangleList(&vertices[0], 4, &indices[0], 2);
	driver->setTransform(irr::video::ETS_PROJECTION, oldProjMat);
	driver->setTransform(irr::video::ETS_VIEW, oldViewMat);
}
//! constructor
CGUIImageButton::CGUIImageButton(IGUIEnvironment* environment, IGUIElement* parent,
								 s32 id, core::rect<s32> rectangle, bool noclip)
	: IGUIButton(environment, parent, id, rectangle),
	SpriteBank(0), OverrideFont(0), Image(0), PressedImage(0),
	ClickTime(0), HoverTime(0), FocusTime(0),
	IsPushButton(false), Pressed(false),
	UseAlphaChannel(false), DrawBorder(true), ScaleImage(false) {
#ifdef _DEBUG
	setDebugName("CGUIImageButton");
#endif
	setNotClipped(noclip);

	// Initialize the sprites.
	for(u32 i = 0; i < EGBS_COUNT; ++i)
		ButtonSprites[i].Index = -1;

	// This element can be tabbed.
	setTabStop(true);
	setTabOrder(-1);
	isDrawImage = true;
	isFixedSize = false;
	imageRotation = 0.0f;
	imageScale = core::vector2df(1.0f, 1.0f);
	imageSize = core::dimension2di(rectangle.getWidth(), rectangle.getHeight());
}

CGUIImageButton* CGUIImageButton::addImageButton(IGUIEnvironment *env, const core::rect<s32>& rectangle, IGUIElement* parent, s32 id) {
	CGUIImageButton* button = new CGUIImageButton(env, parent ? parent : 0, id, rectangle);
	button->drop();
	return button;
}

//! destructor
CGUIImageButton::~CGUIImageButton() {
	if(OverrideFont)
		OverrideFont->drop();

	if(Image)
		Image->drop();

	if(PressedImage)
		PressedImage->drop();

	if(SpriteBank)
		SpriteBank->drop();
}


//! Sets if the images should be scaled to fit the button
void CGUIImageButton::setScaleImage(bool scaleImage) {
	ScaleImage = scaleImage;
}


//! Returns whether the button scale the used images
bool CGUIImageButton::isScalingImage() const {
	return ScaleImage;
}


//! Sets if the button should use the skin to draw its border
void CGUIImageButton::setDrawBorder(bool border) {
	DrawBorder = border;
}


void CGUIImageButton::setSpriteBank(IGUISpriteBank* sprites) {
	if(sprites)
		sprites->grab();

	if(SpriteBank)
		SpriteBank->drop();

	SpriteBank = sprites;
}


#if IRRLICHT_VERSION_MAJOR==1 && IRRLICHT_VERSION_MINOR==9
void CGUIImageButton::setSprite(EGUI_BUTTON_STATE state, s32 index, video::SColor color, bool loop, bool scale) {
	ButtonSprites[(u32)state].Index = index;
	ButtonSprites[(u32)state].Color = color;
	ButtonSprites[(u32)state].Loop = loop;
	ButtonSprites[(u32)state].Scale = scale;
}
#else
void CGUIImageButton::setSprite(EGUI_BUTTON_STATE state, s32 index, video::SColor color, bool loop) {
	if(SpriteBank) {
		ButtonSprites[(u32)state].Index = index;
		ButtonSprites[(u32)state].Color = color;
		ButtonSprites[(u32)state].Loop = loop;
	} else {
		ButtonSprites[(u32)state].Index = -1;
	}
}
#endif

//! Get the sprite-index for the given state or -1 when no sprite is set
s32 CGUIImageButton::getSpriteIndex(EGUI_BUTTON_STATE state) const {
	return ButtonSprites[(u32)state].Index;
}

//! Get the sprite color for the given state. Color is only used when a sprite is set.
video::SColor CGUIImageButton::getSpriteColor(EGUI_BUTTON_STATE state) const {
	return ButtonSprites[(u32)state].Color;
}

//! Returns if the sprite in the given state does loop
bool CGUIImageButton::getSpriteLoop(EGUI_BUTTON_STATE state) const {
	return ButtonSprites[(u32)state].Loop;
}

//! Returns if the sprite in the given state is scaled
bool CGUIImageButton::getSpriteScale(EGUI_BUTTON_STATE state) const {
	return ButtonSprites[(u32)state].Scale;
}

//! called if an event happened.
bool CGUIImageButton::OnEvent(const SEvent& event) {
	if(!isEnabled())
		return IGUIElement::OnEvent(event);

	switch(event.EventType) {
		case EET_KEY_INPUT_EVENT:
			if(event.KeyInput.PressedDown &&
				(event.KeyInput.Key == KEY_RETURN || event.KeyInput.Key == KEY_SPACE)) {
				if(!IsPushButton)
					setPressed(true);
				else
					setPressed(!Pressed);

				return true;
			}
			if(Pressed && !IsPushButton && event.KeyInput.PressedDown && event.KeyInput.Key == KEY_ESCAPE) {
				setPressed(false);
				return true;
			} else
				if(!event.KeyInput.PressedDown && Pressed &&
					(event.KeyInput.Key == KEY_RETURN || event.KeyInput.Key == KEY_SPACE)) {

					if(!IsPushButton)
						setPressed(false);

					if(Parent) {
						SEvent newEvent;
						newEvent.EventType = EET_GUI_EVENT;
						newEvent.GUIEvent.Caller = this;
						newEvent.GUIEvent.Element = 0;
						newEvent.GUIEvent.EventType = EGET_BUTTON_CLICKED;
						Parent->OnEvent(newEvent);
					}
					return true;
				}
			break;
		case EET_GUI_EVENT:
			if(event.GUIEvent.Caller == this) {
				if(event.GUIEvent.EventType == EGET_ELEMENT_FOCUS_LOST) {
					if(!IsPushButton)
						setPressed(false);
					FocusTime = os::Timer::getTime();
				} else if(event.GUIEvent.EventType == EGET_ELEMENT_FOCUSED) {
					FocusTime = os::Timer::getTime();
				} else if(event.GUIEvent.EventType == EGET_ELEMENT_HOVERED || event.GUIEvent.EventType == EGET_ELEMENT_LEFT) {
					HoverTime = os::Timer::getTime();
				}
			}
			break;
		case EET_MOUSE_INPUT_EVENT:
			if(event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN) {
				if(Environment->hasFocus(this) &&
				   !AbsoluteClippingRect.isPointInside(core::position2d<s32>(event.MouseInput.X, event.MouseInput.Y))) {
					Environment->removeFocus(this);
					return false;
				}

				if(!IsPushButton)
					setPressed(true);

				Environment->setFocus(this);
				return true;
			} else
				if(event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP) {
					bool wasPressed = Pressed;

					if(!AbsoluteClippingRect.isPointInside(core::position2d<s32>(event.MouseInput.X, event.MouseInput.Y))) {
						if(!IsPushButton)
							setPressed(false);
						return true;
					}

					if(!IsPushButton)
						setPressed(false);
					else {
						setPressed(!Pressed);
					}

					if((!IsPushButton && wasPressed && Parent) ||
						(IsPushButton && wasPressed != Pressed)) {
						SEvent newEvent;
						newEvent.EventType = EET_GUI_EVENT;
						newEvent.GUIEvent.Caller = this;
						newEvent.GUIEvent.Element = 0;
						newEvent.GUIEvent.EventType = EGET_BUTTON_CLICKED;
						Parent->OnEvent(newEvent);
					}

					return true;
				}
			break;
		default:
			break;
	}

	return Parent ? Parent->OnEvent(event) : false;
}


//! draws the element and its children
void CGUIImageButton::draw() {
	if(!IsVisible)
		return;
	IGUISkin* skin = Environment->getSkin();
	video::IVideoDriver* driver = Environment->getVideoDriver();
	core::position2di center = AbsoluteRect.getCenter();
	core::position2di pos = center;
	pos.X -= (s32)(ImageRect.getWidth() * imageScale.X * 0.5f);
	pos.Y -= (s32)(ImageRect.getHeight() * imageScale.Y * 0.5f);
	if(Pressed) {
		pos.X += 1;
		pos.Y += 1;
		center.X += 1;
		center.Y += 1;
		if(DrawBorder)
			skin->draw3DButtonPanePressed(this, AbsoluteRect, &AbsoluteClippingRect);
	} else {
		if(DrawBorder)
			skin->draw3DButtonPaneStandard(this, AbsoluteRect, &AbsoluteClippingRect);
	}
	if(Image && isDrawImage)
		irr::gui::Draw2DImageRotation(driver, Image, ImageRect, pos, center, imageRotation, imageScale);
	IGUIElement::draw();
}

#if IRRLICHT_VERSION_MAJOR==1 && IRRLICHT_VERSION_MINOR==9
void CGUIImageButton::drawSprite(EGUI_BUTTON_STATE state, u32 startTime, const core::position2di& center) {
	u32 stateIdx = (u32)state;

	if(ButtonSprites[stateIdx].Index != -1) {
		if(ButtonSprites[stateIdx].Scale) {
			const video::SColor colors[] = { ButtonSprites[stateIdx].Color,ButtonSprites[stateIdx].Color,ButtonSprites[stateIdx].Color,ButtonSprites[stateIdx].Color };
			SpriteBank->draw2DSprite(ButtonSprites[stateIdx].Index, AbsoluteRect,
									 &AbsoluteClippingRect, colors,
									 os::Timer::getTime() - startTime, ButtonSprites[stateIdx].Loop);
		} else {
			SpriteBank->draw2DSprite(ButtonSprites[stateIdx].Index, center,
									 &AbsoluteClippingRect, ButtonSprites[stateIdx].Color, startTime, os::Timer::getTime(),
									 ButtonSprites[stateIdx].Loop, true);
		}
	}
}
#endif

//! sets another skin independent font. if this is set to zero, the button uses the font of the skin.
void CGUIImageButton::setOverrideFont(IGUIFont* font) {
	if(OverrideFont == font)
		return;

	if(OverrideFont)
		OverrideFont->drop();

	OverrideFont = font;

	if(OverrideFont)
		OverrideFont->grab();
}

IGUIFont* CGUIImageButton::getOverrideFont(void) const {
	IGUISkin* skin = Environment->getSkin();
	if(!skin)
		return NULL;
	return skin->getFont();
}

IGUIFont* CGUIImageButton::getActiveFont() const {
	IGUISkin* skin = Environment->getSkin();
	if(!skin)
		return NULL;
	return skin->getFont();
}

#if IRRLICHT_VERSION_MAJOR==1 && IRRLICHT_VERSION_MINOR==9
void CGUIImageButton::setImage(EGUI_BUTTON_IMAGE_STATE state, video::ITexture* image, const core::rect<s32>& sourceRect) {
	setImage(image);
	ImageRect = sourceRect;
}
#endif

//! Sets an image which should be displayed on the button when it is in normal state.
void CGUIImageButton::setImage(video::ITexture* image) {
	if(image)
		image->grab();
	if(Image)
		Image->drop();

	Image = image;
	if(image) {
		ImageRect = core::rect<s32>(core::position2d<s32>(0, 0), image->getOriginalSize());
		if(isFixedSize)
			imageScale = core::vector2df((irr::f32)imageSize.Width / image->getSize().Width, (irr::f32)imageSize.Height / image->getSize().Height);
	}

	if(!PressedImage)
		setPressedImage(Image);
}


//! Sets the image which should be displayed on the button when it is in its normal state.
void CGUIImageButton::setImage(video::ITexture* image, const core::rect<s32>& pos) {
	setImage(image);
	ImageRect = pos;
}

void CGUIImageButton::setDrawImage(bool b) {
	isDrawImage = b;
}

void CGUIImageButton::setImageRotation(f32 r) {
	imageRotation = r;
}

void CGUIImageButton::setImageScale(core::vector2df s) {
	imageScale = s;
}

void CGUIImageButton::setImageSize(core::dimension2di s) {
	isFixedSize = true;
	imageSize = s;
}


//! Sets an image which should be displayed on the button when it is in pressed state.
void CGUIImageButton::setPressedImage(video::ITexture* image) {
	if(image)
		image->grab();

	if(PressedImage)
		PressedImage->drop();

	PressedImage = image;
	if(image)
		PressedImageRect = core::rect<s32>(core::position2d<s32>(0, 0), image->getOriginalSize());
}


//! Sets the image which should be displayed on the button when it is in its pressed state.
void CGUIImageButton::setPressedImage(video::ITexture* image, const core::rect<s32>& pos) {
	setPressedImage(image);
	PressedImageRect = pos;
}


//! Sets if the button should behave like a push button. Which means it
//! can be in two states: Normal or Pressed. With a click on the button,
//! the user can change the state of the button.
void CGUIImageButton::setIsPushButton(bool isPushButton) {
	IsPushButton = isPushButton;
}


//! Returns if the button is currently pressed
bool CGUIImageButton::isPressed() const {
	return Pressed;
}


//! Sets the pressed state of the button if this is a pushbutton
void CGUIImageButton::setPressed(bool pressed) {
	if(Pressed != pressed) {
		ClickTime = os::Timer::getTime();
		Pressed = pressed;
	}
}


//! Returns whether the button is a push button
bool CGUIImageButton::isPushButton() const {
	return IsPushButton;
}


//! Sets if the alpha channel should be used for drawing images on the button (default is false)
void CGUIImageButton::setUseAlphaChannel(bool useAlphaChannel) {
	UseAlphaChannel = useAlphaChannel;
}


//! Returns if the alpha channel should be used for drawing images on the button
bool CGUIImageButton::isAlphaChannelUsed() const {
	return UseAlphaChannel;
}


bool CGUIImageButton::isDrawingBorder() const {
	return DrawBorder;
}


//! Writes attributes of the element.
void CGUIImageButton::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options = 0) const {
	IGUIButton::serializeAttributes(out, options);

	out->addBool("PushButton", IsPushButton);
	if(IsPushButton)
		out->addBool("Pressed", Pressed);

	out->addTexture("Image", Image);
	out->addRect("ImageRect", ImageRect);
	out->addTexture("PressedImage", PressedImage);
	out->addRect("PressedImageRect", PressedImageRect);

	out->addBool("UseAlphaChannel", isAlphaChannelUsed());
	out->addBool("Border", isDrawingBorder());
	out->addBool("ScaleImage", isScalingImage());

	//   out->addString  ("OverrideFont",	OverrideFont);
}


//! Reads attributes of the element
void CGUIImageButton::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options = 0) {
	IGUIButton::deserializeAttributes(in, options);

	IsPushButton = in->getAttributeAsBool("PushButton");
	Pressed = IsPushButton ? in->getAttributeAsBool("Pressed") : false;

	core::rect<s32> rec = in->getAttributeAsRect("ImageRect");
	if(rec.isValid())
		setImage(in->getAttributeAsTexture("Image"), rec);
	else
		setImage(in->getAttributeAsTexture("Image"));

	rec = in->getAttributeAsRect("PressedImageRect");
	if(rec.isValid())
		setPressedImage(in->getAttributeAsTexture("PressedImage"), rec);
	else
		setPressedImage(in->getAttributeAsTexture("PressedImage"));

	setDrawBorder(in->getAttributeAsBool("Border"));
	setUseAlphaChannel(in->getAttributeAsBool("UseAlphaChannel"));
	setScaleImage(in->getAttributeAsBool("ScaleImage"));

	//   setOverrideFont(in->getAttributeAsString("OverrideFont"));

	updateAbsolutePosition();
}


} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

