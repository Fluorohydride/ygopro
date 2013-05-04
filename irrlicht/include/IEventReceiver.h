// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_EVENT_RECEIVER_H_INCLUDED__
#define __I_EVENT_RECEIVER_H_INCLUDED__

#include "ILogger.h"
#include "Keycodes.h"
#include "irrString.h"

namespace irr
{
	//! Enumeration for all event types there are.
	enum EEVENT_TYPE
	{
		//! An event of the graphical user interface.
		/** GUI events are created by the GUI environment or the GUI elements in response
		to mouse or keyboard events. When a GUI element receives an event it will either
		process it and return true, or pass the event to its parent. If an event is not absorbed
		before it reaches the root element then it will then be passed to the user receiver. */
		EET_GUI_EVENT = 0,

		//! A mouse input event.
		/** Mouse events are created by the device and passed to IrrlichtDevice::postEventFromUser
		in response to mouse input received from the operating system.
		Mouse events are first passed to the user receiver, then to the GUI environment and its elements,
		then finally the input receiving scene manager where it is passed to the active camera.
		*/
		EET_MOUSE_INPUT_EVENT,

		//! A key input event.
		/** Like mouse events, keyboard events are created by the device and passed to
		IrrlichtDevice::postEventFromUser. They take the same path as mouse events. */
		EET_KEY_INPUT_EVENT,

		//! A joystick (joypad, gamepad) input event.
		/** Joystick events are created by polling all connected joysticks once per
		device run() and then passing the events to IrrlichtDevice::postEventFromUser.
		They take the same path as mouse events.
		Windows, SDL: Implemented.
		Linux: Implemented, with POV hat issues.
		MacOS / Other: Not yet implemented.
		*/
		EET_JOYSTICK_INPUT_EVENT,

		//! A log event
		/** Log events are only passed to the user receiver if there is one. If they are absorbed by the
		user receiver then no text will be sent to the console. */
		EET_LOG_TEXT_EVENT,

		//! A user event with user data.
		/** This is not used by Irrlicht and can be used to send user
		specific data though the system. The Irrlicht 'window handle'
		can be obtained from IrrlichtDevice::getExposedVideoData()
		The usage and behavior depends on the operating system:
		Windows: send a WM_USER message to the Irrlicht Window; the
			wParam and lParam will be used to populate the
			UserData1 and UserData2 members of the SUserEvent.
		Linux: send a ClientMessage via XSendEvent to the Irrlicht
			Window; the data.l[0] and data.l[1] members will be
			casted to s32 and used as UserData1 and UserData2.
		MacOS: Not yet implemented
		*/
		EET_USER_EVENT,

		//! This enum is never used, it only forces the compiler to
		//! compile these enumeration values to 32 bit.
		EGUIET_FORCE_32_BIT = 0x7fffffff

	};

	//! Enumeration for all mouse input events
	enum EMOUSE_INPUT_EVENT
	{
		//! Left mouse button was pressed down.
		EMIE_LMOUSE_PRESSED_DOWN = 0,

		//! Right mouse button was pressed down.
		EMIE_RMOUSE_PRESSED_DOWN,

		//! Middle mouse button was pressed down.
		EMIE_MMOUSE_PRESSED_DOWN,

		//! Left mouse button was left up.
		EMIE_LMOUSE_LEFT_UP,

		//! Right mouse button was left up.
		EMIE_RMOUSE_LEFT_UP,

		//! Middle mouse button was left up.
		EMIE_MMOUSE_LEFT_UP,

		//! The mouse cursor changed its position.
		EMIE_MOUSE_MOVED,

		//! The mouse wheel was moved. Use Wheel value in event data to find out
		//! in what direction and how fast.
		EMIE_MOUSE_WHEEL,

		//! Left mouse button double click.
		//! This event is generated after the second EMIE_LMOUSE_PRESSED_DOWN event.
		EMIE_LMOUSE_DOUBLE_CLICK,

		//! Right mouse button double click.
		//! This event is generated after the second EMIE_RMOUSE_PRESSED_DOWN event.
		EMIE_RMOUSE_DOUBLE_CLICK,

		//! Middle mouse button double click.
		//! This event is generated after the second EMIE_MMOUSE_PRESSED_DOWN event.
		EMIE_MMOUSE_DOUBLE_CLICK,

