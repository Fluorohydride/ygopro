#include "utils.h"
#include "game.h"

namespace ygo {
	Utils utils;
#ifdef _WIN32
	bool Utils::Makedirectory(const std::wstring& path) {
		return CreateDirectory(path.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError();
	}
	bool Utils::Makedirectory(const std::string& path) {
		return Makedirectory(BufferIO::DecodeUTF8s(path));
	}
#else
	bool Utils::Makedirectory(const std::wstring& path) {
		return Makedirectory(BufferIO::EncodeUTF8s(path));
	}
	bool Utils::Makedirectory(const std::string& path) {
		return !mkdir(&path[0], 0777) || errno == EEXIST;
	}
#endif
	bool Utils::Movefile(const std::string& source, const std::string& destination) {
		std::ifstream  src(source, std::ios::binary);
		if(!src.is_open())
			return false;
		std::ofstream  dst(destination, std::ios::binary);
		if(!dst.is_open())
			return false;
		dst << src.rdbuf();
		src.close();
		remove(source.c_str());
		return true;
	}
	bool Utils::Movefile(const std::wstring& source, const std::wstring& destination) {
		return Movefile(BufferIO::EncodeUTF8s(source), BufferIO::EncodeUTF8s(destination));
	}
	bool Utils::Deletefile(const std::string & source) {
		return remove(source.c_str()) == 0;
	}
	bool Utils::Deletefile(const std::wstring & source) {
		return Deletefile(BufferIO::EncodeUTF8s(source));
	}
	void Utils::initUtils() {
		//create directories if missing
		Makedirectory("deck");
		Makedirectory("pics");
		Makedirectory("pics/field");
		Makedirectory("replay");
		Makedirectory("screenshots");
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

	void Utils::changeCursor(irr::gui::ECURSOR_ICON icon) {
		irr::gui::ICursorControl* cursor = mainGame->device->getCursorControl();
		if (cursor->getActiveIcon() != icon) {
			cursor->setActiveIcon(icon);
		}
	}
}