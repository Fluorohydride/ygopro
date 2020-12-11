// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CGUIFileSelectListBox.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include <algorithm> // std::find

#include <IGUISkin.h>
#include <IGUIEnvironment.h>
#include <IGUIFont.h>
#include <IGUISpriteBank.h>
#if IRRLICHT_VERSION_MAJOR==1 && IRRLICHT_VERSION_MINOR==9
#include "../IrrlichtCommonIncludes1.9/CGUIScrollBar.h"
#include "../IrrlichtCommonIncludes1.9/os.h"
#else
#include "../IrrlichtCommonIncludes/CGUIScrollBar.h"
#include "../IrrlichtCommonIncludes/os.h"
#endif
#include "../utils.h"

namespace irr {
namespace gui {

//! constructor
CGUIFileSelectListBox::CGUIFileSelectListBox(IGUIEnvironment* environment, IGUIElement* parent,
											 s32 id, core::rect<s32> rectangle, io::IFileSystem* filesystem, bool clip,
											 bool drawBack, bool moveOverSelect)
	: IGUIListBox(environment, parent, id, rectangle), Selected(-1),
	ItemHeight(0), ItemHeightOverride(0),
	TotalItemHeight(0), ItemsIconWidth(0), Font(0), IconBank(0),
	ScrollBar(0), selectTime(0), LastKeyTime(0), Selecting(false), DrawBack(drawBack),
	MoveOverSelect(moveOverSelect), AutoScroll(true), HighlightWhenNotFocused(true),
	filesystem(filesystem), basePath{}, curRelPath{}, TotalFolders(0), filter(0),
	NativeDirectoryHandling(false) {
#ifdef _DEBUG
	setDebugName("CGUIFileSelectListBox");
#endif

	IGUISkin* skin = Environment->getSkin();
	const s32 s = skin->getSize(EGDS_SCROLLBAR_SIZE);

	ScrollBar = new CGUIScrollBar(false, Environment, this, -1,
								  core::rect<s32>(RelativeRect.getWidth() - s, 0, RelativeRect.getWidth(), RelativeRect.getHeight()),
								  !clip);
	ScrollBar->setSubElement(true);
	ScrollBar->setTabStop(false);
	ScrollBar->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT);
	ScrollBar->setVisible(false);
	ScrollBar->setPos(0);

	setNotClipped(!clip);

	// this element can be tabbed to
	setTabStop(true);
	setTabOrder(-1);