		//! Left mouse button triple click.
		//! This event is generated after the third EMIE_LMOUSE_PRESSED_DOWN event.
		EMIE_LMOUSE_TRIPLE_CLICK,

		//! Right mouse button triple click.
		//! This event is generated after the third EMIE_RMOUSE_PRESSED_DOWN event.
		EMIE_RMOUSE_TRIPLE_CLICK,

		//! Middle mouse button triple click.
		//! This event is generated after the third EMIE_MMOUSE_PRESSED_DOWN event.
		EMIE_MMOUSE_TRIPLE_CLICK,

		//! No real event. Just for convenience to get number of events
		EMIE_COUNT
	};

	//! Masks for mouse button states
	enum E_MOUSE_BUTTON_STATE_MASK
	{
		EMBSM_LEFT    = 0x01,
		EMBSM_RIGHT   = 0x02,
		EMBSM_MIDDLE  = 0x04,

		//! currently only on windows
		EMBSM_EXTRA1  = 0x08,

		//! currently only on windows
		EMBSM_EXTRA2  = 0x10,

		EMBSM_FORCE_32_BIT = 0x7fffffff
	};

	namespace gui
	{

		class IGUIElement;

		//! Enumeration for all events which are sendable by the gui system
		enum EGUI_EVENT_TYPE
		{
			//! A gui element has lost its focus.
			/** GUIEvent.Caller is losing the focus to GUIEvent.Element.
			If the event is absorbed then the focus will not be changed. */
			EGET_ELEMENT_FOCUS_LOST = 0,

			//! A gui element has got the focus.
			/** If the event is absorbed then the focus will not be changed. */
			EGET_ELEMENT_FOCUSED,

			//! The mouse cursor hovered over a gui element.
			/** If an element has sub-elements you also get this message for the subelements */
			EGET_ELEMENT_HOVERED,

			//! The mouse cursor left the hovered element.
			/** If an element has sub-elements you also get this message for the subelements */
			EGET_ELEMENT_LEFT,

			//! An element would like to close.
			/** Windows and context menus use this event when they would like to close,
			this can be cancelled by absorbing the event. */
			EGET_ELEMENT_CLOSED,

			//! A button was clicked.
			EGET_BUTTON_CLICKED,

			//! A scrollbar has changed its position.
			EGET_SCROLL_BAR_CHANGED,

			//! A checkbox has changed its check state.
			EGET_CHECKBOX_CHANGED,

			//! A new item in a listbox was selected.
			/** NOTE: You also get this event currently when the same item was clicked again after more than 500 ms. */
			EGET_LISTBOX_CHANGED,

			//! An item in the listbox was selected, which was already selected.
			/** NOTE: You get the event currently only if the item was clicked again within 500 ms or selected by "enter" or "space". */
			EGET_LISTBOX_SELECTED_AGAIN,

			//! A file has been selected in the file dialog
			EGET_FILE_SELECTED,

			//! A directory has been selected in the file dialog
			EGET_DIRECTORY_SELECTED,

			//! A file open dialog has been closed without choosing a file
			EGET_FILE_CHOOSE_DIALOG_CANCELLED,

			//! 'Yes' was clicked on a messagebox
			EGET_MESSAGEBOX_YES,

			//! 'No' was clicked on a messagebox
			EGET_MESSAGEBOX_NO,

			//! 'OK' was clicked on a messagebox
			EGET_MESSAGEBOX_OK,

			//! 'Cancel' was clicked on a messagebox
			EGET_MESSAGEBOX_CANCEL,

			//! In an editbox 'ENTER' was pressed
			EGET_EDITBOX_ENTER,

			//! The text in an editbox was changed. This does not include automatic changes in text-breaking.
			EGET_EDITBOX_CHANGED,

            //! The marked area in an editbox was changed.
			EGET_EDITBOX_MARKING_CHANGED,

			//! The tab was changed in an tab control
			EGET_TAB_CHANGED,

			//! A menu item was selected in a (context) menu
			EGET_MENU_ITEM_SELECTED,

			//! The selection in a combo box has been changed
			EGET_COMBO_BOX_CHANGED,

