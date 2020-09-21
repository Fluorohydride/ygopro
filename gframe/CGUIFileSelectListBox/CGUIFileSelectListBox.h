// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_GUI_FILE_SELECT_LIST_BOX_H_INCLUDED__
#define __C_GUI_FILE_SELECT_LIST_BOX_H_INCLUDED__

#include <IrrCompileConfig.h>
#ifdef _IRR_COMPILE_WITH_GUI_

#include <vector>
#include <string>
#include <functional>
#include <IGUIListBox.h>
#ifndef _IRR_OVERRIDE_
#define _IRR_OVERRIDE_
#endif
#include "../utils.h"

namespace irr {
namespace io {
class IFileSystem;
}
namespace gui {

class IGUIFont;
class IGUIScrollBar;

class CGUIFileSelectListBox : public IGUIListBox {
public:
	//! constructor
	CGUIFileSelectListBox(IGUIEnvironment* environment, IGUIElement* parent,
						  s32 id, core::rect<s32> rectangle, io::IFileSystem* filesystem, bool clip = true,
						  bool drawBack = false, bool moveOverSelect = false);

	static CGUIFileSelectListBox* addFileSelectListBox(IGUIEnvironment* environment, IGUIElement* parent,
													   s32 id, core::rect<s32> rectangle, io::IFileSystem* filesystem, bool clip = true,
													   bool drawBack = false, bool moveOverSelect = false);

	//! destructor
	virtual ~CGUIFileSelectListBox();

	//! returns amount of list items
	virtual u32 getItemCount() const _IRR_OVERRIDE_;

	//! returns string of a list item. the id may be a value from 0 to itemCount-1

	virtual const wchar_t* getListItem(u32 id) const _IRR_OVERRIDE_;

	const wchar_t* getListItem(u32 id, bool relativepath) const;

	//! adds an list item, returns id of item
	virtual u32 addItem(const wchar_t* text) _IRR_OVERRIDE_;

	//! clears the list
	virtual void clear() _IRR_OVERRIDE_;

	//! returns id of selected item. returns -1 if no item is selected.
	virtual s32 getSelected() const _IRR_OVERRIDE_;

	//! sets the selected item. Set this to -1 if no item should be selected
	virtual void setSelected(s32 id) _IRR_OVERRIDE_;

	//! sets the selected item. Set this to -1 if no item should be selected
	virtual void setSelected(const wchar_t *item) _IRR_OVERRIDE_;

	//! called if an event happened.
	virtual bool OnEvent(const SEvent& event) _IRR_OVERRIDE_;

	//! draws the element and its children
	virtual void draw() _IRR_OVERRIDE_;

	//! adds an list item with an icon
	//! \param text Text of list entry
	//! \param icon Sprite index of the Icon within the current sprite bank. Set it to -1 if you want no icon
	//! \return
	//! returns the id of the new created item
	virtual u32 addItem(const wchar_t* text, s32 icon) _IRR_OVERRIDE_;

	//! Returns the icon of an item
	virtual s32 getIcon(u32 id) const _IRR_OVERRIDE_;

	//! removes an item from the list
	virtual void removeItem(u32 id) _IRR_OVERRIDE_;

	//! get the the id of the item at the given absolute coordinates
	virtual s32 getItemAt(s32 xpos, s32 ypos) const _IRR_OVERRIDE_;

	//! Sets the sprite bank which should be used to draw list icons. This font is set to the sprite bank of
	//! the built-in-font by default. A sprite can be displayed in front of every list item.
	//! An icon is an index within the icon sprite bank. Several default icons are available in the
	//! skin through getIcon
	virtual void setSpriteBank(IGUISpriteBank* bank) _IRR_OVERRIDE_;

	//! set whether the listbox should scroll to newly selected items
	virtual void setAutoScrollEnabled(bool scroll) _IRR_OVERRIDE_;

	//! returns true if automatic scrolling is enabled, false if not.
	virtual bool isAutoScrollEnabled() const _IRR_OVERRIDE_;

	//! Update the position and size of the listbox, and update the scrollbar
	virtual void updateAbsolutePosition() _IRR_OVERRIDE_;

	//! Writes attributes of the element.
	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const _IRR_OVERRIDE_;

	//! Reads attributes of the element
	virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options) _IRR_OVERRIDE_;

	//! set all item colors at given index to color
	virtual void setItemOverrideColor(u32 index, video::SColor color) _IRR_OVERRIDE_;

	//! set all item colors of specified type at given index to color
	virtual void setItemOverrideColor(u32 index, EGUI_LISTBOX_COLOR colorType, video::SColor color) _IRR_OVERRIDE_;

