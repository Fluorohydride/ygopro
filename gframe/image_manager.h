#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include "config.h"
#include "data_manager.h"
#include <unordered_map>
#include <queue>
#include <mutex>

namespace ygo {

class ImageManager {
private:
	void resizeImage(irr::video::IImage* src, irr::video::IImage* dest, bool use_threading);
	irr::video::ITexture* addTexture(const char* name, irr::video::IImage* srcimg, irr::s32 width, irr::s32 height);
public:
	bool Initial();
	void SetDevice(irr::IrrlichtDevice* dev);
	void ClearTexture();
	void ResizeTexture();
	irr::video::ITexture* GetTextureFromFile(const char* file, irr::s32 width, irr::s32 height);
	irr::video::IImage* GetImage(int code);
	irr::video::ITexture* GetTexture(int code, irr::s32 width, irr::s32 height);
	irr::video::ITexture* GetTexture(int code, bool fit = false);
	irr::video::ITexture* GetBigPicture(int code, float zoom);
	irr::video::ITexture* GetTextureThumb(int code);
	irr::video::ITexture* GetTextureField(int code);
	static int LoadThumbThread();

	std::unordered_map<int, irr::video::ITexture*> tMap[2];
	std::unordered_map<int, irr::video::ITexture*> tThumb;
	std::unordered_map<int, irr::video::ITexture*> tFields;
	std::unordered_map<int, irr::video::IImage*> tThumbLoading;
	std::queue<int> tThumbLoadingCodes;
	std::mutex tThumbLoadingMutex;
	bool tThumbLoadingThreadRunning;
	irr::IrrlichtDevice* device;
	irr::video::IVideoDriver* driver;
	irr::video::ITexture* tCover[4];
	irr::video::ITexture* tUnknown;
	irr::video::ITexture* tUnknownFit;
	irr::video::ITexture* tUnknownThumb;
	irr::video::ITexture* tBigPicture;
	irr::video::ITexture* tLoading;
	irr::video::ITexture* tAct;
	irr::video::ITexture* tAttack;
	irr::video::ITexture* tNegated;
	irr::video::ITexture* tChain;
	irr::video::ITexture* tNumber;
	irr::video::ITexture* tLPFrame;
	irr::video::ITexture* tLPBar;
	irr::video::ITexture* tMask;
	irr::video::ITexture* tEquip;
	irr::video::ITexture* tTarget;
	irr::video::ITexture* tChainTarget;
	irr::video::ITexture* tLim;
	irr::video::ITexture* tLimCredit;
	irr::video::ITexture* tOT;
	irr::video::ITexture* tHand[3];
	irr::video::ITexture* tBackGround;
	irr::video::ITexture* tBackGround_menu;
	irr::video::ITexture* tBackGround_deck;
	irr::video::ITexture* tField[2];
	irr::video::ITexture* tFieldTransparent[2];
};

extern ImageManager imageManager;

}

#endif // IMAGEMANAGER_H