	updateAbsolutePosition();
}

CGUIFileSelectListBox * CGUIFileSelectListBox::addFileSelectListBox(IGUIEnvironment * environment, IGUIElement * parent, s32 id, core::rect<s32> rectangle, io::IFileSystem* filesystem, bool clip, bool drawBack, bool moveOverSelect) {
	CGUIFileSelectListBox* box = new CGUIFileSelectListBox(environment, parent, id, rectangle, filesystem, clip, drawBack, moveOverSelect);
	box->drop();
	return box;
}


//! destructor
CGUIFileSelectListBox::~CGUIFileSelectListBox() {
	if(ScrollBar)
		ScrollBar->drop();

	if(Font)
		Font->drop();

	if(IconBank)
		IconBank->drop();
}


//! returns amount of list items
u32 CGUIFileSelectListBox::getItemCount() const {
	return Items.size();
}


//! returns string of a list item. the may be a value from 0 to itemCount-1
const wchar_t* CGUIFileSelectListBox::getListItem(u32 id) const {
	return getListItem(id, false);
}

const wchar_t* CGUIFileSelectListBox::getListItem(u32 id, bool relativepath) const {
	id += TotalFolders;
	if(id >= Items.size())
		return 0;

	if(relativepath)
		return Items[id].reltext.c_str();

	return Items[id].text.c_str();
}


//! Returns the icon of an item
s32 CGUIFileSelectListBox::getIcon(u32 id) const {
	id += TotalFolders;
	if(id >= Items.size())
		return -1;

	return Items[id].icon;
}


//! adds a list item, returns id of item
u32 CGUIFileSelectListBox::addItem(const wchar_t* text) {
	return 0;
}


//! adds a list item, returns id of item
void CGUIFileSelectListBox::removeItem(u32 id) {
	return;
}


s32 CGUIFileSelectListBox::getItemAt(s32 xpos, s32 ypos) const {
	if(xpos < AbsoluteRect.UpperLeftCorner.X || xpos >= AbsoluteRect.LowerRightCorner.X
	   || ypos < AbsoluteRect.UpperLeftCorner.Y || ypos >= AbsoluteRect.LowerRightCorner.Y
	   )
		return -1;

	if(ItemHeight == 0)
		return -1;

	s32 item = ((ypos - AbsoluteRect.UpperLeftCorner.Y - 1) + ScrollBar->getPos()) / ItemHeight;
	if(item < 0 || item >= (s32)Items.size())
		return -1;

	return item;
}

//! clears the list
void CGUIFileSelectListBox::clear() {
	Items.clear();
	ItemsIconWidth = 0;
	Selected = -1;

	if(ScrollBar)
		ScrollBar->setPos(0);

	recalculateItemHeight();
}


void CGUIFileSelectListBox::recalculateItemHeight() {
	IGUISkin* skin = Environment->getSkin();

	if(Font != skin->getFont()) {
		if(Font)
			Font->drop();

		Font = skin->getFont();
		if(0 == ItemHeightOverride)
			ItemHeight = 0;

		if(Font) {
			if(0 == ItemHeightOverride)
				ItemHeight = Font->getDimension(L"A").Height + 4;

			Font->grab();
		}
	}

	TotalItemHeight = ItemHeight * Items.size();
	ScrollBar->setMax(core::max_(0, TotalItemHeight - AbsoluteRect.getHeight()));
	s32 minItemHeight = ItemHeight > 0 ? ItemHeight : 1;
	ScrollBar->setSmallStep(minItemHeight);
	ScrollBar->setLargeStep(2 * minItemHeight);

	if(TotalItemHeight <= AbsoluteRect.getHeight())
		ScrollBar->setVisible(false);
	else
		ScrollBar->setVisible(true);
}


//! returns id of selected item. returns -1 if no item is selected.
s32 CGUIFileSelectListBox::getSelected() const {
	return std::max(Selected - TotalFolders, -1);
}


//! sets the selected item. Set this to -1 if no item should be selected
void CGUIFileSelectListBox::setSelected(s32 id) {
	if((u32)id >= Items.size())
		Selected = -1;
	else
		Selected = id;

	selectTime = os::Timer::getTime();

	recalculateScrollPos();
}

//! sets the selected item. Set this to -1 if no item should be selected
void CGUIFileSelectListBox::setSelected(const wchar_t *item) {
	s32 index = -1;

	if(item) {
		for(index = 0; index < (s32)Items.size(); ++index) {
			if(Items[index].text == item)
				break;
		}
	}
	setSelected(index);
}

//! called if an event happened.
bool CGUIFileSelectListBox::OnEvent(const SEvent& event) {
	if(isEnabled()) {
		switch(event.EventType) {
			case EET_KEY_INPUT_EVENT:
				if(event.KeyInput.PressedDown &&
					(event.KeyInput.Key == KEY_DOWN ||
					 event.KeyInput.Key == KEY_UP ||
					 event.KeyInput.Key == KEY_HOME ||
					 event.KeyInput.Key == KEY_END ||
					 event.KeyInput.Key == KEY_NEXT ||
					 event.KeyInput.Key == KEY_PRIOR)) {
					s32 oldSelected = Selected;
					switch(event.KeyInput.Key) {
						case KEY_DOWN:
							Selected += 1;
							break;
						case KEY_UP:
							Selected -= 1;
							break;
						case KEY_HOME:
							Selected = 0;
							break;
						case KEY_END:
							Selected = (s32)Items.size() - 1;
							break;
						case KEY_NEXT:
							Selected += AbsoluteRect.getHeight() / ItemHeight;
							break;
						case KEY_PRIOR:
							Selected -= AbsoluteRect.getHeight() / ItemHeight;
							break;
						default:
							break;
					}
					if(Selected >= (s32)Items.size())
						Selected = Items.size() - 1;
					else
						if(Selected < 0)
							Selected = 0;

					recalculateScrollPos();

					// post the news

					if(oldSelected != Selected && Parent && !Selecting && !MoveOverSelect) {
						SEvent e;
						e.EventType = EET_GUI_EVENT;
						e.GUIEvent.Caller = this;
						e.GUIEvent.Element = 0;
						e.GUIEvent.EventType = EGET_LISTBOX_CHANGED;
						Parent->OnEvent(e);
					}

					return true;
				} else
					if(!event.KeyInput.PressedDown && (event.KeyInput.Key == KEY_RETURN || event.KeyInput.Key == KEY_SPACE)) {
						if(Items[Selected].isDirectory) {
							prevRelPath = curRelPath;
							curRelPath = Items[Selected].reltext;
							Selected = 0;
							if(ScrollBar)
								ScrollBar->setPos(0);
							LoadFolderContents();
							return true;
						}
						if(Parent) {
							SEvent e;
							e.EventType = EET_GUI_EVENT;
							e.GUIEvent.Caller = this;
							e.GUIEvent.Element = 0;
							e.GUIEvent.EventType = EGET_LISTBOX_SELECTED_AGAIN;
							Parent->OnEvent(e);
						}
						return true;
					} else if(event.KeyInput.PressedDown && event.KeyInput.Char) {
						// change selection based on text as it is typed.
						u32 now = os::Timer::getTime();

						if(now - LastKeyTime < 500) {
							// add to key buffer if it isn't a key repeat
							if(!(KeyBuffer.size() == 1 && KeyBuffer[0] == event.KeyInput.Char)) {
								KeyBuffer += L" ";
								KeyBuffer[KeyBuffer.size() - 1] = event.KeyInput.Char;
							}
						} else {
							KeyBuffer = L" ";
							KeyBuffer[0] = event.KeyInput.Char;
						}
						LastKeyTime = now;

						// find the selected item, starting at the current selection
						s32 start = Selected;
						// dont change selection if the key buffer matches the current item
						if(Selected > -1 && KeyBuffer.size() > 1) {
							if(Items[Selected].text.size() >= KeyBuffer.size() &&
							   ygo::Utils::EqualIgnoreCase(KeyBuffer, Items[Selected].text.substr(0, KeyBuffer.size())))
								return true;
						}

						s32 current;
						for(current = start + 1; current < (s32)Items.size(); ++current) {
							if(Items[current].text.size() >= KeyBuffer.size()) {
								if(ygo::Utils::EqualIgnoreCase(KeyBuffer, Items[current].text.substr(0, KeyBuffer.size()))) {
									if(Parent && Selected != current && !Selecting && !MoveOverSelect) {
										SEvent e;
										e.EventType = EET_GUI_EVENT;
										e.GUIEvent.Caller = this;
										e.GUIEvent.Element = 0;
										e.GUIEvent.EventType = EGET_LISTBOX_CHANGED;
										Parent->OnEvent(e);
									}
									setSelected(current);
									return true;
								}
							}
						}
						for(current = 0; current <= start; ++current) {
							if(Items[current].text.size() >= KeyBuffer.size()) {
								if(ygo::Utils::EqualIgnoreCase(KeyBuffer, Items[current].text.substr(0, KeyBuffer.size()))) {
									if(Parent && Selected != current && !Selecting && !MoveOverSelect) {
										Selected = current;
										SEvent e;
										e.EventType = EET_GUI_EVENT;
										e.GUIEvent.Caller = this;
										e.GUIEvent.Element = 0;
										e.GUIEvent.EventType = EGET_LISTBOX_CHANGED;
										Parent->OnEvent(e);
									}
									setSelected(current);
									return true;
								}
							}
						}

						return true;
					}
					break;

			case EET_GUI_EVENT:
				switch(event.GUIEvent.EventType) {
					case gui::EGET_SCROLL_BAR_CHANGED:
						if(event.GUIEvent.Caller == ScrollBar)
							return true;
						break;
					case gui::EGET_ELEMENT_FOCUS_LOST:
					{
						if(event.GUIEvent.Caller == this)
							Selecting = false;
					}
					default:
						break;
				}
				break;

			case EET_MOUSE_INPUT_EVENT:
			{
				core::position2d<s32> p(event.MouseInput.X, event.MouseInput.Y);

				switch(event.MouseInput.Event) {
					case EMIE_MOUSE_WHEEL:
						ScrollBar->setPos(ScrollBar->getPos() + (event.MouseInput.Wheel < 0 ? -1 : 1)*-ItemHeight / 2);
						if(Selecting || MoveOverSelect)
							selectNew(event.MouseInput.Y, true);
						return true;

					case EMIE_LMOUSE_PRESSED_DOWN:
					{
						Selecting = true;
						selectNew(event.MouseInput.Y, true);
						return true;
					}

					case EMIE_LMOUSE_LEFT_UP:
					{
						Selecting = false;

						if(isPointInside(p))
							selectNew(event.MouseInput.Y);

						return true;
					}
#ifndef __ANDROID__
					case EMIE_MOUSE_MOVED:
						if(Selecting || MoveOverSelect) {
							if(isPointInside(p)) {
								selectNew(event.MouseInput.Y, true);
								return true;
							}
						}
#endif
					default:
						break;
				}
			}
			default:
				break;
		}
	}

	return IGUIElement::OnEvent(event);
}

bool CGUIFileSelectListBox::isDirectory(u32 index) {
	return index >= 0 && index < Items.size() && Items[index].isDirectory;
}

void CGUIFileSelectListBox::enterDirectory(u32 index) {
	if(!isDirectory(index))
		return;
	u32 now = os::Timer::getTime();
	selectTime = now;
	curRelPath = Items[index].reltext;
	Selected = -1;
	if(ScrollBar)
		ScrollBar->setPos(0);
	LoadFolderContents();
}

void CGUIFileSelectListBox::selectNew(s32 ypos, bool onlyHover) {
	u32 now = os::Timer::getTime();
	s32 oldSelected = Selected;

	Selected = getItemAt(AbsoluteRect.UpperLeftCorner.X, ypos);
	if(Selected < 0 && !Items.empty()) {
		if(onlyHover)
			Selected = oldSelected;
		else
			Selected = 0;
	}

	recalculateScrollPos();
	bool selagain = false;

	if(!onlyHover) {
		selagain = Selected >= 0 && (Selected == oldSelected && prevRelPath == curRelPath && now < selectTime + 500);
		prevRelPath = curRelPath;
		selectTime = now;
		if(selagain && Items[Selected].isDirectory && NativeDirectoryHandling) {
			enterDirectory(Selected);
			return;
		}
	}

	if(Selected != oldSelected)
		selectTime = 0;

	gui::EGUI_EVENT_TYPE eventType = selagain ? EGET_LISTBOX_SELECTED_AGAIN : EGET_LISTBOX_CHANGED;
	// post the news
	if(Parent) {
		SEvent event;
		event.EventType = EET_GUI_EVENT;
		event.GUIEvent.Caller = this;
		event.GUIEvent.Element = 0;
		event.GUIEvent.EventType = eventType;
		Parent->OnEvent(event);
	}
}


//! Update the position and size of the listbox, and update the scrollbar
void CGUIFileSelectListBox::updateAbsolutePosition() {
	IGUIElement::updateAbsolutePosition();

	recalculateItemHeight();
}


//! draws the element and its children
void CGUIFileSelectListBox::draw() {
	if(!IsVisible)
		return;

	recalculateItemHeight(); // if the font changed

	IGUISkin* skin = Environment->getSkin();

	core::rect<s32>* clipRect = 0;

	// draw background
	core::rect<s32> frameRect(AbsoluteRect);

	// draw items

	core::rect<s32> clientClip(AbsoluteRect);
	clientClip.UpperLeftCorner.Y += 1;
	clientClip.UpperLeftCorner.X += 1;
	if(ScrollBar->isVisible())
		clientClip.LowerRightCorner.X = AbsoluteRect.LowerRightCorner.X - skin->getSize(EGDS_SCROLLBAR_SIZE);
	clientClip.LowerRightCorner.Y -= 1;
	clientClip.clipAgainst(AbsoluteClippingRect);

	skin->draw3DSunkenPane(this, skin->getColor(EGDC_3D_HIGH_LIGHT), true,
						   DrawBack, frameRect, &clientClip);

	if(clipRect)
		clientClip.clipAgainst(*clipRect);

	frameRect = AbsoluteRect;
	frameRect.UpperLeftCorner.X += 1;
	if(ScrollBar->isVisible())
		frameRect.LowerRightCorner.X = AbsoluteRect.LowerRightCorner.X - skin->getSize(EGDS_SCROLLBAR_SIZE);

	frameRect.LowerRightCorner.Y = AbsoluteRect.UpperLeftCorner.Y + ItemHeight;

	frameRect.UpperLeftCorner.Y -= ScrollBar->getPos();
	frameRect.LowerRightCorner.Y -= ScrollBar->getPos();

	bool hl = (HighlightWhenNotFocused || Environment->hasFocus(this) || Environment->hasFocus(ScrollBar));

	for(s32 i = 0; i < (s32)Items.size(); ++i) {
		if(frameRect.LowerRightCorner.Y >= AbsoluteRect.UpperLeftCorner.Y &&
		   frameRect.UpperLeftCorner.Y <= AbsoluteRect.LowerRightCorner.Y) {
			if(i == Selected && hl)
				skin->draw2DRectangle(this, skin->getColor(EGDC_HIGH_LIGHT), frameRect, &clientClip);

			core::rect<s32> textRect = frameRect;
			textRect.UpperLeftCorner.X += 3;

			if(Font) {
				if(IconBank && (Items[i].icon > -1)) {
					core::position2di iconPos = textRect.UpperLeftCorner;
					iconPos.Y += textRect.getHeight() / 2;
					iconPos.X += ItemsIconWidth / 2;

					if(i == Selected && hl) {
						IconBank->draw2DSprite((u32)Items[i].icon, iconPos, &clientClip,
											   hasItemOverrideColor(i, EGUI_LBC_ICON_HIGHLIGHT) ?
											   getItemOverrideColor(i, EGUI_LBC_ICON_HIGHLIGHT) : getItemDefaultColor(EGUI_LBC_ICON_HIGHLIGHT),
											   selectTime, os::Timer::getTime(), false, true);
					} else {
						IconBank->draw2DSprite((u32)Items[i].icon, iconPos, &clientClip,
											   hasItemOverrideColor(i, EGUI_LBC_ICON) ? getItemOverrideColor(i, EGUI_LBC_ICON) : getItemDefaultColor(EGUI_LBC_ICON),
											   0, (i == Selected) ? os::Timer::getTime() : 0, false, true);
					}
				}

				textRect.UpperLeftCorner.X += ItemsIconWidth + 3;

				if(i == Selected && hl) {
					Font->draw(Items[i].text.c_str(), textRect,
							   hasItemOverrideColor(i, EGUI_LBC_TEXT_HIGHLIGHT) ?
							   getItemOverrideColor(i, EGUI_LBC_TEXT_HIGHLIGHT) : getItemDefaultColor(EGUI_LBC_TEXT_HIGHLIGHT),
							   false, true, &clientClip);
				} else {
					Font->draw(Items[i].text.c_str(), textRect,
							   hasItemOverrideColor(i, EGUI_LBC_TEXT) ? getItemOverrideColor(i, EGUI_LBC_TEXT) : getItemDefaultColor(EGUI_LBC_TEXT),
							   false, true, &clientClip);
				}

				textRect.UpperLeftCorner.X -= ItemsIconWidth + 3;
			}
		}

		frameRect.UpperLeftCorner.Y += ItemHeight;
		frameRect.LowerRightCorner.Y += ItemHeight;
	}

	IGUIElement::draw();
}


//! adds an list item with an icon
u32 CGUIFileSelectListBox::addItem(const wchar_t* text, s32 icon) {
	return 0;
}


void CGUIFileSelectListBox::setSpriteBank(IGUISpriteBank* bank) {
	if(bank == IconBank)
		return;
	if(IconBank)
		IconBank->drop();

	IconBank = bank;
	if(IconBank)
		IconBank->grab();
}


void CGUIFileSelectListBox::recalculateScrollPos() {
	if(!AutoScroll)
		return;

	const s32 selPos = (Selected == -1 ? TotalItemHeight : Selected * ItemHeight) - ScrollBar->getPos();

	if(selPos < 0) {
		ScrollBar->setPos(ScrollBar->getPos() + selPos);
	} else
		if(selPos > AbsoluteRect.getHeight() - ItemHeight) {
			ScrollBar->setPos(ScrollBar->getPos() + selPos - AbsoluteRect.getHeight() + ItemHeight);
		}
}


void CGUIFileSelectListBox::setAutoScrollEnabled(bool scroll) {
	AutoScroll = scroll;
}


bool CGUIFileSelectListBox::isAutoScrollEnabled() const {
	return AutoScroll;
}


bool CGUIFileSelectListBox::getSerializationLabels(EGUI_LISTBOX_COLOR colorType, core::stringc & useColorLabel, core::stringc & colorLabel) const {
	switch(colorType) {
		case EGUI_LBC_TEXT:
			useColorLabel = "UseColText";
			colorLabel = "ColText";
			break;
		case EGUI_LBC_TEXT_HIGHLIGHT:
			useColorLabel = "UseColTextHl";
			colorLabel = "ColTextHl";
			break;
		case EGUI_LBC_ICON:
			useColorLabel = "UseColIcon";
			colorLabel = "ColIcon";
			break;
		case EGUI_LBC_ICON_HIGHLIGHT:
			useColorLabel = "UseColIconHl";
			colorLabel = "ColIconHl";
			break;
		default:
			return false;
	}
	return true;
}
void CGUIFileSelectListBox::LoadFolderContents() {
	Items.clear();
	Selected = -1;
	TotalFolders = 0;
	curRelPath = ygo::Utils::NormalizePath(curRelPath);
	bool is_root = BaseIsRoot && curRelPath == basePath;
	ygo::Utils::FindFiles(ygo::Utils::ToPathString(curRelPath), [&](epro::path_stringview _name, bool is_directory) {
		auto name = ygo::Utils::ToUnicodeIfNeeded(_name);
		if(name == L".")
			return;
		if(name == L"..") {
			if(is_root) {
				return;
			}
		}
		if((filter && !filter(curRelPath + name, is_directory)) || (!filter && !defaultFilter(curRelPath + name, is_directory))) {
			return;
		}
		ListItem item;
		item.reltext = ygo::Utils::NormalizePath(curRelPath + L"/" + name, false);
		if(is_directory) {
			if(NativeDirectoryHandling)
				TotalFolders++;
			item.reltext += L"/";
			name = L"[" + name + L"]";
		}
		item.text = name;
		item.icon = -1;
		item.isDirectory = is_directory;
		Items.push_back(item);
	});
	std::sort(Items.begin(), Items.end());
	recalculateItemHeight();
}

//! Writes attributes of the element.
void CGUIFileSelectListBox::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options = 0) const {
	IGUIListBox::serializeAttributes(out, options);

	// todo: out->addString	("IconBank",		IconBank->getName?);
	out->addBool("DrawBack", DrawBack);
	out->addBool("MoveOverSelect", MoveOverSelect);
	out->addBool("AutoScroll", AutoScroll);

	out->addInt("ItemCount", Items.size());
	for(u32 i = 0; i < Items.size(); ++i) {
		core::stringc label("text");
		label += i;
		out->addString(label.c_str(), Items[i].text.c_str());

		for(s32 c = 0; c < (s32)EGUI_LBC_COUNT; ++c) {
			core::stringc useColorLabel, colorLabel;
			if(!getSerializationLabels((EGUI_LISTBOX_COLOR)c, useColorLabel, colorLabel))
				return;
			label = useColorLabel; label += i;
			if(Items[i].OverrideColors[c].Use) {
				out->addBool(label.c_str(), true);
				label = colorLabel; label += i;
				out->addColor(label.c_str(), Items[i].OverrideColors[c].Color);
			} else {
				out->addBool(label.c_str(), false);
			}
		}
	}
}


