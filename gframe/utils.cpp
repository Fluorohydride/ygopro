#include "utils.h"
#include "game.h"

namespace ygo {
	Utils utils;

	void Utils::initUtils() {
		//create directories if missing
#ifdef WIN32
		CreateDirectoryW(L"deck", NULL);
		CreateDirectoryW(L"pics", NULL);
		CreateDirectoryW(L"pics/field", NULL);
		CreateDirectoryW(L"replay", NULL);
		CreateDirectoryW(L"screenshots", NULL);
#else

#endif
	}

	void Utils::takeScreenshot(irr::IrrlichtDevice* device)
	{
		irr::video::IVideoDriver* const driver = device->getVideoDriver();

		//get image from the last rendered frame 
		irr::video::IImage* const image = driver->createScreenShot();
		if (image) //should always be true, but you never know. ;) 
		{
			//construct a filename, consisting of local time and file extension 
			irr::c8 filename[64];
			snprintf(filename, 64, "screenshots/ygopro_%u.png", device->getTimer()->getRealTime());

			//write screenshot to file 
			if (!driver->writeImageToFile(image, filename))
				device->getLogger()->log(L"Failed to take screenshot.", irr::ELL_WARNING);

			//Don't forget to drop image since we don't need it anymore. 
			image->drop();
		}
	}

	void Utils::changeCursor(ECURSOR_ICON icon) {
		gui::ICursorControl* cursor = mainGame->device->getCursorControl();
		if (cursor->getActiveIcon() != icon) {
			cursor->setActiveIcon(icon);
		}
	}
}