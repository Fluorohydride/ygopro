#ifndef CIMAGEGUISKIN_H_
#define CIMAGEGUISKIN_H_

#include <IGUISkin.h>
#include <irrString.h>
#include <irrMap.h>
#include <IGUISpriteBank.h>

namespace irr
{
namespace video
{
    class IVideoDriver;
    class ITexture;
}
namespace gui
{

struct SImageGUIElementStyle
{
    struct SBorder
    {
        s32 Top, Left, Bottom, Right;
        SBorder() : Top(0), Left(0), Bottom(0), Right(0) {}
    };
    SBorder SrcBorder;
    SBorder DstBorder;
    video::ITexture* Texture;
    video::SColor Color;

    SImageGUIElementStyle() : Texture(0), Color(255,255,255,255) {}
};

struct SImageGUISkinConfig
{
    SImageGUIElementStyle SunkenPane, Window, Button, ButtonPressed, ButtonDisabled, ProgressBar, ProgressBarFilled, TabButton, TabButtonPressed, TabBody, MenuBar, MenuPane, MenuPressed, CheckBox, CheckBoxDisabled, ComboBox, ComboBoxDisabled;
	video::SColor CheckBoxColor;
};

class CImageGUISkin : public IGUISkin
{
public:
    CImageGUISkin( video::IVideoDriver* videoDriver, IGUISkin* fallbackSkin );
    virtual ~CImageGUISkin();

    void loadConfig( const SImageGUISkinConfig& config );

    //! returns default color
    virtual video::SColor getColor(EGUI_DEFAULT_COLOR color) const;

    //! sets a default color
    virtual void setColor(EGUI_DEFAULT_COLOR which, video::SColor newColor);

    //! returns default color
    virtual s32 getSize(EGUI_DEFAULT_SIZE size) const;

    //! Returns a default text. 
    virtual const wchar_t* getDefaultText(EGUI_DEFAULT_TEXT text) const;

    //! Sets a default text.
    virtual void setDefaultText(EGUI_DEFAULT_TEXT which, const wchar_t* newText);

    //! sets a default size
    virtual void setSize(EGUI_DEFAULT_SIZE which, s32 size);

    //! returns the default font
    virtual IGUIFont* getFont(EGUI_DEFAULT_FONT defaultFont) const;

    //! sets a default font
    virtual void setFont(IGUIFont* font, EGUI_DEFAULT_FONT defaultFont);

    //! returns the sprite bank
    virtual IGUISpriteBank* getSpriteBank() const;

    //! sets the sprite bank
    virtual void setSpriteBank(IGUISpriteBank* bank);

    virtual u32 getIcon(EGUI_DEFAULT_ICON icon) const;

    virtual void setIcon(EGUI_DEFAULT_ICON icon, u32 index);

    virtual void draw3DButtonPaneStandard(IGUIElement* element,
            const core::rect<s32>& rect,
            const core::rect<s32>* clip=0);

    virtual void draw3DButtonPanePressed(IGUIElement* element, 
            const core::rect<s32>& rect,
            const core::rect<s32>* clip=0);

    virtual void draw3DSunkenPane(IGUIElement* element,
            video::SColor bgcolor, bool flat, bool fillBackGround,
            const core::rect<s32>& rect,
            const core::rect<s32>* clip=0);
	/* Updates for irrlicht 1.7 by Mamnarock
    virtual core::rect<s32> draw3DWindowBackground(IGUIElement* element,
            bool drawTitleBar, video::SColor titleBarColor,
            const core::rect<s32>& rect,
            const core::rect<s32>* clip=0);
  */
	virtual core::rect<s32> draw3DWindowBackground(IGUIElement* element, 
         bool drawTitleBar, video::SColor titleBarColor, 
         const core::rect<s32>& rect, 
         const core::rect<s32>* clip=0, 
         core::rect<s32>* checkClientArea=0);

    virtual void draw3DMenuPane(IGUIElement* element,
            const core::rect<s32>& rect,
            const core::rect<s32>* clip=0);

    virtual void draw3DToolBar(IGUIElement* element,
            const core::rect<s32>& rect,
            const core::rect<s32>* clip=0);

    virtual void draw3DTabButton(IGUIElement* element, bool active,
            const core::rect<s32>& rect, const core::rect<s32>* clip=0, EGUI_ALIGNMENT alignment=EGUIA_UPPERLEFT);

    virtual void draw3DTabBody(IGUIElement* element, bool border, bool background,
            const core::rect<s32>& rect, const core::rect<s32>* clip=0,s32 tabHeight=-1, gui::EGUI_ALIGNMENT alignment=EGUIA_UPPERLEFT );

    virtual void drawIcon(IGUIElement* element, EGUI_DEFAULT_ICON icon,
            const core::position2di position, u32 starttime=0, u32 currenttime=0, 
            bool loop=false, const core::rect<s32>* clip=0);
	// Madoc - I had to add some things
	
	// Exposes config so we can get the progress bar colors
	virtual SImageGUISkinConfig getConfig() { return Config; }

	// End Madoc adds
    virtual void drawHorizontalProgressBar( IGUIElement* element, const core::rect<s32>& rectangle, const core::rect<s32>* clip,
            f32 filledRatio, video::SColor fillColor, video::SColor emptyColor );

	virtual void draw2DRectangle(IGUIElement* element, const video::SColor &color, 
		const core::rect<s32>& pos, const core::rect<s32>* clip = 0);
	virtual void setProperty(core::stringw key, core::stringw value);
	virtual core::stringw getProperty(core::stringw key);

private:
    void drawElementStyle( const SImageGUIElementStyle& elem, const core::rect<s32>& rect, const core::rect<s32>* clip, video::SColor* color=0 );

    video::IVideoDriver* VideoDriver;
    IGUISkin* FallbackSkin;
    SImageGUISkinConfig Config;
	
	core::map<core::stringw,core::stringw> properties;
};

}
}

#endif