//! Reads attributes of the element
void CGUIFileSelectListBox::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options = 0) {
	clear();

	DrawBack = in->getAttributeAsBool("DrawBack");
	MoveOverSelect = in->getAttributeAsBool("MoveOverSelect");
	AutoScroll = in->getAttributeAsBool("AutoScroll");

	IGUIListBox::deserializeAttributes(in, options);

	const s32 count = in->getAttributeAsInt("ItemCount");
	for(s32 i = 0; i < count; ++i) {
		core::stringc label("text");
		ListItem item;

		label += i;
		item.text = in->getAttributeAsStringW(label.c_str()).c_str();

		addItem(item.text.c_str(), item.icon);

		for(u32 c = 0; c < EGUI_LBC_COUNT; ++c) {
			core::stringc useColorLabel, colorLabel;
			if(!getSerializationLabels((EGUI_LISTBOX_COLOR)c, useColorLabel, colorLabel))
				return;
			label = useColorLabel; label += i;
			Items[i].OverrideColors[c].Use = in->getAttributeAsBool(label.c_str());
			if(Items[i].OverrideColors[c].Use) {
				label = colorLabel; label += i;
				Items[i].OverrideColors[c].Color = in->getAttributeAsColor(label.c_str());
			}
		}
	}
}


void CGUIFileSelectListBox::recalculateItemWidth(s32 icon) {
	if(IconBank && icon > -1 &&
	   IconBank->getSprites().size() > (u32)icon &&
	   IconBank->getSprites()[(u32)icon].Frames.size()) {
		u32 rno = IconBank->getSprites()[(u32)icon].Frames[0].rectNumber;
		if(IconBank->getPositions().size() > rno) {
			const s32 w = IconBank->getPositions()[rno].getWidth();
			if(w > ItemsIconWidth)
				ItemsIconWidth = w;
		}
	}
}


