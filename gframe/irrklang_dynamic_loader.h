#ifndef IRRKLANG_DYNAMIC_LOADER_H
#define IRRKLANG_DYNAMIC_LOADER_H
#include <ik_ESoundOutputDrivers.h>

namespace irrklang{
class ISoundEngine;
}
class KlangLoader {
public:
	KlangLoader();
	~KlangLoader();
	irrklang::ISoundEngine* createIrrKlangDevice();
private:
	void* library{ nullptr };
	using CreateDevice = irrklang::ISoundEngine* (*)(irrklang::E_SOUND_OUTPUT_DRIVER, int, const char*, const char*);
	CreateDevice createdevice{ nullptr };
};


#endif //IRRKLANG_DYNAMIC_LOADER_H