			//! The value of a spin box has changed
			EGET_SPINBOX_CHANGED,

			//! A table has changed
			EGET_TABLE_CHANGED,
			EGET_TABLE_HEADER_CHANGED,
			EGET_TABLE_SELECTED_AGAIN,

			//! A tree view node lost selection. See IGUITreeView::getLastEventNode().
			EGET_TREEVIEW_NODE_DESELECT,

			//! A tree view node was selected. See IGUITreeView::getLastEventNode().
			EGET_TREEVIEW_NODE_SELECT,

			//! A tree view node was expanded. See IGUITreeView::getLastEventNode().
			EGET_TREEVIEW_NODE_EXPAND,

			//! A tree view node was collapsed. See IGUITreeView::getLastEventNode().
			EGET_TREEVIEW_NODE_COLLAPSE,

			//! deprecated - use EGET_TREEVIEW_NODE_COLLAPSE instead. This
			//! may be removed by Irrlicht 1.9
			EGET_TREEVIEW_NODE_COLLAPS = EGET_TREEVIEW_NODE_COLLAPSE,

			//! No real event. Just for convenience to get number of events
			EGET_COUNT
		};
	} // end namespace gui


//! SEvents hold information about an event. See irr::IEventReceiver for details on event handling.
struct SEvent
{
	//! Any kind of GUI event.
	struct SGUIEvent
	{
		//! IGUIElement who called the event
		gui::IGUIElement* Caller;

		//! If the event has something to do with another element, it will be held here.
		gui::IGUIElement* Element;

		//! Type of GUI Event
		gui::EGUI_EVENT_TYPE EventType;

	};

	//! Any kind of mouse event.
	struct SMouseInput
	{
		//! X position of mouse cursor
		s32 X;

		//! Y position of mouse cursor
		s32 Y;

		//! mouse wheel delta, often 1.0 or -1.0, but can have other values < 0.f or > 0.f;
		/** Only valid if event was EMIE_MOUSE_WHEEL */
		f32 Wheel;

		//! True if shift was also pressed
		bool Shift:1;

		//! True if ctrl was also pressed
		bool Control:1;

		//! A bitmap of button states. You can use isButtonPressed() to determine
		//! if a button is pressed or not.
		//! Currently only valid if the event was EMIE_MOUSE_MOVED
		u32 ButtonStates;

		//! Is the left button pressed down?
		bool isLeftPressed() const { return 0 != ( ButtonStates & EMBSM_LEFT ); }

		//! Is the right button pressed down?
		bool isRightPressed() const { return 0 != ( ButtonStates & EMBSM_RIGHT ); }

		//! Is the middle button pressed down?
		bool isMiddlePressed() const { return 0 != ( ButtonStates & EMBSM_MIDDLE ); }

		//! Type of mouse event
		EMOUSE_INPUT_EVENT Event;
	};

	//! Any kind of keyboard event.
	struct SKeyInput
	{
		//! Character corresponding to the key (0, if not a character)
		wchar_t Char;

		//! Key which has been pressed or released
		EKEY_CODE Key;

		//! If not true, then the key was left up
		bool PressedDown:1;

		//! True if shift was also pressed
		bool Shift:1;

		//! True if ctrl was also pressed
		bool Control:1;
	};

	//! A joystick event.
	/** Unlike other events, joystick events represent the result of polling
	 * each connected joystick once per run() of the device. Joystick events will
	 * not be generated by default.  If joystick support is available for the
	 * active device, _IRR_COMPILE_WITH_JOYSTICK_EVENTS_ is defined, and
	 * @ref irr::IrrlichtDevice::activateJoysticks() has been called, an event of
	 * this type will be generated once per joystick per @ref IrrlichtDevice::run()
	 * regardless of whether the state of the joystick has actually changed. */
	struct SJoystickEvent
	{
		enum
		{
			NUMBER_OF_BUTTONS = 32,

			AXIS_X = 0, // e.g. analog stick 1 left to right
			AXIS_Y,		// e.g. analog stick 1 top to bottom
			AXIS_Z,		// e.g. throttle, or analog 2 stick 2 left to right
			AXIS_R,		// e.g. rudder, or analog 2 stick 2 top to bottom
			AXIS_U,
			AXIS_V,
			NUMBER_OF_AXES
		};

