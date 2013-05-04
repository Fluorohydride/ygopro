// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CGUISkin.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include "IGUIFont.h"
#include "IGUISpriteBank.h"
#include "IGUIElement.h"
#include "IVideoDriver.h"
#include "IAttributes.h"

namespace irr
{
namespace gui
{

CGUISkin::CGUISkin(EGUI_SKIN_TYPE type, video::IVideoDriver* driver)
: SpriteBank(0), Driver(driver), Type(type)
{
	#ifdef _DEBUG
	setDebugName("CGUISkin");
	#endif

	if ((Type == EGST_WINDOWS_CLASSIC) || (Type == EGST_WINDOWS_METALLIC))
	{
		Colors[EGDC_3D_DARK_SHADOW]     = video::SColor(101,50,50,50);
		Colors[EGDC_3D_SHADOW]          = video::SColor(101,130,130,130);
		Colors[EGDC_3D_FACE]            = video::SColor(101,210,210,210);
		Colors[EGDC_3D_HIGH_LIGHT]      = video::SColor(101,255,255,255);
		Colors[EGDC_3D_LIGHT]           = video::SColor(101,210,210,210);
		Colors[EGDC_ACTIVE_BORDER]      = video::SColor(101,16,14,115);
		Colors[EGDC_ACTIVE_CAPTION]     = video::SColor(255,255,255,255);
		Colors[EGDC_APP_WORKSPACE]      = video::SColor(101,100,100,100);
		Colors[EGDC_BUTTON_TEXT]        = video::SColor(240,10,10,10);
		Colors[EGDC_GRAY_TEXT]          = video::SColor(240,130,130,130);
		Colors[EGDC_HIGH_LIGHT]         = video::SColor(101,8,36,107);
		Colors[EGDC_HIGH_LIGHT_TEXT]    = video::SColor(240,255,255,255);
		Colors[EGDC_INACTIVE_BORDER]    = video::SColor(101,165,165,165);
		Colors[EGDC_INACTIVE_CAPTION]   = video::SColor(255,30,30,30);
		Colors[EGDC_TOOLTIP]            = video::SColor(200,0,0,0);
		Colors[EGDC_TOOLTIP_BACKGROUND] = video::SColor(200,255,255,225);
		Colors[EGDC_SCROLLBAR]          = video::SColor(101,230,230,230);
		Colors[EGDC_WINDOW]             = video::SColor(101,255,255,255);
		Colors[EGDC_WINDOW_SYMBOL]      = video::SColor(200,10,10,10);
		Colors[EGDC_ICON]               = video::SColor(200,255,255,255);
		Colors[EGDC_ICON_HIGH_LIGHT]    = video::SColor(200,8,36,107);
		Colors[EGDC_GRAY_WINDOW_SYMBOL] = video::SColor(240,100,100,100);
		Colors[EGDC_EDITABLE] 			= video::SColor(255,255,255,255);
		Colors[EGDC_GRAY_EDITABLE]		= video::SColor(255,120,120,120);
		Colors[EGDC_FOCUSED_EDITABLE]	= video::SColor(255,240,240,255);


		Sizes[EGDS_SCROLLBAR_SIZE] = 14;
		Sizes[EGDS_MENU_HEIGHT] = 30;
		Sizes[EGDS_WINDOW_BUTTON_WIDTH] = 15;
		Sizes[EGDS_CHECK_BOX_WIDTH] = 18;
		Sizes[EGDS_MESSAGE_BOX_WIDTH] = 500;
		Sizes[EGDS_MESSAGE_BOX_HEIGHT] = 200;
		Sizes[EGDS_BUTTON_WIDTH] = 80;
		Sizes[EGDS_BUTTON_HEIGHT] = 30;

		Sizes[EGDS_TEXT_DISTANCE_X] = 2;
		Sizes[EGDS_TEXT_DISTANCE_Y] = 0;

		Sizes[EGDS_TITLEBARTEXT_DISTANCE_X] = 2;
		Sizes[EGDS_TITLEBARTEXT_DISTANCE_Y] = 0;
	}
	else
	{
		//0x80a6a8af
		Colors[EGDC_3D_DARK_SHADOW] 	=	0x60767982;
		//Colors[EGDC_3D_FACE]			=	0xc0c9ccd4;		// tab background
		Colors[EGDC_3D_FACE]			=	0xc0cbd2d9;		// tab background
		Colors[EGDC_3D_SHADOW]			=	0x50e4e8f1;		// tab background, and left-top highlight
		Colors[EGDC_3D_HIGH_LIGHT]		=	0x40c7ccdc;
		Colors[EGDC_3D_LIGHT]			=	0x802e313a;
		Colors[EGDC_ACTIVE_BORDER]		=	0x80404040;		// window title
		Colors[EGDC_ACTIVE_CAPTION] 	=	0xffd0d0d0;
		Colors[EGDC_APP_WORKSPACE]		=	0xc0646464;		// unused
		Colors[EGDC_BUTTON_TEXT]		=	0xd0161616;
		Colors[EGDC_GRAY_TEXT]			=	0x3c141414;
		Colors[EGDC_HIGH_LIGHT]			=	0x6c606060;
		Colors[EGDC_HIGH_LIGHT_TEXT]	=	0xd0e0e0e0;
		Colors[EGDC_INACTIVE_BORDER]	=	0xf0a5a5a5;
		Colors[EGDC_INACTIVE_CAPTION]	=	0xffd2d2d2;
		Colors[EGDC_TOOLTIP]			=	0xf00f2033;
		Colors[EGDC_TOOLTIP_BACKGROUND]	= 	0xc0cbd2d9;
		Colors[EGDC_SCROLLBAR]			= 	0xf0e0e0e0;
		Colors[EGDC_WINDOW]				= 	0xf0f0f0f0;
		Colors[EGDC_WINDOW_SYMBOL]		= 	0xd0161616;
		Colors[EGDC_ICON]				= 	0xd0161616;
		Colors[EGDC_ICON_HIGH_LIGHT]	= 	0xd0606060;
		Colors[EGDC_GRAY_WINDOW_SYMBOL] = 	0x3c101010;
		Colors[EGDC_EDITABLE] 			= 	0xf0ffffff;
		Colors[EGDC_GRAY_EDITABLE]		= 	0xf0cccccc;
		Colors[EGDC_FOCUSED_EDITABLE]	= 	0xf0fffff0;

		Sizes[EGDS_SCROLLBAR_SIZE] = 14;
		Sizes[EGDS_MENU_HEIGHT] = 48;
		Sizes[EGDS_WINDOW_BUTTON_WIDTH] = 15;
		Sizes[EGDS_CHECK_BOX_WIDTH] = 18;
		Sizes[EGDS_MESSAGE_BOX_WIDTH] = 500;
		Sizes[EGDS_MESSAGE_BOX_HEIGHT] = 200;
		Sizes[EGDS_BUTTON_WIDTH] = 80;
		Sizes[EGDS_BUTTON_HEIGHT] = 30;

		Sizes[EGDS_TEXT_DISTANCE_X] = 3;
		Sizes[EGDS_TEXT_DISTANCE_Y] = 2;

		Sizes[EGDS_TITLEBARTEXT_DISTANCE_X] = 3;
		Sizes[EGDS_TITLEBARTEXT_DISTANCE_Y] = 2;
	}

	Sizes[EGDS_MESSAGE_BOX_GAP_SPACE] = 15;
	Sizes[EGDS_MESSAGE_BOX_MIN_TEXT_WIDTH] = 0;
	Sizes[EGDS_MESSAGE_BOX_MAX_TEXT_WIDTH] = 500;
	Sizes[EGDS_MESSAGE_BOX_MIN_TEXT_HEIGHT] = 0;
	Sizes[EGDS_MESSAGE_BOX_MAX_TEXT_HEIGHT] = 99999;

	Sizes[EGDS_BUTTON_PRESSED_IMAGE_OFFSET_X] = 1;
	Sizes[EGDS_BUTTON_PRESSED_IMAGE_OFFSET_Y] = 1;
	Sizes[EGDS_BUTTON_PRESSED_TEXT_OFFSET_X] = 0;
	Sizes[EGDS_BUTTON_PRESSED_TEXT_OFFSET_Y] = 2;

	Texts[EGDT_MSG_BOX_OK] = L"OK";
	Texts[EGDT_MSG_BOX_CANCEL] = L"Cancel";
	Texts[EGDT_MSG_BOX_YES] = L"Yes";
	Texts[EGDT_MSG_BOX_NO] = L"No";
	Texts[EGDT_WINDOW_CLOSE] = L"Close";
	Texts[EGDT_WINDOW_RESTORE] = L"Restore";
	Texts[EGDT_WINDOW_MINIMIZE] = L"Minimize";
	Texts[EGDT_WINDOW_MAXIMIZE] = L"Maximize";

	Icons[EGDI_WINDOW_MAXIMIZE] = 225;
	Icons[EGDI_WINDOW_RESTORE] = 226;
	Icons[EGDI_WINDOW_CLOSE] = 227;
	Icons[EGDI_WINDOW_MINIMIZE] = 228;
	Icons[EGDI_CURSOR_UP] = 229;
	Icons[EGDI_CURSOR_DOWN] = 230;
	Icons[EGDI_CURSOR_LEFT] = 231;
	Icons[EGDI_CURSOR_RIGHT] = 232;
	Icons[EGDI_MENU_MORE] = 232;
	Icons[EGDI_CHECK_BOX_CHECKED] = 233;
	Icons[EGDI_DROP_DOWN] = 234;
	Icons[EGDI_SMALL_CURSOR_UP] = 235;
	Icons[EGDI_SMALL_CURSOR_DOWN] = 236;
	Icons[EGDI_RADIO_BUTTON_CHECKED] = 237;
	Icons[EGDI_MORE_LEFT] = 238;
	Icons[EGDI_MORE_RIGHT] = 239;
	Icons[EGDI_MORE_UP] = 240;
	Icons[EGDI_MORE_DOWN] = 241;
	Icons[EGDI_WINDOW_RESIZE] = 242;
	Icons[EGDI_EXPAND] = 243;
	Icons[EGDI_COLLAPSE] = 244;

	Icons[EGDI_FILE] = 245;
	Icons[EGDI_DIRECTORY] = 246;

	for (u32 i=0; i<EGDF_COUNT; ++i)
		Fonts[i] = 0;

	UseGradient = (Type == EGST_WINDOWS_METALLIC) || (Type == EGST_BURNING_SKIN) ;
}


//! destructor
CGUISkin::~CGUISkin()
{
	for (u32 i=0; i<EGDF_COUNT; ++i)
	{
		if (Fonts[i])
			Fonts[i]->drop();
	}

	if (SpriteBank)
		SpriteBank->drop();
}


//! returns default color
video::SColor CGUISkin::getColor(EGUI_DEFAULT_COLOR color) const
{
	if ((u32)color < EGDC_COUNT)
		return Colors[color];
	else
		return video::SColor();
}


//! sets a default color
void CGUISkin::setColor(EGUI_DEFAULT_COLOR which, video::SColor newColor)
{
	if ((u32)which < EGDC_COUNT)
		Colors[which] = newColor;
}


//! returns size for the given size type
s32 CGUISkin::getSize(EGUI_DEFAULT_SIZE size) const
{
	if ((u32)size < EGDS_COUNT)
		return Sizes[size];
	else
		return 0;
}


//! sets a default size
void CGUISkin::setSize(EGUI_DEFAULT_SIZE which, s32 size)
{
	if ((u32)which < EGDS_COUNT)
		Sizes[which] = size;
}


//! returns the default font
IGUIFont* CGUISkin::getFont(EGUI_DEFAULT_FONT which) const
{
	if (((u32)which < EGDF_COUNT) && Fonts[which])
		return Fonts[which];
	else
		return Fonts[EGDF_DEFAULT];
}


//! sets a default font
void CGUISkin::setFont(IGUIFont* font, EGUI_DEFAULT_FONT which)
{
	if ((u32)which >= EGDF_COUNT)
		return;

	if (font)
	{
		font->grab();
		if (Fonts[which])
			Fonts[which]->drop();

		Fonts[which] = font;
	}
}


//! gets the sprite bank stored
IGUISpriteBank* CGUISkin::getSpriteBank() const
{
	return SpriteBank;
}


//! set a new sprite bank or remove one by passing 0
void CGUISkin::setSpriteBank(IGUISpriteBank* bank)
{
	if (bank)
		bank->grab();

	if (SpriteBank)
		SpriteBank->drop();

	SpriteBank = bank;
}


//! Returns a default icon
u32 CGUISkin::getIcon(EGUI_DEFAULT_ICON icon) const
{
	if ((u32)icon < EGDI_COUNT)
		return Icons[icon];
	else
		return 0;
}


//! Sets a default icon
void CGUISkin::setIcon(EGUI_DEFAULT_ICON icon, u32 index)
{
	if ((u32)icon < EGDI_COUNT)
		Icons[icon] = index;
}


//! Returns a default text. For example for Message box button captions:
//! "OK", "Cancel", "Yes", "No" and so on.
const wchar_t* CGUISkin::getDefaultText(EGUI_DEFAULT_TEXT text) const
{
	if ((u32)text < EGDT_COUNT)
		return Texts[text].c_str();
	else
		return Texts[0].c_str();
}


//! Sets a default text. For example for Message box button captions:
//! "OK", "Cancel", "Yes", "No" and so on.
void CGUISkin::setDefaultText(EGUI_DEFAULT_TEXT which, const wchar_t* newText)
{
	if ((u32)which < EGDT_COUNT)
		Texts[which] = newText;
}


//! draws a standard 3d button pane
/**	Used for drawing for example buttons in normal state.
It uses the colors EGDC_3D_DARK_SHADOW, EGDC_3D_HIGH_LIGHT, EGDC_3D_SHADOW and
EGDC_3D_FACE for this. See EGUI_DEFAULT_COLOR for details.
\param rect: Defining area where to draw.
\param clip: Clip area.
\param element: Pointer to the element which wishes to draw this. This parameter
is usually not used by ISkin, but can be used for example by more complex
implementations to find out how to draw the part exactly. */
void CGUISkin::draw3DButtonPaneStandard(IGUIElement* element,
					const core::rect<s32>& r,
					const core::rect<s32>* clip)
{
	if (!Driver)
		return;

	core::rect<s32> rect = r;

	if ( Type == EGST_BURNING_SKIN )
	{
		rect.UpperLeftCorner.X -= 1;
		rect.UpperLeftCorner.Y -= 1;
		rect.LowerRightCorner.X += 1;
		rect.LowerRightCorner.Y += 1;
		draw3DSunkenPane(element,
					getColor( EGDC_WINDOW ).getInterpolated( 0xFFFFFFFF, 0.9f )
					,false, true, rect, clip);
		return;
	}

	Driver->draw2DRectangle(getColor(EGDC_3D_DARK_SHADOW), rect, clip);

	rect.LowerRightCorner.X -= 1;
	rect.LowerRightCorner.Y -= 1;
	Driver->draw2DRectangle(getColor(EGDC_3D_HIGH_LIGHT), rect, clip);

	rect.UpperLeftCorner.X += 1;
	rect.UpperLeftCorner.Y += 1;
	Driver->draw2DRectangle(getColor(EGDC_3D_SHADOW), rect, clip);

	rect.LowerRightCorner.X -= 1;
	rect.LowerRightCorner.Y -= 1;

	if (!UseGradient)
	{
		Driver->draw2DRectangle(getColor(EGDC_3D_FACE), rect, clip);
	}
	else
	{
		const video::SColor c1 = getColor(EGDC_3D_FACE);
		const video::SColor c2 = c1.getInterpolated(getColor(EGDC_3D_DARK_SHADOW), 0.4f);
		Driver->draw2DRectangle(rect, c1, c1, c2, c2, clip);
	}
}


//! draws a pressed 3d button pane
/**	Used for drawing for example buttons in pressed state.
It uses the colors EGDC_3D_DARK_SHADOW, EGDC_3D_HIGH_LIGHT, EGDC_3D_SHADOW and
EGDC_3D_FACE for this. See EGUI_DEFAULT_COLOR for details.
\param rect: Defining area where to draw.
\param clip: Clip area.
\param element: Pointer to the element which wishes to draw this. This parameter
is usually not used by ISkin, but can be used for example by more complex
implementations to find out how to draw the part exactly. */
void CGUISkin::draw3DButtonPanePressed(IGUIElement* element,
					const core::rect<s32>& r,
					const core::rect<s32>* clip)
{
	if (!Driver)
		return;

	core::rect<s32> rect = r;
	Driver->draw2DRectangle(getColor(EGDC_3D_HIGH_LIGHT), rect, clip);

	rect.LowerRightCorner.X -= 1;
	rect.LowerRightCorner.Y -= 1;
	Driver->draw2DRectangle(getColor(EGDC_3D_DARK_SHADOW), rect, clip);

	rect.UpperLeftCorner.X += 1;
	rect.UpperLeftCorner.Y += 1;
	Driver->draw2DRectangle(getColor(EGDC_3D_SHADOW), rect, clip);

	rect.UpperLeftCorner.X += 1;
	rect.UpperLeftCorner.Y += 1;

	if (!UseGradient)
	{
		Driver->draw2DRectangle(getColor(EGDC_3D_FACE), rect, clip);
	}
	else
	{
		const video::SColor c1 = getColor(EGDC_3D_FACE);
		const video::SColor c2 = c1.getInterpolated(getColor(EGDC_3D_DARK_SHADOW), 0.4f);
		Driver->draw2DRectangle(rect, c1, c1, c2, c2, clip);
	}
}


//! draws a sunken 3d pane
/** Used for drawing the background of edit, combo or check boxes.
\param element: Pointer to the element which wishes to draw this. This parameter
is usually not used by ISkin, but can be used for example by more complex
implementations to find out how to draw the part exactly.
\param bgcolor: Background color.
\param flat: Specifies if the sunken pane should be flat or displayed as sunken
deep into the ground.
\param rect: Defining area where to draw.
\param clip: Clip area.	*/
void CGUISkin::draw3DSunkenPane(IGUIElement* element, video::SColor bgcolor,
				bool flat, bool fillBackGround,
				const core::rect<s32>& r,
				const core::rect<s32>* clip)
{
	if (!Driver)
		return;

	core::rect<s32> rect = r;

	if (fillBackGround)
		Driver->draw2DRectangle(bgcolor, rect, clip);

	if (flat)
	{
		// draw flat sunken pane

		rect.LowerRightCorner.Y = rect.UpperLeftCorner.Y + 1;
		Driver->draw2DRectangle(getColor(EGDC_3D_SHADOW), rect, clip);	// top

		++rect.UpperLeftCorner.Y;
		rect.LowerRightCorner.Y = r.LowerRightCorner.Y;
		rect.LowerRightCorner.X = rect.UpperLeftCorner.X + 1;
		Driver->draw2DRectangle(getColor(EGDC_3D_SHADOW), rect, clip);	// left

		rect = r;
		++rect.UpperLeftCorner.Y;
		rect.UpperLeftCorner.X = rect.LowerRightCorner.X - 1;
		Driver->draw2DRectangle(getColor(EGDC_3D_HIGH_LIGHT), rect, clip);	// right

		rect = r;
		++rect.UpperLeftCorner.X;
		rect.UpperLeftCorner.Y = r.LowerRightCorner.Y - 1;
		--rect.LowerRightCorner.X;
		Driver->draw2DRectangle(getColor(EGDC_3D_HIGH_LIGHT), rect, clip);	// bottom
	}
	else
	{
		// draw deep sunken pane
		rect.LowerRightCorner.Y = rect.UpperLeftCorner.Y + 1;
		Driver->draw2DRectangle(getColor(EGDC_3D_SHADOW), rect, clip);	// top
		++rect.UpperLeftCorner.X;
		++rect.UpperLeftCorner.Y;
		--rect.LowerRightCorner.X;
		++rect.LowerRightCorner.Y;
		Driver->draw2DRectangle(getColor(EGDC_3D_DARK_SHADOW), rect, clip);

		rect.UpperLeftCorner.X = r.UpperLeftCorner.X;
		rect.UpperLeftCorner.Y = r.UpperLeftCorner.Y+1;
		rect.LowerRightCorner.X = rect.UpperLeftCorner.X + 1;
		rect.LowerRightCorner.Y = r.LowerRightCorner.Y;
		Driver->draw2DRectangle(getColor(EGDC_3D_SHADOW), rect, clip);	// left
		++rect.UpperLeftCorner.X;
		++rect.UpperLeftCorner.Y;
		++rect.LowerRightCorner.X;
		--rect.LowerRightCorner.Y;
		Driver->draw2DRectangle(getColor(EGDC_3D_DARK_SHADOW), rect, clip);

		rect = r;
		rect.UpperLeftCorner.X = rect.LowerRightCorner.X - 1;
		++rect.UpperLeftCorner.Y;
		Driver->draw2DRectangle(getColor(EGDC_3D_HIGH_LIGHT), rect, clip);	// right
		--rect.UpperLeftCorner.X;
		++rect.UpperLeftCorner.Y;
		--rect.LowerRightCorner.X;
		--rect.LowerRightCorner.Y;
		Driver->draw2DRectangle(getColor(EGDC_3D_LIGHT), rect, clip);

		rect = r;
		++rect.UpperLeftCorner.X;
		rect.UpperLeftCorner.Y = r.LowerRightCorner.Y - 1;
		--rect.LowerRightCorner.X;
		Driver->draw2DRectangle(getColor(EGDC_3D_HIGH_LIGHT), rect, clip);	// bottom
		++rect.UpperLeftCorner.X;
		--rect.UpperLeftCorner.Y;
		--rect.LowerRightCorner.X;
		--rect.LowerRightCorner.Y;
		Driver->draw2DRectangle(getColor(EGDC_3D_LIGHT), rect, clip);
	}
}


//! draws a window background
// return where to draw title bar text.
core::rect<s32> CGUISkin::draw3DWindowBackground(IGUIElement* element,
				bool drawTitleBar, video::SColor titleBarColor,
				const core::rect<s32>& r,
				const core::rect<s32>* clip,
				core::rect<s32>* checkClientArea)
{
	if (!Driver)
	{
		if ( checkClientArea )
		{
			*checkClientArea = r;
		}
		return r;
	}

	core::rect<s32> rect = r;

	// top border
	rect.LowerRightCorner.Y = rect.UpperLeftCorner.Y + 1;
	if ( !checkClientArea )
	{
		Driver->draw2DRectangle(getColor(EGDC_3D_HIGH_LIGHT), rect, clip);
	}

	// left border
	rect.LowerRightCorner.Y = r.LowerRightCorner.Y;
	rect.LowerRightCorner.X = rect.UpperLeftCorner.X + 1;
	if ( !checkClientArea )
	{
		Driver->draw2DRectangle(getColor(EGDC_3D_HIGH_LIGHT), rect, clip);
	}

	// right border dark outer line
	rect.UpperLeftCorner.X = r.LowerRightCorner.X - 1;
	rect.LowerRightCorner.X = r.LowerRightCorner.X;
	rect.UpperLeftCorner.Y = r.UpperLeftCorner.Y;
	rect.LowerRightCorner.Y = r.LowerRightCorner.Y;
	if ( !checkClientArea )
	{
		Driver->draw2DRectangle(getColor(EGDC_3D_DARK_SHADOW), rect, clip);
	}

	// right border bright innner line
	rect.UpperLeftCorner.X -= 1;
	rect.LowerRightCorner.X -= 1;
	rect.UpperLeftCorner.Y += 1;
	rect.LowerRightCorner.Y -= 1;
	if ( !checkClientArea )
	{
		Driver->draw2DRectangle(getColor(EGDC_3D_SHADOW), rect, clip);
	}

	// bottom border dark outer line
	rect.UpperLeftCorner.X = r.UpperLeftCorner.X;
	rect.UpperLeftCorner.Y = r.LowerRightCorner.Y - 1;
	rect.LowerRightCorner.Y = r.LowerRightCorner.Y;
	rect.LowerRightCorner.X = r.LowerRightCorner.X;
	if ( !checkClientArea )
	{
		Driver->draw2DRectangle(getColor(EGDC_3D_DARK_SHADOW), rect, clip);
	}

	// bottom border bright inner line
	rect.UpperLeftCorner.X += 1;
	rect.LowerRightCorner.X -= 1;
	rect.UpperLeftCorner.Y -= 1;
	rect.LowerRightCorner.Y -= 1;
	if ( !checkClientArea )
	{
		Driver->draw2DRectangle(getColor(EGDC_3D_SHADOW), rect, clip);
	}

	// client area for background
	rect = r;
	rect.UpperLeftCorner.X +=1;
	rect.UpperLeftCorner.Y +=1;
	rect.LowerRightCorner.X -= 2;
	rect.LowerRightCorner.Y -= 2;
	if (checkClientArea)
	{
		*checkClientArea = rect;
	}

	if ( !checkClientArea )
	{
		if (!UseGradient)
		{
			Driver->draw2DRectangle(getColor(EGDC_3D_FACE), rect, clip);
		}
		else if ( Type == EGST_BURNING_SKIN )
		{
			const video::SColor c1 = getColor(EGDC_WINDOW).getInterpolated ( 0xFFFFFFFF, 0.9f );
			const video::SColor c2 = getColor(EGDC_WINDOW).getInterpolated ( 0xFFFFFFFF, 0.8f );

			Driver->draw2DRectangle(rect, c1, c1, c2, c2, clip);
		}
		else
		{
			const video::SColor c2 = getColor(EGDC_3D_SHADOW);
			const video::SColor c1 = getColor(EGDC_3D_FACE);
			Driver->draw2DRectangle(rect, c1, c1, c1, c2, clip);
		}
	}

	// title bar
	rect = r;
	rect.UpperLeftCorner.X += 2;
	rect.UpperLeftCorner.Y += 2;
	rect.LowerRightCorner.X -= 2;
	rect.LowerRightCorner.Y = rect.UpperLeftCorner.Y + getSize(EGDS_WINDOW_BUTTON_WIDTH) + 2;

	if (drawTitleBar )
	{
		if (checkClientArea)
		{
			(*checkClientArea).UpperLeftCorner.Y = rect.LowerRightCorner.Y;
		}
		else
		{
			// draw title bar
			//if (!UseGradient)
			//	Driver->draw2DRectangle(titleBarColor, rect, clip);
			//else
			if ( Type == EGST_BURNING_SKIN )
			{
				const video::SColor c = titleBarColor.getInterpolated( video::SColor(titleBarColor.getAlpha(),255,255,255), 0.8f);
				Driver->draw2DRectangle(rect, titleBarColor, titleBarColor, c, c, clip);
			}
			else
			{
				const video::SColor c = titleBarColor.getInterpolated(video::SColor(titleBarColor.getAlpha(),0,0,0), 0.2f);
				Driver->draw2DRectangle(rect, titleBarColor, c, titleBarColor, c, clip);
			}
		}
	}

	return rect;
}


//! draws a standard 3d menu pane
/**	Used for drawing for menus and context menus.
It uses the colors EGDC_3D_DARK_SHADOW, EGDC_3D_HIGH_LIGHT, EGDC_3D_SHADOW and
EGDC_3D_FACE for this. See EGUI_DEFAULT_COLOR for details.
\param element: Pointer to the element which wishes to draw this. This parameter
is usually not used by ISkin, but can be used for example by more complex
implementations to find out how to draw the part exactly.
\param rect: Defining area where to draw.
\param clip: Clip area.	*/
void CGUISkin::draw3DMenuPane(IGUIElement* element,
			const core::rect<s32>& r, const core::rect<s32>* clip)
{
	if (!Driver)
		return;

	core::rect<s32> rect = r;

	if ( Type == EGST_BURNING_SKIN )
	{
		rect.UpperLeftCorner.Y -= 3;
		draw3DButtonPaneStandard(element, rect, clip);
		return;
	}

	// in this skin, this is exactly what non pressed buttons look like,
	// so we could simply call
	// draw3DButtonPaneStandard(element, rect, clip);
	// here.
	// but if the skin is transparent, this doesn't look that nice. So
	// We draw it a little bit better, with some more draw2DRectangle calls,
	// but there aren't that much menus visible anyway.

	rect.LowerRightCorner.Y = rect.UpperLeftCorner.Y + 1;
	Driver->draw2DRectangle(getColor(EGDC_3D_HIGH_LIGHT), rect, clip);

	rect.LowerRightCorner.Y = r.LowerRightCorner.Y;
	rect.LowerRightCorner.X = rect.UpperLeftCorner.X + 1;
	Driver->draw2DRectangle(getColor(EGDC_3D_HIGH_LIGHT), rect, clip);

	rect.UpperLeftCorner.X = r.LowerRightCorner.X - 1;
	rect.LowerRightCorner.X = r.LowerRightCorner.X;
	rect.UpperLeftCorner.Y = r.UpperLeftCorner.Y;
	rect.LowerRightCorner.Y = r.LowerRightCorner.Y;
	Driver->draw2DRectangle(getColor(EGDC_3D_DARK_SHADOW), rect, clip);

	rect.UpperLeftCorner.X -= 1;
	rect.LowerRightCorner.X -= 1;
	rect.UpperLeftCorner.Y += 1;
	rect.LowerRightCorner.Y -= 1;
	Driver->draw2DRectangle(getColor(EGDC_3D_SHADOW), rect, clip);

	rect.UpperLeftCorner.X = r.UpperLeftCorner.X;
	rect.UpperLeftCorner.Y = r.LowerRightCorner.Y - 1;
	rect.LowerRightCorner.Y = r.LowerRightCorner.Y;
	rect.LowerRightCorner.X = r.LowerRightCorner.X;
	Driver->draw2DRectangle(getColor(EGDC_3D_DARK_SHADOW), rect, clip);

	rect.UpperLeftCorner.X += 1;
	rect.LowerRightCorner.X -= 1;
	rect.UpperLeftCorner.Y -= 1;
	rect.LowerRightCorner.Y -= 1;
	Driver->draw2DRectangle(getColor(EGDC_3D_SHADOW), rect, clip);

	rect = r;
	rect.UpperLeftCorner.X +=1;
	rect.UpperLeftCorner.Y +=1;
	rect.LowerRightCorner.X -= 2;
	rect.LowerRightCorner.Y -= 2;

	if (!UseGradient)
		Driver->draw2DRectangle(getColor(EGDC_3D_FACE), rect, clip);
	else
	{
		const video::SColor c1 = getColor(EGDC_3D_FACE);
		const video::SColor c2 = getColor(EGDC_3D_SHADOW);
		Driver->draw2DRectangle(rect, c1, c1, c2, c2, clip);
	}
}


//! draws a standard 3d tool bar
/**	Used for drawing for toolbars and menus.
\param element: Pointer to the element which wishes to draw this. This parameter
is usually not used by ISkin, but can be used for example by more complex
implementations to find out how to draw the part exactly.
\param rect: Defining area where to draw.
\param clip: Clip area.	*/
void CGUISkin::draw3DToolBar(IGUIElement* element,
				const core::rect<s32>& r,
				const core::rect<s32>* clip)
{
	if (!Driver)
		return;

	core::rect<s32> rect = r;

	rect.UpperLeftCorner.X = r.UpperLeftCorner.X;
	rect.UpperLeftCorner.Y = r.LowerRightCorner.Y - 1;
	rect.LowerRightCorner.Y = r.LowerRightCorner.Y;
	rect.LowerRightCorner.X = r.LowerRightCorner.X;
	Driver->draw2DRectangle(getColor(EGDC_3D_SHADOW), rect, clip);

	rect = r;
	rect.LowerRightCorner.Y -= 1;

	if (!UseGradient)
	{
		Driver->draw2DRectangle(getColor(EGDC_3D_FACE), rect, clip);
	}
	else
	if ( Type == EGST_BURNING_SKIN )
	{
		const video::SColor c1 = 0xF0000000 | getColor(EGDC_3D_FACE).color;
		const video::SColor c2 = 0xF0000000 | getColor(EGDC_3D_SHADOW).color;

		rect.LowerRightCorner.Y += 1;
		Driver->draw2DRectangle(rect, c1, c2, c1, c2, clip);
	}
	else
	{
		const video::SColor c1 = getColor(EGDC_3D_FACE);
		const video::SColor c2 = getColor(EGDC_3D_SHADOW);
		Driver->draw2DRectangle(rect, c1, c1, c2, c2, clip);
	}
}


//! draws a tab button
/**	Used for drawing for tab buttons on top of tabs.
\param element: Pointer to the element which wishes to draw this. This parameter
is usually not used by ISkin, but can be used for example by more complex
implementations to find out how to draw the part exactly.
\param active: Specifies if the tab is currently active.
\param rect: Defining area where to draw.
\param clip: Clip area.	*/
void CGUISkin::draw3DTabButton(IGUIElement* element, bool active,
	const core::rect<s32>& frameRect, const core::rect<s32>* clip, EGUI_ALIGNMENT alignment)
{
	if (!Driver)
		return;

	core::rect<s32> tr = frameRect;

	if ( alignment == EGUIA_UPPERLEFT )
	{
		tr.LowerRightCorner.X -= 2;
		tr.LowerRightCorner.Y = tr.UpperLeftCorner.Y + 1;
		tr.UpperLeftCorner.X += 1;
		Driver->draw2DRectangle(getColor(EGDC_3D_HIGH_LIGHT), tr, clip);

		// draw left highlight
		tr = frameRect;
		tr.LowerRightCorner.X = tr.UpperLeftCorner.X + 1;
		tr.UpperLeftCorner.Y += 1;
		Driver->draw2DRectangle(getColor(EGDC_3D_HIGH_LIGHT), tr, clip);

		// draw grey background
		tr = frameRect;
		tr.UpperLeftCorner.X += 1;
		tr.UpperLeftCorner.Y += 1;
		tr.LowerRightCorner.X -= 2;
		Driver->draw2DRectangle(getColor(EGDC_3D_FACE), tr, clip);

		// draw right middle gray shadow
		tr.LowerRightCorner.X += 1;
		tr.UpperLeftCorner.X = tr.LowerRightCorner.X - 1;
		Driver->draw2DRectangle(getColor(EGDC_3D_SHADOW), tr, clip);

		tr.LowerRightCorner.X += 1;
		tr.UpperLeftCorner.X += 1;
		tr.UpperLeftCorner.Y += 1;
		Driver->draw2DRectangle(getColor(EGDC_3D_DARK_SHADOW), tr, clip);
	}
	else
	{
		tr.LowerRightCorner.X -= 2;
		tr.UpperLeftCorner.Y = tr.LowerRightCorner.Y - 1;
		tr.UpperLeftCorner.X += 1;
		Driver->draw2DRectangle(getColor(EGDC_3D_HIGH_LIGHT), tr, clip);

		// draw left highlight
		tr = frameRect;
		tr.LowerRightCorner.X = tr.UpperLeftCorner.X + 1;
		tr.LowerRightCorner.Y -= 1;
		Driver->draw2DRectangle(getColor(EGDC_3D_HIGH_LIGHT), tr, clip);

		// draw grey background
		tr = frameRect;
		tr.UpperLeftCorner.X += 1;
		tr.UpperLeftCorner.Y -= 1;
		tr.LowerRightCorner.X -= 2;
		tr.LowerRightCorner.Y -= 1;
		Driver->draw2DRectangle(getColor(EGDC_3D_FACE), tr, clip);

		// draw right middle gray shadow
		tr.LowerRightCorner.X += 1;
		tr.UpperLeftCorner.X = tr.LowerRightCorner.X - 1;
		//tr.LowerRightCorner.Y -= 1;
		Driver->draw2DRectangle(getColor(EGDC_3D_SHADOW), tr, clip);

		tr.LowerRightCorner.X += 1;
		tr.UpperLeftCorner.X += 1;
		tr.LowerRightCorner.Y -= 1;
		Driver->draw2DRectangle(getColor(EGDC_3D_DARK_SHADOW), tr, clip);
	}
}


//! draws a tab control body
/**	\param element: Pointer to the element which wishes to draw this. This parameter
is usually not used by ISkin, but can be used for example by more complex
implementations to find out how to draw the part exactly.
\param border: Specifies if the border should be drawn.
\param background: Specifies if the background should be drawn.
\param rect: Defining area where to draw.
\param clip: Clip area.	*/
void CGUISkin::draw3DTabBody(IGUIElement* element, bool border, bool background,
	const core::rect<s32>& rect, const core::rect<s32>* clip, s32 tabHeight, EGUI_ALIGNMENT alignment)
{
	if (!Driver)
		return;

	core::rect<s32> tr = rect;

	if ( tabHeight == -1 )
		tabHeight = getSize(gui::EGDS_BUTTON_HEIGHT);

	// draw border.
	if (border)
	{
		if ( alignment == EGUIA_UPPERLEFT )
		{
			// draw left hightlight
			tr.UpperLeftCorner.Y += tabHeight + 2;
			tr.LowerRightCorner.X = tr.UpperLeftCorner.X + 1;
			Driver->draw2DRectangle(getColor(EGDC_3D_HIGH_LIGHT), tr, clip);

			// draw right shadow
			tr.UpperLeftCorner.X = rect.LowerRightCorner.X - 1;
			tr.LowerRightCorner.X = tr.UpperLeftCorner.X + 1;
			Driver->draw2DRectangle(getColor(EGDC_3D_SHADOW), tr, clip);

			// draw lower shadow
			tr = rect;
			tr.UpperLeftCorner.Y = tr.LowerRightCorner.Y - 1;
			Driver->draw2DRectangle(getColor(EGDC_3D_SHADOW), tr, clip);
		}
		else
		{
			// draw left hightlight
			tr.LowerRightCorner.Y -= tabHeight + 2;
			tr.LowerRightCorner.X = tr.UpperLeftCorner.X + 1;
			Driver->draw2DRectangle(getColor(EGDC_3D_HIGH_LIGHT), tr, clip);

			// draw right shadow
			tr.UpperLeftCorner.X = rect.LowerRightCorner.X - 1;
			tr.LowerRightCorner.X = tr.UpperLeftCorner.X + 1;
			Driver->draw2DRectangle(getColor(EGDC_3D_SHADOW), tr, clip);

			// draw lower shadow
			tr = rect;
			tr.LowerRightCorner.Y = tr.UpperLeftCorner.Y + 1;
			Driver->draw2DRectangle(getColor(EGDC_3D_HIGH_LIGHT), tr, clip);
		}
	}

	if (background)
	{
		if ( alignment == EGUIA_UPPERLEFT )
		{
			tr = rect;
			tr.UpperLeftCorner.Y += tabHeight + 2;
			tr.LowerRightCorner.X -= 1;
			tr.UpperLeftCorner.X += 1;
			tr.LowerRightCorner.Y -= 1;
		}
		else
		{
			tr = rect;
			tr.UpperLeftCorner.X += 1;
			tr.UpperLeftCorner.Y -= 1;
			tr.LowerRightCorner.X -= 1;
			tr.LowerRightCorner.Y -= tabHeight + 2;
			//tr.UpperLeftCorner.X += 1;
		}

		if (!UseGradient)
			Driver->draw2DRectangle(getColor(EGDC_3D_FACE), tr, clip);
		else
		{
			video::SColor c1 = getColor(EGDC_3D_FACE);
			video::SColor c2 = getColor(EGDC_3D_SHADOW);
			Driver->draw2DRectangle(tr, c1, c1, c2, c2, clip);
		}
	}
}


//! draws an icon, usually from the skin's sprite bank
/**	\param parent: Pointer to the element which wishes to draw this icon.
This parameter is usually not used by IGUISkin, but can be used for example
by more complex implementations to find out how to draw the part exactly.
\param icon: Specifies the icon to be drawn.
\param position: The position to draw the icon
\param starttime: The time at the start of the animation
\param currenttime: The present time, used to calculate the frame number
\param loop: Whether the animation should loop or not
\param clip: Clip area.	*/
void CGUISkin::drawIcon(IGUIElement* element, EGUI_DEFAULT_ICON icon,
			const core::position2di position,
			u32 starttime, u32 currenttime,
			bool loop, const core::rect<s32>* clip)
{
	if (!SpriteBank)
		return;

	bool gray = element && !element->isEnabled();
	SpriteBank->draw2DSprite(Icons[icon], position, clip,
			Colors[gray? EGDC_GRAY_WINDOW_SYMBOL : EGDC_WINDOW_SYMBOL], starttime, currenttime, loop, true);
}


EGUI_SKIN_TYPE CGUISkin::getType() const
{
	return Type;
}


//! draws a 2d rectangle.
void CGUISkin::draw2DRectangle(IGUIElement* element,
		const video::SColor &color, const core::rect<s32>& pos,
		const core::rect<s32>* clip)
{
	Driver->draw2DRectangle(color, pos, clip);
}


//! Writes attributes of the object.
//! Implement this to expose the attributes of your scene node animator for
//! scripting languages, editors, debuggers or xml serialization purposes.
void CGUISkin::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
{
	u32 i;
	for (i=0; i<EGDC_COUNT; ++i)
		out->addColor(GUISkinColorNames[i], Colors[i]);

	for (i=0; i<EGDS_COUNT; ++i)
		out->addInt(GUISkinSizeNames[i], Sizes[i]);

	for (i=0; i<EGDT_COUNT; ++i)
		out->addString(GUISkinTextNames[i], Texts[i].c_str());

	for (i=0; i<EGDI_COUNT; ++i)
		out->addInt(GUISkinIconNames[i], Icons[i]);
}


//! Reads attributes of the object.
//! Implement this to set the attributes of your scene node animator for
//! scripting languages, editors, debuggers or xml deserialization purposes.
void CGUISkin::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	// TODO: This is not nice code for downward compatibility, whenever new values are added and users
	// load an old skin the corresponding values will be set to 0.
	u32 i;
	for (i=0; i<EGDC_COUNT; ++i)
		Colors[i] = in->getAttributeAsColor(GUISkinColorNames[i]);

	for (i=0; i<EGDS_COUNT; ++i)
		Sizes[i] = in->getAttributeAsInt(GUISkinSizeNames[i]);

	for (i=0; i<EGDT_COUNT; ++i)
		Texts[i] = in->getAttributeAsStringW(GUISkinTextNames[i]);

	for (i=0; i<EGDI_COUNT; ++i)
		Icons[i] = in->getAttributeAsInt(GUISkinIconNames[i]);
}


} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

