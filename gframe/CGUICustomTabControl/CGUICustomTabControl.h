// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_GUI_CUSTOM_TAB_CONTROL_H_INCLUDED__
#define __C_GUI_CUSTOM_TAB_CONTROL_H_INCLUDED__

#include <IrrCompileConfig.h>
#ifdef _IRR_COMPILE_WITH_GUI_

#include <IGUITabControl.h>
#include "../IrrlichtCommonIncludes/CGUITabControl.h"
#include <irrArray.h>

namespace irr {
namespace gui {
//! A standard tab control
class CGUICustomTabControl : public IGUITabControl {
public:

	//! destructor
	CGUICustomTabControl(IGUIEnvironment* environment,
						 IGUIElement* parent, const core::rect<s32>& rectangle,
						 bool fillbackground = true, bool border = true, s32 id = -1);

	static IGUITabControl* addCustomTabControl(IGUIEnvironment* environment, const core::rect<s32>& rectangle,
											   IGUIElement* parent, bool fillbackground = false, bool border = true, s32 id = -1);

	//! destructor
	virtual ~CGUICustomTabControl();

	//! Adds a tab
	virtual IGUITab* addTab(const wchar_t* caption, s32 id = -1);

	//! Adds a tab that has already been created
	virtual void addTab(CGUITab* tab);

	//! Insert the tab at the given index
	virtual IGUITab* insertTab(s32 idx, const wchar_t* caption, s32 id = -1);

	//! Removes a tab from the tabcontrol
	virtual void removeTab(s32 idx);

	//! Clears the tabcontrol removing all tabs
	virtual void clear();

	//! Returns amount of tabs in the tabcontrol
	virtual s32 getTabCount() const;

	//! Returns a tab based on zero based index
	virtual IGUITab* getTab(s32 idx) const;

	//! Brings a tab to front.
	virtual bool setActiveTab(s32 idx);

	//! Brings a tab to front.
	virtual bool setActiveTab(IGUITab *tab);

	//! Returns which tab is currently active
	virtual s32 getActiveTab() const;

	//! get the the id of the tab at the given absolute coordinates
	virtual s32 getTabAt(s32 xpos, s32 ypos) const;

	//! called if an event happened.
	virtual bool OnEvent(const SEvent& event);

	//! draws the element and its children
	virtual void draw();

	//! Removes a child.
	virtual void removeChild(IGUIElement* child);

	//! Writes attributes of the element.
	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const;
	//! Set the height of the tabs
	virtual void setTabHeight(s32 height);

	//! Reads attributes of the element
	virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options);

	//! Get the height of the tabs
	virtual s32 getTabHeight() const;

	//! set the maximal width of a tab. Per default width is 0 which means "no width restriction".
	virtual void setTabMaxWidth(s32 width);

	//! get the maximal width of a tab
	virtual s32 getTabMaxWidth() const;

	//! Set the alignment of the tabs
	//! note: EGUIA_CENTER is not an option
	virtual void setTabVerticalAlignment(gui::EGUI_ALIGNMENT alignment);

	//! Get the alignment of the tabs
	virtual gui::EGUI_ALIGNMENT getTabVerticalAlignment() const;

	//! Set the extra width added to tabs on each side of the text
	virtual void setTabExtraWidth(s32 extraWidth);

	//! Get the extra width added to tabs on each side of the text
	virtual s32 getTabExtraWidth() const;

	//! Update the position of the element, decides scroll button status
	virtual void updateAbsolutePosition();

private:

	void scrollLeft();
	void scrollRight();
	bool needScrollControl(s32 startIndex = 0, bool withScrollControl = false);
	s32 calcTabWidth(s32 pos, IGUIFont* font, const wchar_t* text, bool withScrollControl) const;
	core::rect<s32> calcTabPos();

	void recalculateScrollButtonPlacement();
	void recalculateScrollBar();
	void refreshSprites();

	core::array<CGUITab*> Tabs;	// CGUITab* because we need setNumber (which is certainly not nice)
	s32 ActiveTab;
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

