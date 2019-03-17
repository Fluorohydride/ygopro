#ifndef CGUIPROGRESSBAR_H_
#define CGUIPROGRESSBAR_H_

#include <IGUIElement.h>
#include <SColor.h>

namespace irr
{
namespace gui
{

/*!
 * A horizontal progess bar. ONLY works with CImageGUISkin applied - will fail
 * if another skin is used.
 */
class CGUIProgressBar : public IGUIElement
{
public:
	// If you want your progress bar colors to be independent of the skin set bind to false -- Madoc
    CGUIProgressBar( IGUIElement* parent, IGUIEnvironment* environment, const core::rect<s32>& rectangle, s32 id=-1 ,bool bind=true);
    
    //! Automatically updates the progress bar's text every time the progress changes.
    //! Set format to NULL or an empty string to disable automatic text.
    void setAutomaticText( const wchar_t* format );

    void setFillColor( video::SColor fill );
    video::SColor getFillColor() const;

    void setEmptyColor( video::SColor empty );
    video::SColor getEmptyColor() const;

    void setProgress( f32 progress );
    f32 getProgress() const;

    virtual void draw();

private:
    video::SColor FillColor;
	video::SColor EmptyColor;
	bool bindColorsToSkin;
    f32 FilledRatio;
    core::stringw AutomaticTextFormat;
};

} // namespace gui
} // namespace irr

#endif

