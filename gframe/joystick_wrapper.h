#ifndef JOYSTICK_WRAPPER_H
#define JOYSTICK_WRAPPER_H

namespace irr {
class IrrlichtDevice;
}
struct _SDL_GameController;
typedef struct _SDL_GameController SDL_GameController;

struct JWrapper {
	struct Buttons {
		enum {
			A = 1 << 0,
			B = 1 << 1,
			X = 1 << 2,
			Y = 1 << 3,
			BACK		= 1 << 4,
			GUIDE		= 1 << 5,
			START		= 1 << 6,
			LEFTSTICK	= 1 << 7,
			RIGHTSTICK		= 1 << 8,
			LEFTSHOULDER	= 1 << 9,
			RIGHTSHOULDER	= 1 << 10,
			DPAD_UP			= 1 << 11,
			DPAD_DOWN		= 1 << 12,
			DPAD_LEFT		= 1 << 13,
			DPAD_RIGHT		= 1 << 14
		};
	};
	struct Axis {
		enum {
			LEFTX,
			LEFTY,
			RIGHTX,
			RIGHTY,
			TRIGGERLEFT,
			TRIGGERRIGHT
		};
	};
	JWrapper(irr::IrrlichtDevice* device);
	~JWrapper();
	void ProcessEvents();
private:
	irr::IrrlichtDevice* device{ nullptr };
	bool init{ false };
	SDL_GameController* gamecontroller{ nullptr };
};

extern JWrapper* gJWrapper;

#endif //JOYSTICK_WRAPPER_H