void CGUIFileSelectListBox::setItem(u32 index, const wchar_t* text, s32 icon) {
	return;
}


//! Insert the item at the given index
//! Return the index on success or -1 on failure.
s32 CGUIFileSelectListBox::insertItem(u32 index, const wchar_t* text, s32 icon) {
	return 0;
}


void CGUIFileSelectListBox::swapItems(u32 index1, u32 index2) {
	return;
}


void CGUIFileSelectListBox::setItemOverrideColor(u32 index, video::SColor color) {
	for(u32 c = 0; c < EGUI_LBC_COUNT; ++c) {
		Items[index].OverrideColors[c].Use = true;
		Items[index].OverrideColors[c].Color = color;
	}
}


void CGUIFileSelectListBox::setItemOverrideColor(u32 index, EGUI_LISTBOX_COLOR colorType, video::SColor color) {
	if(index >= Items.size() || colorType < 0 || colorType >= EGUI_LBC_COUNT)
		return;

	Items[index].OverrideColors[colorType].Use = true;
	Items[index].OverrideColors[colorType].Color = color;
}


void CGUIFileSelectListBox::clearItemOverrideColor(u32 index) {
	for(u32 c = 0; c < (u32)EGUI_LBC_COUNT; ++c) {
		Items[index].OverrideColors[c].Use = false;
	}
}


