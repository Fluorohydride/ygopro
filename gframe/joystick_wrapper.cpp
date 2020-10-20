#include "joystick_wrapper.h"
#ifndef YGOPRO_USE_JOYSTICK
JWrapper::JWrapper(irr::IrrlichtDevice* _device) {
}

JWrapper::~JWrapper() {
}

void JWrapper::ProcessEvents() {
}
#else
#include <SDL2/SDL.h>
#include <IrrlichtDevice.h>
#include <fmt/printf.h>
#include <IEventReceiver.h>
JWrapper::JWrapper(irr::IrrlichtDevice* _device) {
	device = _device;
	if(SDL_InitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) < 0) {
		fmt::printf("Couldn't initialize SDL: %s\n", SDL_GetError());
		return;
	}
	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_CRITICAL);
	init = true;
}

JWrapper::~JWrapper() {
	if(gamecontroller)
		SDL_GameControllerClose(gamecontroller);
	SDL_QuitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);
}

void JWrapper::ProcessEvents() {
	if(!init)
		return;
	SDL_Event event;
	uint16_t changed = 0;
	while(SDL_PollEvent(&event)) {
		switch(event.type) {
			case SDL_CONTROLLERDEVICEADDED:
				fmt::printf("Game controller device %d added.\n", (int)event.cdevice.which);
				if(!gamecontroller) {
					gamecontroller = SDL_GameControllerOpen(event.cdevice.which);
					/*if(gamecontroller) {
						SDL_Log("Game controller device %s added.\n", SDL_GameControllerName(gamecontroller));
					} else {
						SDL_Log("Couldn't open controller: %s\n", SDL_GetError());
					}*/
				}
				break;

			case SDL_CONTROLLERDEVICEREMOVED:
				//SDL_Log("Game controller device %d removed.\n", (int)event.cdevice.which);
				if(gamecontroller && event.cdevice.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(gamecontroller))) {
					SDL_GameControllerClose(gamecontroller);
					gamecontroller = SDL_GameControllerOpen(0);
					if(gamecontroller) {
						//SDL_Log("Game controller device %s added.\n", SDL_GameControllerName(gamecontroller));
					}
				}
				break;

			case SDL_CONTROLLERAXISMOTION:
				//SDL_Log("Controller axis %s changed to %d\n", SDL_GameControllerGetStringForAxis((SDL_GameControllerAxis)event.caxis.axis), event.caxis.value);
				break;
			case SDL_CONTROLLERBUTTONDOWN:
			case SDL_CONTROLLERBUTTONUP:
				changed |= 1 << event.cbutton.button;
				//SDL_Log("Controller button %s %s\n", SDL_GameControllerGetStringForButton((SDL_GameControllerButton)event.cbutton.button), event.cbutton.state ? "pressed" : "released");
				break;
			default:
				break;
		}
	}

	if(gamecontroller) {
		irr::SEvent joyevent;
		joyevent.EventType = irr::EET_JOYSTICK_INPUT_EVENT;
		joyevent.JoystickEvent.ButtonStates = 0;
		for(int j = 0; j < SDL_CONTROLLER_BUTTON_MAX; ++j)
			joyevent.JoystickEvent.ButtonStates |= (SDL_GameControllerGetButton(gamecontroller, (SDL_GameControllerButton)j) << j);

		joyevent.JoystickEvent.Axis[Axis::LEFTX] = 0;
		joyevent.JoystickEvent.Axis[Axis::LEFTY] = 0;
		joyevent.JoystickEvent.Axis[Axis::RIGHTX] = 0;
		joyevent.JoystickEvent.Axis[Axis::RIGHTY] = 0;
		joyevent.JoystickEvent.Axis[Axis::TRIGGERLEFT] = 0;
		joyevent.JoystickEvent.Axis[Axis::TRIGGERRIGHT] = 0;
		for(int i = 0; i < SDL_CONTROLLER_AXIS_MAX; ++i)
			joyevent.JoystickEvent.Axis[i] = SDL_GameControllerGetAxis(gamecontroller, (SDL_GameControllerAxis)(i));
		joyevent.JoystickEvent.POV = changed;
		joyevent.JoystickEvent.Joystick = 0;
		device->postEventFromUser(joyevent);
	}
}
#endif
