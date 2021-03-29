// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_GUI_TAB_CONTROL_H_INCLUDED__
#define __C_GUI_TAB_CONTROL_H_INCLUDED__

#include <IrrCompileConfig.h>
#ifdef _IRR_COMPILE_WITH_GUI_

#include <IGUITabControl.h>
#include <irrArray.h>
#include <IGUISkin.h>

namespace irr {
namespace gui {
class CGUITabControl;
class IGUIButton;

// A tab, onto which other gui elements could be added.
class CGUITab : public IGUITab {
public:

	//! constructor
	CGUITab(IGUIEnvironment* environment,
			IGUIElement* parent, const core::rect<s32>& rectangle,
			s32 id);

	//! draws the element and its children
	virtual void draw() _IRR_OVERRIDE_;

	//! sets if the tab should draw its background
	virtual void setDrawBackground(bool draw = true) _IRR_OVERRIDE_;

	//! sets the color of the background, if it should be drawn.
	virtual void setBackgroundColor(video::SColor c) _IRR_OVERRIDE_;

	//! sets the color of the text
	virtual void setTextColor(video::SColor c) _IRR_OVERRIDE_;

	//! returns true if the tab is drawing its background, false if not
	virtual bool isDrawingBackground() const _IRR_OVERRIDE_;

	//! returns the color of the background
	virtual video::SColor getBackgroundColor() const _IRR_OVERRIDE_;

	virtual video::SColor getTextColor() const _IRR_OVERRIDE_;

	//! Writes attributes of the element.
	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const _IRR_OVERRIDE_;

	//! Reads attributes of the element
	virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options) _IRR_OVERRIDE_;

private:

	video::SColor BackColor;
	bool OverrideTextColorEnabled;
	video::SColor TextColor;
	bool DrawBackground;
};


//! A standard tab control
class CGUITabControl : public IGUITabControl {
public:

	//! destructor
	CGUITabControl(IGUIEnvironment* environment,
				   IGUIElement* parent, const core::rect<s32>& rectangle,
				   bool fillbackground = true, bool border = true, s32 id = -1);

	//! destructor
	virtual ~CGUITabControl();

	//! Adds a tab
	virtual IGUITab* addTab(const wchar_t* caption, s32 id = -1) _IRR_OVERRIDE_;

	//! Adds an existing tab
	virtual s32 addTab(IGUITab* tab) _IRR_OVERRIDE_;

	//! Insert the tab at the given index
	virtual IGUITab* insertTab(s32 idx, const wchar_t* caption, s32 id = -1) _IRR_OVERRIDE_;

	//! Insert an existing tab
	/** Note that it will also add the tab as a child of this TabControl.
	\return Index of added tab (should be same as the one passed) or -1 for failure*/
	virtual s32 insertTab(s32 idx, IGUITab* tab, bool serializationMode) _IRR_OVERRIDE_;

	//! Removes a tab from the tabcontrol
	virtual void removeTab(s32 idx) _IRR_OVERRIDE_;

	//! Clears the tabcontrol removing all tabs
	virtual void clear() _IRR_OVERRIDE_;

	//! Returns amount of tabs in the tabcontrol
	virtual s32 getTabCount() const _IRR_OVERRIDE_;

	//! Returns a tab based on zero based index
	virtual IGUITab* getTab(s32 idx) const _IRR_OVERRIDE_;

	//! Brings a tab to front.
	virtual bool setActiveTab(s32 idx) _IRR_OVERRIDE_;

	//! Brings a tab to front.
	virtual bool setActiveTab(IGUITab* tab) _IRR_OVERRIDE_;

	//! For given given tab find it's zero-based index (or -1 for not found)
	virtual s32 getTabIndex(const IGUIElement* tab) const _IRR_OVERRIDE_;

	//! Returns which tab is currently active
	virtual s32 getActiveTab() const _IRR_OVERRIDE_;

	//! get the the id of the tab at the given absolute coordinates
	virtual s32 getTabAt(s32 xpos, s32 ypos) const _IRR_OVERRIDE_;

	//! called if an event happened.
	virtual bool OnEvent(const SEvent& event) _IRR_OVERRIDE_;

	//! draws the element and its children
	virtual void draw() _IRR_OVERRIDE_;

	//! Removes a child.
	virtual void removeChild(IGUIElement* child) _IRR_OVERRIDE_;

	//! Writes attributes of the element.
	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const _IRR_OVERRIDE_;
	//! Set the height of the tabs
	virtual void setTabHeight(s32 height) _IRR_OVERRIDE_;

	//! Reads attributes of the element
	virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options) _IRR_OVERRIDE_;

	//! Get the height of the tabs
	virtual s32 getTabHeight() const _IRR_OVERRIDE_;

	//! set the maximal width of a tab. Per default width is 0 which means "no width restriction".
	virtual void setTabMaxWidth(s32 width) _IRR_OVERRIDE_;

	//! get the maximal width of a tab
	virtual s32 getTabMaxWidth() const _IRR_OVERRIDE_;

	//! Set the alignment of the tabs
	//! note: EGUIA_CENTER is not an option
	virtual void setTabVerticalAlignment(gui::EGUI_ALIGNMENT alignment) _IRR_OVERRIDE_;

	//! Get the alignment of the tabs
	virtual gui::EGUI_ALIGNMENT getTabVerticalAlignment() const _IRR_OVERRIDE_;

	//! Set the extra width added to tabs on each side of the text
	virtual void setTabExtraWidth(s32 extraWidth) _IRR_OVERRIDE_;

	//! Get the extra width added to tabs on each side of the text
	virtual s32 getTabExtraWidth() const _IRR_OVERRIDE_;

	//! Update the position of the element, decides scroll button status
	virtual void updateAbsolutePosition() _IRR_OVERRIDE_;

private:

	void scrollLeft();
	void scrollRight();
	bool needScrollControl(s32 startIndex = 0, bool withScrollControl = false);
	s32 calcTabWidth(s32 pos, IGUIFont* font, const wchar_t* text, bool withScrollControl) const;
	core::rect<s32> calcTabPos();
	void setVisibleTab(s32 idx);
	void removeTabButNotChild(s32 idx);

	void recalculateScrollButtonPlacement();
	void recalculateScrollBar();
	void refreshSprites();

	core::array<IGUITab*> Tabs;
	s32 ActiveTabIndex;
	bool Border;
	bool FillBackground;
	bool ScrollControl;
	s32 TabHeight;
	gui::EGUI_ALIGNMENT VerticalAlignment;
	IGUIButton* UpButton;
	IGUIButton* DownButton;
	s32 TabMaxWidth;
	s32 CurrentScrollTabIndex;
	s32 TabExtraWidth;
};


} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

#endif

