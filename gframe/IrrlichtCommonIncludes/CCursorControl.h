#include <ICursorControl.h>
#include <WindowsX.h>

namespace irr {

class CIrrDeviceWin32;

class CCursorControl : public gui::ICursorControl {
public:

	CCursorControl(CIrrDeviceWin32* device, const core::dimension2d<u32>& wsize, HWND hwnd, bool fullscreen);
	~CCursorControl();

	//! Changes the visible state of the mouse cursor.
	virtual void setVisible(bool visible) {
		CURSORINFO info;
		info.cbSize = sizeof(CURSORINFO);
		BOOL gotCursorInfo = GetCursorInfo(&info);
		while(gotCursorInfo) {
#ifdef CURSOR_SUPPRESSED
			// Since Windows 8 the cursor can be suppressed by a touch interface
			if(visible && info.flags == CURSOR_SUPPRESSED) {
				break;
			}
#endif
			if((visible && info.flags == CURSOR_SHOWING) || // visible
				(!visible && info.flags == 0)) // hidden
			{
				break;
			}
			// this only increases an internal
			// display counter in windows, so it
			// might have to be called some more
			const int showResult = ShowCursor(visible);
			// if result has correct sign we can
			// stop here as well
			if((!visible && showResult < 0) ||
				(visible && showResult >= 0))
				break;
			// yes, it really must be set each time
			info.cbSize = sizeof(CURSORINFO);
			gotCursorInfo = GetCursorInfo(&info);

#ifdef CURSOR_SUPPRESSED
			// Not sure if a cursor which we tried to hide still can be suppressed.
			// I have no touch-display for testing this and MSDN doesn't describe it.
			// But adding this check shouldn't hurt and might prevent an endless loop.
			if(!visible && info.flags == CURSOR_SUPPRESSED) {
				break;
			}
#endif
		}
		IsVisible = visible;
	}

	//! Returns if the cursor is currently visible.
	virtual bool isVisible() const {
		_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
		return IsVisible;
	}

	//! Sets the new position of the cursor.
	virtual void setPosition(const core::position2d<f32> &pos) {
		setPosition(pos.X, pos.Y);
	}

	//! Sets the new position of the cursor.
	virtual void setPosition(f32 x, f32 y) {
		if(!UseReferenceRect)
			setPosition(core::round32(x*WindowSize.Width), core::round32(y*WindowSize.Height));
		else
			setPosition(core::round32(x*ReferenceRect.getWidth()), core::round32(y*ReferenceRect.getHeight()));
	}

	//! Sets the new position of the cursor.
	virtual void setPosition(const core::position2d<s32> &pos) {
		setPosition(pos.X, pos.Y);
	}

	//! Sets the new position of the cursor.
	virtual void setPosition(s32 x, s32 y) {
		if(UseReferenceRect) {
			SetCursorPos(ReferenceRect.UpperLeftCorner.X + x,
						 ReferenceRect.UpperLeftCorner.Y + y);
		} else {
			RECT rect;
			if(GetWindowRect(HWnd, &rect))
				SetCursorPos(x + rect.left + BorderX, y + rect.top + BorderY);
		}

		CursorPos.X = x;
		CursorPos.Y = y;
	}

	//! Returns the current position of the mouse cursor.
	virtual const core::position2d<s32>& getPosition() {
		updateInternalCursorPosition();
		return CursorPos;
	}

	//! Returns the current position of the mouse cursor.
	virtual core::position2d<f32> getRelativePosition() {
		updateInternalCursorPosition();

		if(!UseReferenceRect) {
			return core::position2d<f32>(CursorPos.X * InvWindowSize.Width,
										 CursorPos.Y * InvWindowSize.Height);
		}

		return core::position2d<f32>(CursorPos.X / (f32)ReferenceRect.getWidth(),
									 CursorPos.Y / (f32)ReferenceRect.getHeight());
	}

