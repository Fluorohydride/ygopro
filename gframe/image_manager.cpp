#include "image_manager.h"
#include "game.h"

namespace ygo {

bool ImageManager::Initial() {
	tCover = driver->getTexture("textures/cover.jpg");
	tAct = driver->getTexture("textures/act.png");
	tAttack = driver->getTexture("textures/attack.png");
	tChain = driver->getTexture("textures/chain.png");
	tNegated = driver->getTexture("textures/negated.png");
	tNumber = driver->getTexture("textures/number.png");
	tLPBar = driver->getTexture("textures/lp.png");
	tLPFrame = driver->getTexture("textures/lpf.png");
	tMask = driver->getTexture("textures/mask.png");
	tEquip = driver->getTexture("textures/equip.png");
	tTarget = driver->getTexture("textures/target.png");
	tLim = driver->getTexture("textures/lim.png");
	tBackGround = driver->getTexture("textures/bg.jpg");
	return true;
}
void ImageManager::SetDevice(irr::IrrlichtDevice* dev) {
	device = dev;
	driver = dev->getVideoDriver();
}
void ImageManager::ClearTexture() {
	mainGame->matManager.mCard.setTexture(0, 0);
	mainGame->imgCard->setImage(0);
	mainGame->btnPSAU->setImage();
	mainGame->btnPSDU->setImage();
	mainGame->btnCardSelect[0]->setImage();
	mainGame->btnCardSelect[1]->setImage();
	mainGame->btnCardSelect[2]->setImage();
	mainGame->btnCardSelect[3]->setImage();
	mainGame->btnCardSelect[4]->setImage();
	for(auto tit = tMap.begin(); tit != tMap.end(); ++tit) {
		if(tit->second)
			driver->removeTexture(tit->second);
	}
	for(auto tit = tThumb.begin(); tit != tThumb.end(); ++tit) {
		if(tit->second)
			driver->removeTexture(tit->second);
	}
	tMap.clear();
	tThumb.clear();
}
void ImageManager::RemoveTexture(int code) {
	auto tit = tMap.find(code);
	if(tit != tMap.end()) {
		if(tit->second)
			driver->removeTexture(tit->second);
		tMap.erase(tit);
	}
}
irr::video::ITexture* ImageManager::GetTexture(int code) {
	if(code == 0)
		return 0;
	auto tit = tMap.find(code);
	if(tit == tMap.end()) {
		char file[256];
		sprintf(file, "pics/%d.jpg", code);
		irr::video::ITexture* img = driver->getTexture(file);
		tMap[code] = img;
		return img;
	}
	return tit->second;
}
irr::video::ITexture* ImageManager::GetTextureThumb(int code) {
	if(code == 0)
		return 0;
	auto tit = tThumb.find(code);
	if(tit == tThumb.end()) {
		char file[32];
		sprintf(file, "pics/thumbnail/%d.jpg", code);
		irr::video::ITexture* img = driver->getTexture(file);
		tThumb[code] = img;
		return img;
	}
	return tit->second;
}
}
