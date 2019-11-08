#include "CImageGUISkin.h"
#include <IVideoDriver.h>
#include <ITexture.h>
#include <IGUIElement.h>
#include "clipRects.h"

namespace irr
{
namespace gui
{

CImageGUISkin::CImageGUISkin( video::IVideoDriver* driver, IGUISkin* fallbackSkin )
{
    VideoDriver = driver;
    FallbackSkin = fallbackSkin;
    FallbackSkin->grab();
}

CImageGUISkin::~CImageGUISkin()
{
    FallbackSkin->drop();
	properties.clear();

}

void CImageGUISkin::setProperty(core::stringw key, core::stringw value) {
	properties.set(key,value);
}
core::stringw CImageGUISkin::getProperty(core::stringw key) {
	core::map<core::stringw,core::stringw>::Node* node = properties.find(key);
	if(node == 0) 
		return core::stringw("");
	else return node->getValue();

}
void CImageGUISkin::loadConfig( const SImageGUISkinConfig& config )
{
    Config = config;
}

video::SColor CImageGUISkin::getColor(EGUI_DEFAULT_COLOR color) const
{
    return FallbackSkin->getColor(color);
}

void CImageGUISkin::setColor( EGUI_DEFAULT_COLOR which, video::SColor newColor )
{
    FallbackSkin->setColor(which, newColor);
}

s32 CImageGUISkin::getSize(EGUI_DEFAULT_SIZE size) const
{
    return FallbackSkin->getSize(size);
}

const wchar_t* CImageGUISkin::getDefaultText(EGUI_DEFAULT_TEXT text) const
{
    return FallbackSkin->getDefaultText(text);
}

void CImageGUISkin::setDefaultText(EGUI_DEFAULT_TEXT which, const wchar_t* newText)
{
    FallbackSkin->setDefaultText(which, newText);
}


void CImageGUISkin::setSize(EGUI_DEFAULT_SIZE which, s32 size)
{
    FallbackSkin->setSize(which, size);
}

IGUIFont* CImageGUISkin::getFont(EGUI_DEFAULT_FONT defaultFont) const
{
    return FallbackSkin->getFont(defaultFont);
}

void CImageGUISkin::setFont(IGUIFont* font, EGUI_DEFAULT_FONT defaultFont)
{
    FallbackSkin->setFont(font, defaultFont);
}

IGUISpriteBank* CImageGUISkin::getSpriteBank() const
{
    return FallbackSkin->getSpriteBank();
}

void CImageGUISkin::setSpriteBank( IGUISpriteBank* bank )
{
    FallbackSkin->setSpriteBank(bank);
}

u32 CImageGUISkin::getIcon( EGUI_DEFAULT_ICON icon ) const
{
    return FallbackSkin->getIcon(icon);
}

void CImageGUISkin::setIcon( EGUI_DEFAULT_ICON icon, u32 index )
{
    FallbackSkin->setIcon(icon, index);
}

void CImageGUISkin::draw3DButtonPaneStandard( IGUIElement* element, const core::rect<s32>& rect, const core::rect<s32>* clip )
{
	if(element->isEnabled())
	{
		if ( !Config.Button.Texture )
		{
			FallbackSkin->draw3DButtonPaneStandard( element, rect, clip );
			return;
		}

		drawElementStyle( Config.Button, rect, clip );
	}
	else
	{
		if ( !Config.ButtonDisabled.Texture )
		{
			FallbackSkin->draw3DButtonPaneStandard( element, rect, clip );
			return;
		}

		drawElementStyle( Config.ButtonDisabled, rect, clip );
	}    
}

void CImageGUISkin::draw3DButtonPanePressed( IGUIElement* element, const core::rect<s32>& rect, const core::rect<s32>* clip )
{
	if ( !Config.Button.Texture )
	{
		FallbackSkin->draw3DButtonPanePressed( element, rect, clip );
		return;
	}

	drawElementStyle( Config.ButtonPressed, rect, clip );
}

void CImageGUISkin::draw3DSunkenPane(IGUIElement* element,
            video::SColor bgcolor, bool flat, bool fillBackGround,
            const core::rect<s32>& rect,
            const core::rect<s32>* clip)
{
	if(element->getType() == EGUIET_MENU)
	{
		if ( !Config.MenuPressed.Texture )
		{
			FallbackSkin->draw3DSunkenPane(element, bgcolor, flat, fillBackGround, rect, clip);
			return;
		}

		drawElementStyle( Config.MenuPressed, rect, clip );
	}
	else if (element->getType() == EGUIET_CHECK_BOX)
	{
		if(element->isEnabled())
		{
			if ( !Config.CheckBox.Texture )
			{
				FallbackSkin->draw3DSunkenPane(element, bgcolor, flat, fillBackGround, rect, clip);
				return;
			}

			drawElementStyle( Config.CheckBox, rect, clip );
		}
		else
		{
			if ( !Config.CheckBoxDisabled.Texture )
			{
				FallbackSkin->draw3DSunkenPane(element, bgcolor, flat, fillBackGround, rect, clip);
				return;
			}

			drawElementStyle( Config.CheckBoxDisabled, rect, clip );
		}
	}
	else if (element->getType() == EGUIET_COMBO_BOX)
	{
		if(element->isEnabled())
		{
			if ( !Config.ComboBox.Texture )
			{
				FallbackSkin->draw3DSunkenPane(element, bgcolor, flat, fillBackGround, rect, clip);
				return;
			}

			if(fillBackGround)
				FallbackSkin->draw2DRectangle(element, bgcolor, rect, clip);

			drawElementStyle( Config.ComboBox, rect, clip );	
		}
		else
		{
			if ( !Config.ComboBoxDisabled.Texture )
			{
				FallbackSkin->draw3DSunkenPane(element, bgcolor, flat, fillBackGround, rect, clip);
				return;
			}

			if(fillBackGround)
				FallbackSkin->draw2DRectangle(element, bgcolor, rect, clip);

			drawElementStyle( Config.ComboBoxDisabled, rect, clip );	
		}
	}
	else
	{
		if ( !Config.SunkenPane.Texture )
		{
			FallbackSkin->draw3DSunkenPane(element, bgcolor, flat, fillBackGround, rect, clip);
			return;
		}

		if(fillBackGround)
			FallbackSkin->draw2DRectangle(element, bgcolor, rect, clip);

		drawElementStyle( Config.SunkenPane, rect, clip );		
	}
}

// 1.7 updates by Mamnarock
core::rect<s32> CImageGUISkin::draw3DWindowBackground(IGUIElement* element, 
            bool drawTitleBar, video::SColor titleBarColor, 
            const core::rect<s32>& rect, 
            const core::rect<s32>* clip, 
         core::rect<s32>* checkClientArea)
{
    if ( !Config.Window.Texture )
    {
        return FallbackSkin->draw3DWindowBackground(element, drawTitleBar, titleBarColor, rect, clip );
    }

	drawElementStyle( Config.Window, rect, clip );
    
    return core::rect<s32>( rect.UpperLeftCorner.X+Config.Window.DstBorder.Left, rect.UpperLeftCorner.Y, rect.LowerRightCorner.X-Config.Window.DstBorder.Right, rect.UpperLeftCorner.Y+Config.Window.DstBorder.Top );
}

void CImageGUISkin::draw3DMenuPane(IGUIElement* element,
            const core::rect<s32>& rect,
            const core::rect<s32>* clip)
{
    if ( !Config.MenuPane.Texture )
    {
        FallbackSkin->draw3DToolBar( element, rect, clip );
        return;
    }

	drawElementStyle( Config.MenuPane, rect, clip );
}

void CImageGUISkin::draw3DToolBar(IGUIElement* element,
    const core::rect<s32>& rect,
    const core::rect<s32>* clip)
{
	if ( !Config.MenuBar.Texture )
    {
        FallbackSkin->draw3DToolBar( element, rect, clip );
        return;
    }

	drawElementStyle( Config.MenuBar, rect, clip );
}

void CImageGUISkin::draw3DTabButton(IGUIElement* element, bool active,
	const core::rect<s32>& rect, const core::rect<s32>* clip,  gui::EGUI_ALIGNMENT alignment)
{
	if(active)
	{
		if ( !Config.TabButtonPressed.Texture )
		{
			FallbackSkin->draw3DTabButton(element, active, rect, clip, alignment);
			return;
		}

		drawElementStyle( Config.TabButtonPressed, rect, clip );
	}
	else
	{
		if ( !Config.TabButton.Texture )
		{
			FallbackSkin->draw3DTabButton(element, active, rect, clip, alignment);
			return;
		}

		drawElementStyle( Config.TabButton, rect, clip );
	}
}

void CImageGUISkin::draw3DTabBody(IGUIElement* element, bool border, bool background,
	const core::rect<s32>& rect, const core::rect<s32>* clip, s32 tabHeight, gui::EGUI_ALIGNMENT alignment )
{
	core::rect<s32> newclip,newrect;
	

	if ( !Config.TabBody.Texture )
    {
       FallbackSkin->draw3DTabBody(element, border, background, rect, clip, tabHeight, alignment);
        return;
    }
	newclip.LowerRightCorner.set(clip->LowerRightCorner);
	newclip.UpperLeftCorner.set(clip->UpperLeftCorner.X,clip->UpperLeftCorner.Y+tabHeight);
	
	
	newrect.LowerRightCorner.set(rect.LowerRightCorner);
	newrect.UpperLeftCorner.set(rect.UpperLeftCorner.X,rect.LowerRightCorner.Y+tabHeight);
	
	drawElementStyle( Config.TabBody, newclip, &newclip );
	
}

void CImageGUISkin::drawIcon(IGUIElement* element, EGUI_DEFAULT_ICON icon,
    const core::position2di position, u32 starttime, u32 currenttime, 
    bool loop, const core::rect<s32>* clip)
{
	video::SColor Color = getColor(gui::EGDC_WINDOW_SYMBOL);

	if(icon == gui::EGDI_CHECK_BOX_CHECKED)
	{
		if(Config.CheckBoxColor.getAlpha() > 0)
			Color = Config.CheckBoxColor;
	}

	// Patch to make checkbox and radio color come from gui::EGDC_WINDOW_SYMBOL -- Madoc
	if(icon == gui::EGDI_CHECK_BOX_CHECKED || icon == gui::EGDI_RADIO_BUTTON_CHECKED) 
	{
		FallbackSkin->getSpriteBank()->draw2DSprite( 
			FallbackSkin->getIcon(icon),
			position,
			clip,
			Color,
			starttime,
			currenttime,
			loop,
			true);
	}

    else FallbackSkin->drawIcon(element,icon,position,starttime,currenttime,loop,clip);
}

void CImageGUISkin::drawHorizontalProgressBar( IGUIElement* element, const core::rect<s32>& rectangle, const core::rect<s32>* clip,
            f32 filledRatio, video::SColor fillColor, video::SColor emptyColor )
{
    if ( !Config.ProgressBar.Texture || !Config.ProgressBarFilled.Texture )
    {
        return;
    }

    // Draw empty progress bar
    drawElementStyle( Config.ProgressBar, rectangle, clip, &emptyColor );

    // Draw filled progress bar on top
    if ( filledRatio < 0.0f )
        filledRatio = 0.0f;
    else
    if ( filledRatio > 1.0f )
        filledRatio = 1.0f;

    if ( filledRatio > 0.0f )
    {
        s32 filledPixels = (s32)( filledRatio * rectangle.getSize().Width );
        s32 height = rectangle.getSize().Height;

        core::rect<s32> clipRect = clip? *clip:rectangle;
        if ( filledPixels < height )
        {
            if ( clipRect.LowerRightCorner.X > rectangle.UpperLeftCorner.X + filledPixels )
                clipRect.LowerRightCorner.X = rectangle.UpperLeftCorner.X + filledPixels;

            filledPixels = height;
        }

        core::rect<s32> filledRect = core::rect<s32>( 
            rectangle.UpperLeftCorner.X, 
            rectangle.UpperLeftCorner.Y, 
            rectangle.UpperLeftCorner.X + filledPixels, 
            rectangle.LowerRightCorner.Y );
        
        drawElementStyle( Config.ProgressBarFilled, filledRect, &clipRect, &fillColor );
    }
}

void CImageGUISkin::drawElementStyle( const SImageGUIElementStyle& elem, const core::rect<s32>& rect, const core::rect<s32>* clip, video::SColor* pcolor  )
{
    core::rect<s32> srcRect;
    core::rect<s32> dstRect;
	
	core::dimension2du tsize = elem.Texture->getSize();
    video::ITexture* texture = elem.Texture;
    
    video::SColor color = elem.Color;
    if ( pcolor )
        color = *pcolor;

    video::SColor faceColor = getColor(EGDC_3D_FACE);
    color.setRed( (u8)(color.getRed() * faceColor.getRed() / 255) );
    color.setGreen( (u8)(color.getGreen() * faceColor.getGreen() / 255) );
    color.setBlue( (u8)(color.getBlue() * faceColor.getBlue() / 255) );
    color.setAlpha( (u8)(color.getAlpha() * faceColor.getAlpha() / 255 ) );

    video::SColor colors [4] = { color, color, color, color };

    core::dimension2di dstSize = rect.getSize();

    // Scale the border if there is insufficient room
    SImageGUIElementStyle::SBorder dst = elem.DstBorder;
    f32 scale = 1.0f;
    if ( dstSize.Width < dst.Left + dst.Right )
    {
        scale = dstSize.Width / (f32)( dst.Left + dst.Right );
    }
    if ( dstSize.Height < dst.Top + dst.Bottom )
    {
        f32 x = dstSize.Height / (f32)( dst.Top + dst.Bottom );
        if ( x < scale )
        {
            scale = x;
        }
    }

    if ( scale < 1.0f )
    {
        dst.Left = (s32)( dst.Left * scale );
        dst.Right = (s32)( dst.Right * scale );
        dst.Top = (s32)( dst.Top * scale );
        dst.Bottom = (s32)( dst.Bottom * scale );
    }

    const SImageGUIElementStyle::SBorder& src = elem.SrcBorder;

    // Draw the top left corner
    srcRect = core::rect<s32>( 0, 0, src.Left, src.Top );
    dstRect = core::rect<s32>( rect.UpperLeftCorner.X, rect.UpperLeftCorner.Y, rect.UpperLeftCorner.X+dst.Left, rect.UpperLeftCorner.Y+dst.Top );
    if ( !clip || clipRects( dstRect, srcRect, *clip ) )
        VideoDriver->draw2DImage( texture, dstRect, srcRect, clip, colors, true );

    // Draw the top right corner
    srcRect = core::rect<s32>( tsize.Width-src.Right, 0, tsize.Width, src.Top );
    dstRect = core::rect<s32>( rect.LowerRightCorner.X-dst.Right, rect.UpperLeftCorner.Y, rect.LowerRightCorner.X, rect.UpperLeftCorner.Y+dst.Top );
    if ( !clip || clipRects( dstRect, srcRect, *clip ) )
        VideoDriver->draw2DImage( texture, dstRect, srcRect, clip, colors, true );

    // Draw the top border
    srcRect = core::rect<s32>( src.Left, 0, tsize.Width-src.Right, src.Top );
    dstRect = core::rect<s32>( rect.UpperLeftCorner.X+dst.Left, rect.UpperLeftCorner.Y, rect.LowerRightCorner.X-dst.Right, rect.UpperLeftCorner.Y+dst.Top );
    if ( !clip || clipRects( dstRect, srcRect, *clip ) )
        VideoDriver->draw2DImage( texture, dstRect, srcRect, clip, colors, true );

    // Draw the left border
    srcRect = core::rect<s32>( 0, src.Top, src.Left, tsize.Height-src.Bottom );
    dstRect = core::rect<s32>( rect.UpperLeftCorner.X, rect.UpperLeftCorner.Y+dst.Top, rect.UpperLeftCorner.X+dst.Left, rect.LowerRightCorner.Y-dst.Bottom );
    if ( !clip || clipRects( dstRect, srcRect, *clip ) )
        VideoDriver->draw2DImage( texture, dstRect, srcRect, clip, colors, true );

    // Draw the right border
    srcRect = core::rect<s32>( tsize.Width-src.Right, src.Top, tsize.Width, tsize.Height-src.Bottom );
    dstRect = core::rect<s32>( rect.LowerRightCorner.X-dst.Right, rect.UpperLeftCorner.Y+dst.Top, rect.LowerRightCorner.X, rect.LowerRightCorner.Y-dst.Bottom );
    if ( !clip || clipRects( dstRect, srcRect, *clip ) )
        VideoDriver->draw2DImage( texture, dstRect, srcRect, clip, colors, true );

    // Draw the middle section
    srcRect = core::rect<s32>( src.Left, src.Top, tsize.Width-src.Right, tsize.Height-src.Bottom );
    dstRect = core::rect<s32>( rect.UpperLeftCorner.X+dst.Left, rect.UpperLeftCorner.Y+dst.Top, rect.LowerRightCorner.X-dst.Right, rect.LowerRightCorner.Y-dst.Bottom );
    if ( !clip || clipRects( dstRect, srcRect, *clip ) )
        VideoDriver->draw2DImage( texture, dstRect, srcRect, clip, colors, true );

    // Draw the bottom left corner
    srcRect = core::rect<s32>( 0, tsize.Height-src.Bottom, src.Left, tsize.Height );
    dstRect = core::rect<s32>( rect.UpperLeftCorner.X, rect.LowerRightCorner.Y-dst.Bottom, rect.UpperLeftCorner.X+dst.Left, rect.LowerRightCorner.Y );
    if ( !clip || clipRects( dstRect, srcRect, *clip ) )
        VideoDriver->draw2DImage( texture, dstRect, srcRect, clip, colors, true );

    // Draw the bottom right corner
    srcRect = core::rect<s32>( tsize.Width-src.Right, tsize.Height-src.Bottom, tsize.Width, tsize.Height );
    dstRect = core::rect<s32>( rect.LowerRightCorner.X-dst.Right, rect.LowerRightCorner.Y-dst.Bottom, rect.LowerRightCorner.X, rect.LowerRightCorner.Y );
    if ( !clip || clipRects( dstRect, srcRect, *clip ) )
        VideoDriver->draw2DImage( texture, dstRect, srcRect, clip, colors, true );

    // Draw the bottom border
    srcRect = core::rect<s32>( src.Left, tsize.Height-src.Bottom, tsize.Width-src.Right, tsize.Height );
    dstRect = core::rect<s32>( rect.UpperLeftCorner.X+dst.Left, rect.LowerRightCorner.Y-dst.Bottom, rect.LowerRightCorner.X-dst.Right, rect.LowerRightCorner.Y );
    if ( !clip || clipRects( dstRect, srcRect, *clip ) )
        VideoDriver->draw2DImage( texture, dstRect, srcRect, clip, colors, true );
}

void CImageGUISkin::draw2DRectangle(IGUIElement* element, const video::SColor &color, 
	const core::rect<s32>& pos, const core::rect<s32>* clip)
{
	FallbackSkin->draw2DRectangle(element, color, pos, clip);
}

} // namespace gui
} // namespace irr