	//! Sets an absolute reference rect for calculating the cursor position.
	virtual void setReferenceRect(core::rect<s32>* rect = 0) {
		if(rect) {
			ReferenceRect = *rect;
			UseReferenceRect = true;

			// prevent division through zero and uneven sizes

			if(!ReferenceRect.getHeight() || ReferenceRect.getHeight() % 2)
				ReferenceRect.LowerRightCorner.Y += 1;

			if(!ReferenceRect.getWidth() || ReferenceRect.getWidth() % 2)
				ReferenceRect.LowerRightCorner.X += 1;
		} else
			UseReferenceRect = false;
	}

	/** Used to notify the cursor that the window was resized. */
	virtual void OnResize(const core::dimension2d<u32>& size) {
		WindowSize = size;
		if(size.Width != 0)
			InvWindowSize.Width = 1.0f / size.Width;
		else
			InvWindowSize.Width = 0.f;

		if(size.Height != 0)
			InvWindowSize.Height = 1.0f / size.Height;
		else
			InvWindowSize.Height = 0.f;
	}

	/** Used to notify the cursor that the window resizable settings changed. */
	void updateBorderSize(bool fullscreen, bool resizable) {
		if(!fullscreen) {
			if(resizable) {
				BorderX = GetSystemMetrics(SM_CXSIZEFRAME);
				BorderY = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYSIZEFRAME);
			} else {
				BorderX = GetSystemMetrics(SM_CXDLGFRAME);
				BorderY = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYDLGFRAME);
			}
		} else {
			BorderX = BorderY = 0;
		}
	}


	//! Sets the active cursor icon
	virtual void setActiveIcon(gui::ECURSOR_ICON iconId);

	//! Gets the currently active icon
	virtual gui::ECURSOR_ICON getActiveIcon() const {
		return ActiveIcon;
	}

	//! Add a custom sprite as cursor icon.
	virtual gui::ECURSOR_ICON addIcon(const gui::SCursorSprite& icon);

	//! replace the given cursor icon.
	virtual void changeIcon(gui::ECURSOR_ICON iconId, const gui::SCursorSprite& icon);

	//! Return a system-specific size which is supported for cursors. Larger icons will fail, smaller icons might work.
	virtual core::dimension2di getSupportedIconSize() const;

	void update();

private:

	//! Updates the internal cursor position
	void updateInternalCursorPosition() {
		POINT p;
		if(!GetCursorPos(&p)) {
			DWORD xy = GetMessagePos();
			p.x = GET_X_LPARAM(xy);
			p.y = GET_Y_LPARAM(xy);
		}

		if(UseReferenceRect) {
			CursorPos.X = p.x - ReferenceRect.UpperLeftCorner.X;
			CursorPos.Y = p.y - ReferenceRect.UpperLeftCorner.Y;
		} else {
			RECT rect;
			if(GetWindowRect(HWnd, &rect)) {
				CursorPos.X = p.x - rect.left - BorderX;
				CursorPos.Y = p.y - rect.top - BorderY;
			} else {
				// window seems not to be existent, so set cursor to
				// a negative value
				CursorPos.X = -1;
				CursorPos.Y = -1;
			}
		}
	}

	CIrrDeviceWin32* Device;
	core::position2d<s32> CursorPos;
	core::dimension2d<u32> WindowSize;
	core::dimension2d<f32> InvWindowSize;
	HWND HWnd;

	s32 BorderX, BorderY;
	core::rect<s32> ReferenceRect;
	bool UseReferenceRect;
	bool IsVisible;


	struct CursorFrameW32 {
		CursorFrameW32() : IconHW(0) {}
		CursorFrameW32(HCURSOR icon) : IconHW(icon) {}

		HCURSOR IconHW;	// hardware cursor
	};

	struct CursorW32 {
		CursorW32() {}
		explicit CursorW32(HCURSOR iconHw, u32 frameTime = 0) : FrameTime(frameTime) {
			Frames.push_back(CursorFrameW32(iconHw));
		}
		core::array<CursorFrameW32> Frames;
		u32 FrameTime;
	};

	core::array<CursorW32> Cursors;
	gui::ECURSOR_ICON ActiveIcon;
	u32 ActiveIconStartTime;

	void initCursors();
};

};