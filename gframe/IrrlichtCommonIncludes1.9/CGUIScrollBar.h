// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_GUI_SCROLL_BAR_H_INCLUDED__
#define __C_GUI_SCROLL_BAR_H_INCLUDED__

#include <IrrCompileConfig.h>
#ifdef _IRR_COMPILE_WITH_GUI_

#include <IGUIScrollBar.h>
#include <IGUIButton.h>

namespace irr {
namespace gui {

class CGUIScrollBar : public IGUIScrollBar {
public:

	//! constructor
	CGUIScrollBar(bool horizontal, IGUIEnvironment* environment,
				  IGUIElement* parent, s32 id, core::rect<s32> rectangle,
				  bool noclip = false);

	//! destructor
	virtual ~CGUIScrollBar();

	//! called if an event happened.
	virtual bool OnEvent(const SEvent& event) _IRR_OVERRIDE_;

	//! draws the element and its children
	virtual void draw() _IRR_OVERRIDE_;

	virtual void OnPostRender(u32 timeMs) _IRR_OVERRIDE_;


	//! gets the maximum value of the scrollbar.
	virtual s32 getMax() const _IRR_OVERRIDE_;

	//! sets the maximum value of the scrollbar.
	virtual void setMax(s32 max) _IRR_OVERRIDE_;

	//! gets the minimum value of the scrollbar.
	virtual s32 getMin() const _IRR_OVERRIDE_;

	//! sets the minimum value of the scrollbar.
	virtual void setMin(s32 min) _IRR_OVERRIDE_;

	//! gets the small step value
	virtual s32 getSmallStep() const _IRR_OVERRIDE_;

	//! sets the small step value
	virtual void setSmallStep(s32 step) _IRR_OVERRIDE_;

	//! gets the large step value
	virtual s32 getLargeStep() const _IRR_OVERRIDE_;

	//! sets the large step value
	virtual void setLargeStep(s32 step) _IRR_OVERRIDE_;

	//! gets the current position of the scrollbar
	virtual s32 getPos() const _IRR_OVERRIDE_;

	//! sets the position of the scrollbar
	virtual void setPos(s32 pos) _IRR_OVERRIDE_;

	//! updates the rectangle
	virtual void updateAbsolutePosition() _IRR_OVERRIDE_;

	//! Writes attributes of the element.
	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const _IRR_OVERRIDE_;

	//! Reads attributes of the element
	virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options) _IRR_OVERRIDE_;

private:

	void refreshControls();
	s32 getPosFromMousePos(const core::position2di& p) const;

	IGUIButton* UpButton;
	IGUIButton* DownButton;

	core::rect<s32> SliderRect;

	bool Dragging;
	bool Horizontal;
	bool DraggedBySlider;
	bool TrayClick;
	s32 Pos;
	s32 DrawPos;
	s32 DrawHeight;
	s32 Min;
	s32 Max;
	s32 SmallStep;
	s32 LargeStep;
	s32 DesiredPos;
	u32 LastChange;
	video::SColor CurrentIconColor;

	f32 range() const { return (f32)(Max - Min); }
};

} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

#endif

