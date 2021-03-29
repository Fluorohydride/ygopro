// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
#ifdef __ANDROID__
#include "CGUICustomComboBox.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include <string>
#include <vector>
#include <IGUIEnvironment.h>
#include <IVideoDriver.h>
#include <IGUISkin.h>
#include <IGUIEnvironment.h>
#include <IGUIFont.h>
#include <IGUIButton.h>
#include "../IrrlichtCommonIncludes1.9/CGUIListBox.h"
#include <IGUIStaticText.h>
#include "../IrrlichtCommonIncludes1.9/os.h"
#include "../Android/porting_android.h"
#include "../bufferio.h"

namespace irr {
namespace gui {

IGUIComboBox* CGUICustomComboBox::addCustomComboBox(IGUIEnvironment* environment, const core::rect<s32>& rectangle,
													IGUIElement* parent, s32 id) {
	if(!parent)
		parent = environment->getRootGUIElement();
	auto b = new CGUICustomComboBox(environment, parent, id, rectangle);
	b->drop();
	return b;
}

//! constructor
CGUICustomComboBox::CGUICustomComboBox(IGUIEnvironment* environment, IGUIElement* parent,
									   s32 id, core::rect<s32> rectangle)
	: IGUIComboBox(environment, parent, id, rectangle),
	ListButton(0), SelectedText(0), ListBox(0), LastFocus(0),
	Selected(-1), HAlign(EGUIA_UPPERLEFT), VAlign(EGUIA_CENTER), MaxSelectionRows(5), HasFocus(false) {
#ifdef _DEBUG
	setDebugName("CGUICustomComboBox");
#endif

	IGUISkin* skin = Environment->getSkin();

	s32 width = 15;
	if(skin)
		width = skin->getSize(EGDS_WINDOW_BUTTON_WIDTH);

	core::rect<s32> r;
	r.UpperLeftCorner.X = rectangle.getWidth() - width - 2;
	r.LowerRightCorner.X = rectangle.getWidth() - 2;

	r.UpperLeftCorner.Y = 2;
	r.LowerRightCorner.Y = rectangle.getHeight() - 2;

	ListButton = Environment->addButton(r, this, -1, L"");
	if(skin && skin->getSpriteBank()) {
		ListButton->setSpriteBank(skin->getSpriteBank());
		ListButton->setSprite(EGBS_BUTTON_UP, skin->getIcon(EGDI_CURSOR_DOWN), skin->getColor(EGDC_WINDOW_SYMBOL));
		ListButton->setSprite(EGBS_BUTTON_DOWN, skin->getIcon(EGDI_CURSOR_DOWN), skin->getColor(EGDC_WINDOW_SYMBOL));
	}
	ListButton->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT);
	ListButton->setSubElement(true);
	ListButton->setTabStop(false);

	r.UpperLeftCorner.X = 2;
	r.UpperLeftCorner.Y = 2;
	r.LowerRightCorner.X = RelativeRect.getWidth() - (ListButton->getAbsolutePosition().getWidth() + 2);
	r.LowerRightCorner.Y = RelativeRect.getHeight() - 2;

	SelectedText = Environment->addStaticText(L"", r, false, false, this, -1, false);
	SelectedText->setSubElement(true);
	SelectedText->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT);
	SelectedText->setTextAlignment(EGUIA_UPPERLEFT, EGUIA_CENTER);
	if(skin)
		SelectedText->setOverrideColor(skin->getColor(EGDC_BUTTON_TEXT));
	SelectedText->enableOverrideColor(true);

	// this element can be tabbed to
	setTabStop(true);
	setTabOrder(-1);
}


void CGUICustomComboBox::setTextAlignment(EGUI_ALIGNMENT horizontal, EGUI_ALIGNMENT vertical) {
	HAlign = horizontal;
	VAlign = vertical;
	SelectedText->setTextAlignment(horizontal, vertical);
}


//! Set the maximal number of rows for the selection listbox
void CGUICustomComboBox::setMaxSelectionRows(u32 max) {
	MaxSelectionRows = max;
}

//! Get the maximimal number of rows for the selection listbox
u32 CGUICustomComboBox::getMaxSelectionRows() const {
	return MaxSelectionRows;
}


//! Returns amount of items in box
u32 CGUICustomComboBox::getItemCount() const {
	return Items.size();
}


//! returns string of an item. the idx may be a value from 0 to itemCount-1
const wchar_t* CGUICustomComboBox::getItem(u32 idx) const {
	if(idx >= Items.size())
		return 0;

	return Items[idx].Name.c_str();
}

//! returns string of an item. the idx may be a value from 0 to itemCount-1
u32 CGUICustomComboBox::getItemData(u32 idx) const {
	if(idx >= Items.size())
		return 0;

	return Items[idx].Data;
}

//! Returns index based on item data
s32 CGUICustomComboBox::getIndexForItemData(u32 data) const {
	for(u32 i = 0; i < Items.size(); ++i) {
		if(Items[i].Data == data)
			return i;
	}
	return -1;
}


//! Removes an item from the combo box.
void CGUICustomComboBox::removeItem(u32 idx) {
	if(idx >= Items.size())
		return;

	if(Selected == (s32)idx)
		setSelected(-1);

	Items.erase(idx);
}


//! Returns caption of this element.
const wchar_t* CGUICustomComboBox::getText() const {
	return getItem(Selected);
}


//! adds an item and returns the index of it
u32 CGUICustomComboBox::addItem(const wchar_t* text, u32 data) {
	Items.push_back(SComboData(text, data));

	if(Selected == -1)
		setSelected(0);

	return Items.size() - 1;
}


