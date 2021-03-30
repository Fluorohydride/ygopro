// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_IRR_DEVICE_STUB_H_INCLUDED__
#define __C_IRR_DEVICE_STUB_H_INCLUDED__

#include <IrrlichtDevice.h>
#include "IImagePresenter.h"
#include <SIrrCreationParameters.h>
#include "CVideoModeList.h"
#include <IContextManager.h>

namespace irr
{
	// lots of prototypes:
	class ILogger;
	class CLogger;
	class IRandomizer;

	namespace gui
	{
		class IGUIEnvironment;
		IGUIEnvironment* createGUIEnvironment(io::IFileSystem* fs,
			video::IVideoDriver* Driver, IOSOperator* op);
	}

	namespace scene
	{
		ISceneManager* createSceneManager(video::IVideoDriver* driver,
			io::IFileSystem* fs, gui::ICursorControl* cc, gui::IGUIEnvironment *gui);
	}

	namespace io
	{
		IFileSystem* createFileSystem();
	}

	namespace video
	{
		IVideoDriver* createSoftwareDriver(const core::dimension2d<u32>& windowSize,
				bool fullscreen, io::IFileSystem* io,
				video::IImagePresenter* presenter);
		IVideoDriver* createBurningVideoDriver(const irr::SIrrlichtCreationParameters& params,
				io::IFileSystem* io, video::IImagePresenter* presenter);
		IVideoDriver* createNullDriver(io::IFileSystem* io, const core::dimension2d<u32>& screenSize);
	}



	//! Stub for an Irrlicht Device implementation
	class CIrrDeviceStub : public IrrlichtDevice
	{
	public:

		//! constructor
		CIrrDeviceStub(const SIrrlichtCreationParameters& param);

		//! destructor
		virtual ~CIrrDeviceStub();

		//! returns the video driver
		virtual video::IVideoDriver* getVideoDriver() _IRR_OVERRIDE_;

		//! return file system
		virtual io::IFileSystem* getFileSystem() _IRR_OVERRIDE_;

		//! returns the gui environment
		virtual gui::IGUIEnvironment* getGUIEnvironment() _IRR_OVERRIDE_;

		//! returns the scene manager
		virtual scene::ISceneManager* getSceneManager() _IRR_OVERRIDE_;

		//! \return Returns a pointer to the mouse cursor control interface.
		virtual gui::ICursorControl* getCursorControl() _IRR_OVERRIDE_;

		//! Returns a pointer to a list with all video modes supported by the gfx adapter.
		virtual video::IVideoModeList* getVideoModeList() _IRR_OVERRIDE_;

		//! return the context manager
		virtual video::IContextManager* getContextManager() _IRR_OVERRIDE_;

		//! Returns a pointer to the ITimer object. With it the current Time can be received.
		virtual ITimer* getTimer() _IRR_OVERRIDE_;

		//! Returns the version of the engine.
		virtual const char* getVersion() const _IRR_OVERRIDE_;

		//! send the event to the right receiver
		virtual bool postEventFromUser(const SEvent& event) _IRR_OVERRIDE_;

		//! Sets a new event receiver to receive events
		virtual void setEventReceiver(IEventReceiver* receiver) _IRR_OVERRIDE_;

		//! Returns pointer to the current event receiver. Returns 0 if there is none.
		virtual IEventReceiver* getEventReceiver() _IRR_OVERRIDE_;

		//! Sets the input receiving scene manager.
		/** If set to null, the main scene manager (returned by GetSceneManager()) will receive the input */
		virtual void setInputReceivingSceneManager(scene::ISceneManager* sceneManager) _IRR_OVERRIDE_;

		//! Returns a pointer to the logger.
		virtual ILogger* getLogger() _IRR_OVERRIDE_;

		//! Provides access to the engine's currently set randomizer.
		virtual IRandomizer* getRandomizer() const _IRR_OVERRIDE_;

		//! Sets a new randomizer.
		virtual void setRandomizer(IRandomizer* r) _IRR_OVERRIDE_;

		//! Creates a new default randomizer.
		virtual IRandomizer* createDefaultRandomizer() const _IRR_OVERRIDE_;

		//! Returns the operation system opertator object.
		virtual IOSOperator* getOSOperator() _IRR_OVERRIDE_;

		//! Checks if the window is running in fullscreen mode.
		virtual bool isFullscreen() const _IRR_OVERRIDE_;

		//! get color format of the current window
		virtual video::ECOLOR_FORMAT getColorFormat() const _IRR_OVERRIDE_;