void CGUIFileSelectListBox::clearItemOverrideColor(u32 index, EGUI_LISTBOX_COLOR colorType) {
	if(index >= Items.size() || colorType < 0 || colorType >= EGUI_LBC_COUNT)
		return;

	Items[index].OverrideColors[colorType].Use = false;
}


bool CGUIFileSelectListBox::hasItemOverrideColor(u32 index, EGUI_LISTBOX_COLOR colorType) const {
	if(index >= Items.size() || colorType < 0 || colorType >= EGUI_LBC_COUNT)
		return false;

	return Items[index].OverrideColors[colorType].Use;
}


video::SColor CGUIFileSelectListBox::getItemOverrideColor(u32 index, EGUI_LISTBOX_COLOR colorType) const {
	if((u32)index >= Items.size() || colorType < 0 || colorType >= EGUI_LBC_COUNT)
		return video::SColor();

	return Items[index].OverrideColors[colorType].Color;
}


video::SColor CGUIFileSelectListBox::getItemDefaultColor(EGUI_LISTBOX_COLOR colorType) const {
	IGUISkin* skin = Environment->getSkin();
	if(!skin)
		return video::SColor();

	switch(colorType) {
		case EGUI_LBC_TEXT:
			return skin->getColor(EGDC_BUTTON_TEXT);
		case EGUI_LBC_TEXT_HIGHLIGHT:
			return skin->getColor(EGDC_HIGH_LIGHT_TEXT);
		case EGUI_LBC_ICON:
			return skin->getColor(EGDC_ICON);
		case EGUI_LBC_ICON_HIGHLIGHT:
			return skin->getColor(EGDC_ICON_HIGH_LIGHT);
		default:
			return video::SColor();
	}
}