//! deletes all items in the combo box
void CGUICustomComboBox::clear() {
	Items.clear();
	setSelected(-1);
}


//! returns id of selected item. returns -1 if no item is selected.
s32 CGUICustomComboBox::getSelected() const {
	return Selected;
}


//! sets the selected item. Set this to -1 if no item should be selected
void CGUICustomComboBox::setSelected(s32 idx) {
	if(idx < -1 || idx >= (s32)Items.size())
		return;

	Selected = idx;
	if(Selected == -1)
		SelectedText->setText(L"");
	else
		SelectedText->setText(Items[Selected].Name.c_str());
}


//! called if an event happened.
bool CGUICustomComboBox::OnEvent(const SEvent& event) {
	if(isEnabled()) {
		switch(event.EventType) {
			case EET_GUI_EVENT:

				switch(event.GUIEvent.EventType) {
					case EGET_BUTTON_CLICKED:
						if(event.GUIEvent.Caller == ListButton) {
							Environment->setFocus(this);
							openCloseMenu();
							return true;
						}
						break;
					default:
						break;
				}
				break;
			case EET_MOUSE_INPUT_EVENT:

				switch(event.MouseInput.Event) {
					case EMIE_LMOUSE_LEFT_UP:
					{
						Environment->setFocus(this);
						openCloseMenu();
						return true;
					}
					default:
						break;
				}
				break;
			default:
				break;
		}
	}

	return IGUIElement::OnEvent(event);
}


void CGUICustomComboBox::sendSelectionChangedEvent() {
	if(Parent) {
		SEvent event;

		event.EventType = EET_GUI_EVENT;
		event.GUIEvent.Caller = this;
		event.GUIEvent.Element = 0;
		event.GUIEvent.EventType = EGET_COMBO_BOX_CHANGED;
		Parent->OnEvent(event);
	}
}


//! draws the element and its children
void CGUICustomComboBox::draw() {
	if(!IsVisible)
		return;

	IGUISkin* skin = Environment->getSkin();
	IGUIElement *currentFocus = Environment->getFocus();
	if(currentFocus != LastFocus) {
		HasFocus = currentFocus == this || isMyChild(currentFocus);
		LastFocus = currentFocus;
	}

	// set colors each time as skin-colors can be changed
	SelectedText->setBackgroundColor(skin->getColor(EGDC_HIGH_LIGHT));
	if(isEnabled()) {
		SelectedText->setDrawBackground(HasFocus);
		SelectedText->setOverrideColor(skin->getColor(HasFocus ? EGDC_HIGH_LIGHT_TEXT : EGDC_BUTTON_TEXT));
	} else {
		SelectedText->setDrawBackground(false);
		SelectedText->setOverrideColor(skin->getColor(EGDC_GRAY_TEXT));
	}
	ListButton->setSprite(EGBS_BUTTON_UP, skin->getIcon(EGDI_CURSOR_DOWN), skin->getColor(isEnabled() ? EGDC_WINDOW_SYMBOL : EGDC_GRAY_WINDOW_SYMBOL));
	ListButton->setSprite(EGBS_BUTTON_DOWN, skin->getIcon(EGDI_CURSOR_DOWN), skin->getColor(isEnabled() ? EGDC_WINDOW_SYMBOL : EGDC_GRAY_WINDOW_SYMBOL));


	core::rect<s32> frameRect(AbsoluteRect);

	// draw the border

	skin->draw3DSunkenPane(this, skin->getColor(EGDC_3D_HIGH_LIGHT),
						   true, true, frameRect, &AbsoluteClippingRect);

	// draw children
	IGUIElement::draw();
}


void CGUICustomComboBox::openCloseMenu() {
	std::vector<std::string> parameters;
	for(int i = 0; i < Items.size(); i++) {
		parameters.push_back(BufferIO::EncodeUTF8({ Items[i].Name.data(), Items[i].Name.size() }));
	}
	porting::showComboBox(parameters);
}


//! Writes attributes of the element.
void CGUICustomComboBox::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options = 0) const {
	IGUIComboBox::serializeAttributes(out, options);

	out->addEnum("HTextAlign", HAlign, GUIAlignmentNames);
	out->addEnum("VTextAlign", VAlign, GUIAlignmentNames);
	out->addInt("MaxSelectionRows", (s32)MaxSelectionRows);

	out->addInt("Selected", Selected);
	out->addInt("ItemCount", Items.size());
	for(u32 i = 0; i < Items.size(); ++i) {
		core::stringc s = "Item";
		s += i;
		s += "Text";
		out->addString(s.c_str(), Items[i].Name.c_str());
	}
}


//! Reads attributes of the element
void CGUICustomComboBox::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options = 0) {
	IGUIComboBox::deserializeAttributes(in, options);

	setTextAlignment((EGUI_ALIGNMENT)in->getAttributeAsEnumeration("HTextAlign", GUIAlignmentNames),
		(EGUI_ALIGNMENT)in->getAttributeAsEnumeration("VTextAlign", GUIAlignmentNames));
	setMaxSelectionRows((u32)(in->getAttributeAsInt("MaxSelectionRows")));

	// clear the list
	clear();
	// get item count
	u32 c = in->getAttributeAsInt("ItemCount");
	// add items
	for(u32 i = 0; i < c; ++i) {
		core::stringc s = "Item";
		s += i;
		s += "Text";
		addItem(in->getAttributeAsStringW(s.c_str()).c_str(), 0);
	}

	setSelected(in->getAttributeAsInt("Selected"));
}

} // end namespace gui
} // end namespace irr


#endif // _IRR_COMPILE_WITH_GUI_
#endif // __ANDROID__