		//! Activate any joysticks, and generate events for them.
		virtual bool activateJoysticks(core::array<SJoystickInfo> & joystickInfo) _IRR_OVERRIDE_;

        //! Activate accelerometer.
        virtual bool activateAccelerometer(float updateInterval = 0.016666f) _IRR_OVERRIDE_;
        
        //! Deactivate accelerometer.
        virtual bool deactivateAccelerometer() _IRR_OVERRIDE_;
        
        //! Is accelerometer active.
        virtual bool isAccelerometerActive() _IRR_OVERRIDE_;
        
        //! Is accelerometer available.
        virtual bool isAccelerometerAvailable() _IRR_OVERRIDE_;
        
        //! Activate gyroscope.
        virtual bool activateGyroscope(float updateInterval = 0.016666f) _IRR_OVERRIDE_;
        
        //! Deactivate gyroscope.
        virtual bool deactivateGyroscope() _IRR_OVERRIDE_;
        
        //! Is gyroscope active.
        virtual bool isGyroscopeActive() _IRR_OVERRIDE_;
        
        //! Is gyroscope available.
        virtual bool isGyroscopeAvailable() _IRR_OVERRIDE_;
        
        //! Activate device motion.
        virtual bool activateDeviceMotion(float updateInterval = 0.016666f) _IRR_OVERRIDE_;
        
        //! Deactivate device motion.
        virtual bool deactivateDeviceMotion() _IRR_OVERRIDE_;
        
        //! Is device motion active.
        virtual bool isDeviceMotionActive() _IRR_OVERRIDE_;
        
        //! Is device motion available.
        virtual bool isDeviceMotionAvailable() _IRR_OVERRIDE_;

		//! Set the current Gamma Value for the Display
		virtual bool setGammaRamp( f32 red, f32 green, f32 blue, f32 brightness, f32 contrast ) _IRR_OVERRIDE_;

		//! Get the current Gamma Value for the Display
		virtual bool getGammaRamp( f32 &red, f32 &green, f32 &blue, f32 &brightness, f32 &contrast ) _IRR_OVERRIDE_;

		//! Set the maximal elapsed time between 2 clicks to generate doubleclicks for the mouse. It also affects tripleclick behavior.
		//! When set to 0 no double- and tripleclicks will be generated.
		virtual void setDoubleClickTime( u32 timeMs ) _IRR_OVERRIDE_;

		//! Get the maximal elapsed time between 2 clicks to generate double- and tripleclicks for the mouse.
		virtual u32 getDoubleClickTime() const _IRR_OVERRIDE_;

		//! Remove all messages pending in the system message loop
		virtual void clearSystemMessages() _IRR_OVERRIDE_;

		//! Resize the render window.
		virtual void setWindowSize(const irr::core::dimension2d<u32>& size) _IRR_OVERRIDE_ {}

	protected:

		void createGUIAndScene();

		//! checks version of SDK and prints warning if there might be a problem
		bool checkVersion(const char* version);

		//! Compares to the last call of this function to return double and triple clicks.
		/** Needed for win32 device event handling
		\return Returns only 1,2 or 3. A 4th click will start with 1 again.
		*/
		virtual u32 checkSuccessiveClicks(s32 mouseX, s32 mouseY, EMOUSE_INPUT_EVENT inputEvent);

		void calculateGammaRamp ( u16 *ramp, f32 gamma, f32 relativebrightness, f32 relativecontrast );
		void calculateGammaFromRamp ( f32 &gamma, const u16 *ramp );

		video::IVideoDriver* VideoDriver;
		gui::IGUIEnvironment* GUIEnvironment;
		scene::ISceneManager* SceneManager;
		ITimer* Timer;
		gui::ICursorControl* CursorControl;
		IEventReceiver* UserReceiver;
		CLogger* Logger;
		IOSOperator* Operator;
		IRandomizer* Randomizer;
		io::IFileSystem* FileSystem;
		scene::ISceneManager* InputReceivingSceneManager;

		struct SMouseMultiClicks
		{
			SMouseMultiClicks()
				: DoubleClickTime(500), CountSuccessiveClicks(0), LastClickTime(0), LastMouseInputEvent(EMIE_COUNT)
			{}

			u32 DoubleClickTime;
			u32 CountSuccessiveClicks;
			u32 LastClickTime;
			core::position2di LastClick;
			EMOUSE_INPUT_EVENT LastMouseInputEvent;
		};
		SMouseMultiClicks MouseMultiClicks;
		video::CVideoModeList* VideoModeList;
		video::IContextManager* ContextManager;
		SIrrlichtCreationParameters CreationParams;
		bool Close;
	};

} // end namespace irr

#endif

