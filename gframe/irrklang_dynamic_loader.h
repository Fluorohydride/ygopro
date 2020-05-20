#ifndef IRRKLANG_DYNAMIC_LOADER_H
#define IRRKLANG_DYNAMIC_LOADER_H

namespace irrklang{
class ISoundEngine;
}
namespace irrdyn {
	void init();
	void close();
	irrklang::ISoundEngine* createIrrKlangDevice();
}


#endif //IRRKLANG_DYNAMIC_LOADER_H