//! set global itemHeight
void CGUIFileSelectListBox::setItemHeight(s32 height) {
	ItemHeight = height;
	ItemHeightOverride = 1;
}


//! Sets whether to draw the background
void CGUIFileSelectListBox::setDrawBackground(bool draw) {
	DrawBack = draw;
}

#if IRRLICHT_VERSION_MAJOR==1 && IRRLICHT_VERSION_MINOR==9
IGUIScrollBar* CGUIFileSelectListBox::getVerticalScrollBar() const {
	return ScrollBar;
}
#endif


void CGUIFileSelectListBox::refreshList() {
	LoadFolderContents();
}

void CGUIFileSelectListBox::resetPath() {
	curRelPath = basePath;
	prevRelPath.clear();
	LoadFolderContents();
}

void CGUIFileSelectListBox::setWorkingPath(const std::wstring& newDirectory, bool setAsRoot) {
	BaseIsRoot = setAsRoot;
	basePath = ygo::Utils::NormalizePath(newDirectory);
	curRelPath = basePath;
	prevRelPath = curRelPath;
	LoadFolderContents();
}

void CGUIFileSelectListBox::addFilterFunction(callback* function) {
	filter = function;
}

void CGUIFileSelectListBox::addFilteredExtensions(std::vector<std::wstring> extensions) {
	filtered_extensions = extensions;
	filter = nullptr;
}

bool CGUIFileSelectListBox::defaultFilter(std::wstring name, bool is_directory) {
	if(is_directory) {
		auto elements = ygo::Utils::TokenizeString<std::wstring>(name, L"/");
		return !(elements.size() && elements.back().size() && elements.back().front() == L'.' && elements.back() != L"..");
	}
	if(std::find(filtered_extensions.begin(), filtered_extensions.end(), ygo::Utils::GetFileExtension(name)) == filtered_extensions.end())
		return false;
	return true;
}

void CGUIFileSelectListBox::nativeDirectory(bool native_directory) {
	NativeDirectoryHandling = native_directory;
}

} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

