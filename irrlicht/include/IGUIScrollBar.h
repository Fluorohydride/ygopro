// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_GUI_SCROLL_BAR_H_INCLUDED__
#define __I_GUI_SCROLL_BAR_H_INCLUDED__

#include "IGUIElement.h"

namespace irr
{
namespace gui
{

	//! Default scroll bar GUI element.
	/** \par This element can create the following events of type EGUI_EVENT_TYPE:
	\li EGET_SCROLL_BAR_CHANGED
	*/
	class IGUIScrollBar : public IGUIElement
	{
	public:

		//! constructor
		IGUIScrollBar(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
			: IGUIElement(EGUIET_SCROLL_BAR, environment, parent, id, rectangle) {}

		//! sets the maximum value of the scrollbar.
		virtual void setMax(s32 max) = 0;
		//! gets the maximum value of the scrollbar.
		virtual s32 getMax() const = 0;

		//! sets the minimum value of the scrollbar.
		virtual void setMin(s32 min) = 0;
		//! gets the minimum value of the scrollbar.
		virtual s32 getMin() const = 0;

		//! gets the small step value
		virtual s32 getSmallStep() const = 0;

		//! Sets the small step
		/** That is the amount that the value changes by when clicking
		on the buttons or using the cursor keys. */
		virtual void setSmallStep(s32 step) = 0;

		//! gets the large step value
		virtual s32 getLargeStep() const = 0;

		//! Sets the large step
		/** That is the amount that the value changes by when clicking
		in the tray, or using the page up and page down keys. */
		virtual void setLargeStep(s32 step) = 0;

		//! gets the current position of the scrollbar
		virtual s32 getPos() const = 0;

		//! sets the current position of the scrollbar
		virtual void setPos(s32 pos) = 0;
	};


} // end namespace gui
} // end namespace irr

#endif