		/** A bitmap of button states.  You can use IsButtonPressed() to
		 ( check the state of each button from 0 to (NUMBER_OF_BUTTONS - 1) */
		u32 ButtonStates;

		/** For AXIS_X, AXIS_Y, AXIS_Z, AXIS_R, AXIS_U and AXIS_V
		 * Values are in the range -32768 to 32767, with 0 representing
		 * the center position.  You will receive the raw value from the
		 * joystick, and so will usually want to implement a dead zone around
		 * the center of the range. Axes not supported by this joystick will
		 * always have a value of 0. On Linux, POV hats are represented as axes,
		 * usually the last two active axis.
		 */
		s16 Axis[NUMBER_OF_AXES];

		/** The POV represents the angle of the POV hat in degrees * 100,
		 * from 0 to 35,900.  A value of 65535 indicates that the POV hat
		 * is centered (or not present).
		 * This value is only supported on Windows.  On Linux, the POV hat
		 * will be sent as 2 axes instead. */
		u16 POV;

		//! The ID of the joystick which generated this event.
		/** This is an internal Irrlicht index; it does not map directly
		 * to any particular hardware joystick. */
		u8 Joystick;

		//! A helper function to check if a button is pressed.
		bool IsButtonPressed(u32 button) const
		{
			if(button >= (u32)NUMBER_OF_BUTTONS)
				return false;

			return (ButtonStates & (1 << button)) ? true : false;
		}
	};


	//! Any kind of log event.
	struct SLogEvent
	{
		//! Pointer to text which has been logged
		const c8* Text;

		//! Log level in which the text has been logged
		ELOG_LEVEL Level;
	};

	//! Any kind of user event.
	struct SUserEvent
	{
		//! Some user specified data as int
		s32 UserData1;

		//! Another user specified data as int
		s32 UserData2;
	};

	EEVENT_TYPE EventType;
	union
	{
		struct SGUIEvent GUIEvent;
		struct SMouseInput MouseInput;
		struct SKeyInput KeyInput;
		struct SJoystickEvent JoystickEvent;
		struct SLogEvent LogEvent;
		struct SUserEvent UserEvent;
	};

};

//! Interface of an object which can receive events.
/** Many of the engine's classes inherit IEventReceiver so they are able to
process events. Events usually start at a postEventFromUser function and are
passed down through a chain of event receivers until OnEvent returns true. See
irr::EEVENT_TYPE for a description of where each type of event starts, and the
path it takes through the system. */
class IEventReceiver
{
public:

	//! Destructor
	virtual ~IEventReceiver() {}

	//! Called if an event happened.
	/** Please take care that you should only return 'true' when you want to _prevent_ Irrlicht
	* from processing the event any further. So 'true' does mean that an event is completely done.
	* Therefore your return value for all unprocessed events should be 'false'.
	\return True if the event was processed.
	*/
	virtual bool OnEvent(const SEvent& event) = 0;
};


//! Information on a joystick, returned from @ref irr::IrrlichtDevice::activateJoysticks()
struct SJoystickInfo
{
	//! The ID of the joystick
	/** This is an internal Irrlicht index; it does not map directly
	 * to any particular hardware joystick. It corresponds to the
	 * irr::SJoystickEvent Joystick ID. */
	u8				Joystick;

	//! The name that the joystick uses to identify itself.
	core::stringc	Name;

	//! The number of buttons that the joystick has.
	u32				Buttons;

	//! The number of axes that the joystick has, i.e. X, Y, Z, R, U, V.
	/** Note: with a Linux device, the POV hat (if any) will use two axes. These
	 *  will be included in this count. */
	u32				Axes;

	//! An indication of whether the joystick has a POV hat.
	/** A Windows device will identify the presence or absence or the POV hat.  A
	 *  Linux device cannot, and will always return POV_HAT_UNKNOWN. */
	enum
	{
		//! A hat is definitely present.
		POV_HAT_PRESENT,

		//! A hat is definitely not present.
		POV_HAT_ABSENT,

		//! The presence or absence of a hat cannot be determined.
		POV_HAT_UNKNOWN
	} PovHat;
}; // struct SJoystickInfo


} // end namespace irr

#endif