	//! clear all item colors at index
	virtual void clearItemOverrideColor(u32 index) _IRR_OVERRIDE_;

	//! clear item color at index for given colortype
	virtual void clearItemOverrideColor(u32 index, EGUI_LISTBOX_COLOR colorType) _IRR_OVERRIDE_;

	//! has the item at index its color overwritten?
	virtual bool hasItemOverrideColor(u32 index, EGUI_LISTBOX_COLOR colorType) const _IRR_OVERRIDE_;

	//! return the overwrite color at given item index.
	virtual video::SColor getItemOverrideColor(u32 index, EGUI_LISTBOX_COLOR colorType) const _IRR_OVERRIDE_;

	//! return the default color which is used for the given colorType
	virtual video::SColor getItemDefaultColor(EGUI_LISTBOX_COLOR colorType) const _IRR_OVERRIDE_;

	//! set the item at the given index
	virtual void setItem(u32 index, const wchar_t* text, s32 icon) _IRR_OVERRIDE_;

	//! Insert the item at the given index
	//! Return the index on success or -1 on failure.
	virtual s32 insertItem(u32 index, const wchar_t* text, s32 icon) _IRR_OVERRIDE_;

	//! Swap the items at the given indices
	virtual void swapItems(u32 index1, u32 index2) _IRR_OVERRIDE_;

	//! set global itemHeight
	virtual void setItemHeight(s32 height) _IRR_OVERRIDE_;

	//! Sets whether to draw the background
	virtual void setDrawBackground(bool draw) _IRR_OVERRIDE_;

#if IRRLICHT_VERSION_MAJOR==1 && IRRLICHT_VERSION_MINOR==9
	virtual IGUIScrollBar* getVerticalScrollBar() const _IRR_OVERRIDE_;
#endif

	void refreshList();

	void resetPath();

	void setWorkingPath(const std::wstring& newDirectory, bool setAsRoot = false);

	using callback = bool(std::wstring, bool, void*);

	void addFilterFunction(callback* function);

	void addFilteredExtensions(std::vector<std::wstring> extensions);

	bool defaultFilter(std::wstring name, bool is_directory, void*);

	void nativeDirectory(bool native_directory);

	bool isDirectory(u32 index);

	void enterDirectory(u32 index);


private:

	struct ListItem {
		ListItem() : icon(-1) {
		}

		std::wstring reltext;
		bool isDirectory;
		std::wstring text;
		s32 icon;

		// A multicolor extension
		struct ListItemOverrideColor {
			ListItemOverrideColor() : Use(false) {}
			bool Use;
			video::SColor Color;
		};
		ListItemOverrideColor OverrideColors[EGUI_LBC_COUNT];

		bool operator ==(const struct ListItem& other) const {
			if(isDirectory != other.isDirectory)
				return false;

			return ygo::Utils::EqualIgnoreCase(reltext, other.reltext);
		}

		bool operator <(const struct ListItem& other) const {
			if(isDirectory != other.isDirectory)
				return isDirectory;

			return ygo::Utils::CompareIgnoreCase(reltext, other.reltext);
		}
	};

	void recalculateItemHeight();
	void selectNew(s32 ypos, bool onlyHover = false);
	void recalculateScrollPos();

	// extracted that function to avoid copy&paste code
	void recalculateItemWidth(s32 icon);

	// get labels used for serialization
	bool getSerializationLabels(EGUI_LISTBOX_COLOR colorType, core::stringc & useColorLabel, core::stringc & colorLabel) const;

	void LoadFolderContents();

	std::wstring basePath;
	std::wstring prevRelPath;
	std::wstring curRelPath;
	io::IFileSystem* filesystem;
	callback* filter;
	//std::function<bool(std::wstring, bool, void*)> filter;
	std::vector<std::wstring> filtered_extensions;

	std::vector<ListItem> Items;
	s32 Selected;
	s32 ItemHeight;
	s32 ItemHeightOverride;
	s32 TotalItemHeight;
	s32 ItemsIconWidth;
	gui::IGUIFont* Font;
	gui::IGUISpriteBank* IconBank;
	gui::IGUIScrollBar* ScrollBar;
	u32 selectTime;
	u32 LastKeyTime;
	std::wstring KeyBuffer;
	bool Selecting;
	int TotalFolders;
	bool BaseIsRoot;
	bool DrawBack;
	bool MoveOverSelect;
	bool AutoScroll;
	bool HighlightWhenNotFocused;
	bool NativeDirectoryHandling;
};


} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

#endif