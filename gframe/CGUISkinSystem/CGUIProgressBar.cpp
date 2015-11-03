#include "CGUIProgressBar.h"
#include <IGUIEnvironment.h>
#include "CImageGUISkin.h"
#include <IGUIFont.h>

namespace irr
{
namespace gui
{

CGUIProgressBar::CGUIProgressBar( IGUIElement* parent, IGUIEnvironment* environment, const core::rect<s32>& rectangle, s32 id,bool bind )
    : IGUIElement( EGUIET_ELEMENT, environment, parent, id, rectangle )
{
    FilledRatio = 0.0f;
    FillColor = video::SColor(255,255,0,0);
	EmptyColor = video::SColor();
    AutomaticTextFormat = L"%2.0f %%";
	bindColorsToSkin = bind;
}

void CGUIProgressBar::setAutomaticText( const wchar_t* text )
{
    AutomaticTextFormat = text? text:L"";
    if ( AutomaticTextFormat != L"" )
    {
        wchar_t* buffer = new wchar_t[ AutomaticTextFormat.size() + 10 ];
        swprintf( buffer, AutomaticTextFormat.size() + 10, AutomaticTextFormat.c_str(), 100*FilledRatio );
        Text = buffer;
        delete buffer;
    }
}

void CGUIProgressBar::setFillColor( video::SColor fill )
{
    FillColor = fill;
}

video::SColor CGUIProgressBar::getFillColor() const
{
    return FillColor;
}

void CGUIProgressBar::setEmptyColor( video::SColor empty )
{
    EmptyColor = empty;
}

video::SColor CGUIProgressBar::getEmptyColor() const
{
    return EmptyColor;
}


void CGUIProgressBar::setProgress( f32 progress )
{
    FilledRatio = progress;
    if ( AutomaticTextFormat != L"" )
    {
        wchar_t* buffer = new wchar_t[ AutomaticTextFormat.size() + 10 ];
        swprintf( buffer, AutomaticTextFormat.size() + 10, AutomaticTextFormat.c_str(), 100*FilledRatio );
        Text = buffer;
        delete buffer;
    }
}

f32 CGUIProgressBar::getProgress() const
{
    return FilledRatio;
}

void CGUIProgressBar::draw()
{
    CImageGUISkin* skin = static_cast<CImageGUISkin*>( Environment->getSkin() );
	// Madoc added these changes to let the progress bar get its fill colors from the skin
	if(bindColorsToSkin) {
		EmptyColor = ((gui::SImageGUISkinConfig)skin->getConfig()).ProgressBar.Color;
		FillColor  = ((gui::SImageGUISkinConfig)skin->getConfig()).ProgressBarFilled.Color;
	}
	// End Madoc changes
    skin->drawHorizontalProgressBar( this, AbsoluteRect, &AbsoluteClippingRect, FilledRatio, FillColor, EmptyColor );

    // Draw text in center
	skin->getFont(EGDF_DEFAULT)->draw( Text.c_str(), AbsoluteRect, skin->getColor(EGDC_BUTTON_TEXT), true, true, &AbsoluteClippingRect );
}

}
}

