#include "image_manager.h"
#include "game.h"
#ifndef _WIN32
#include <dirent.h>
#endif

namespace ygo {

ImageManager imageManager;

bool ImageManager::Initial() {
	RefreshRandomImageList();

	tCover[0] = GetRandomImage(TEXTURE_COVER_S);
	if(!tCover[0])
		tCover[0] = driver->getTexture("textures/cover.jpg");
	tCover[1] = GetRandomImage(TEXTURE_COVER_O);
	if(!tCover[1])
		tCover[1] = driver->getTexture("textures/cover2.jpg");
	if(!tCover[1])
		tCover[1] = GetRandomImage(TEXTURE_COVER_S);
	if(!tCover[1])
		tCover[1] = tCover[0];
	tUnknown = driver->getTexture("textures/unknown.jpg");
	tAct = GetRandomImage(TEXTURE_ACTIVATE);
	tAttack = GetRandomImage(TEXTURE_ATTACK);
	if(!tAct)
		tAct = driver->getTexture("textures/act.png");
	if(!tAttack)
		tAttack = driver->getTexture("textures/attack.png");
	tChain = driver->getTexture("textures/chain.png");
	tNegated = driver->getTexture("textures/negated.png");
	tNumber = driver->getTexture("textures/number.png");
	tLPBar = driver->getTexture("textures/lp.png");
	tLPFrame = driver->getTexture("textures/lpf.png");
	tMask = driver->getTexture("textures/mask.png");
	tEquip = driver->getTexture("textures/equip.png");
	tTarget = driver->getTexture("textures/target.png");
	tChainTarget = driver->getTexture("textures/chaintarget.png");
	tLim = driver->getTexture("textures/lim.png");
	tOT = driver->getTexture("textures/ot.png");
	tHand[0] = driver->getTexture("textures/f1.jpg");
	tHand[1] = driver->getTexture("textures/f2.jpg");
	tHand[2] = driver->getTexture("textures/f3.jpg");
	tBackGround = GetRandomImage(TEXTURE_DUEL);
	if(!tBackGround)
		tBackGround = driver->getTexture("textures/bg.jpg");
	if(!tBackGround)
		tBackGround = driver->getTexture("textures/bg_duel.jpg");
	tBackGround_menu = GetRandomImage(TEXTURE_MENU);
	if(!tBackGround_menu)
		tBackGround_menu = driver->getTexture("textures/bg_menu.jpg");
	if(!tBackGround_menu)
		tBackGround_menu = GetRandomImage(TEXTURE_DUEL);
	if(!tBackGround_menu)
		tBackGround_menu = tBackGround;
	tBackGround_deck = GetRandomImage(TEXTURE_DECK);
	if(!tBackGround_deck)
		tBackGround_deck = driver->getTexture("textures/bg_deck.jpg");
	if(!tBackGround_deck)
		tBackGround_deck = GetRandomImage(TEXTURE_DUEL);
	if(!tBackGround_deck)
		tBackGround_deck = tBackGround;	
	tField[0] = driver->getTexture("textures/field2.png");
	tFieldTransparent[0] = driver->getTexture("textures/field-transparent2.png");
	tField[1] = driver->getTexture("textures/field3.png");
	tFieldTransparent[1] = driver->getTexture("textures/field-transparent3.png");
	return true;
}
irr::video::ITexture* ImageManager::GetRandomImage(int image_type) {
	int count = ImageList[image_type].size();
	if(count <= 0)
		return 0;
	char ImageName[1024];
	wchar_t fname[1024];
	int image_id = rand() % count;
	auto name = ImageList[image_type][image_id].c_str();
	myswprintf(fname, L"./textures/%ls", name);
	BufferIO::EncodeUTF8(fname, ImageName);
	return driver->getTexture(ImageName);
}
void ImageManager::RefreshRandomImageList() {
	RefreshImageDir(L"bg/", TEXTURE_DUEL);
	RefreshImageDir(L"bg_duel/", TEXTURE_DUEL);
	RefreshImageDir(L"bg_deck/", TEXTURE_DECK);
	RefreshImageDir(L"bg_menu/", TEXTURE_MENU);
	RefreshImageDir(L"cover/", TEXTURE_COVER_S);
	RefreshImageDir(L"cover2/", TEXTURE_COVER_O);
	RefreshImageDir(L"attack/", TEXTURE_ATTACK);
	RefreshImageDir(L"act/", TEXTURE_ACTIVATE);
}
void ImageManager::RefreshImageDir(std::wstring path, int image_type) {
#ifdef _WIN32
	WIN32_FIND_DATAW fdataw;
	std::wstring search = L"./textures/" + path + L"*.*";
	HANDLE fh = FindFirstFileW(search.c_str(), &fdataw);
	if(fh == INVALID_HANDLE_VALUE)
		return;
	do {
		size_t len = wcslen(fdataw.cFileName);
		if((fdataw.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) || len < 5
			|| !(_wcsicmp(fdataw.cFileName + len - 4, L".jpg") == 0 || _wcsicmp(fdataw.cFileName + len - 4, L".png") == 0))
			continue;
		std::wstring filename = path + (std::wstring)fdataw.cFileName;
		ImageList[image_type].push_back(filename);
	} while(FindNextFileW(fh, &fdataw));
	FindClose(fh);
#else
	DIR * dir;
	struct dirent * dirp;
	std::wstring wsearchpath = L"./textures/" + path;
	char searchpath[256];
	BufferIO::EncodeUTF8(wsearchpath.c_str(), searchpath);
	if((dir = opendir(searchpath)) == NULL)
		return;
	while((dirp = readdir(dir)) != NULL) {
		size_t len = strlen(dirp->d_name);
		if(len < 5 || !(strcasecmp(dirp->d_name + len - 4, ".jpg") == 0 || strcasecmp(dirp->d_name + len - 4, ".png")))
			continue;
		wchar_t wname[256];
		BufferIO::DecodeUTF8(dirp->d_name, wname);
		std::wstring filename = path + (std::wstring)wname;
		ImageList[image_type].push_back(filename);
	}
	closedir(dir);
#endif
}
void ImageManager::SetDevice(irr::IrrlichtDevice* dev) {
	device = dev;
	driver = dev->getVideoDriver();
}
void ImageManager::ClearTexture() {
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
// function by Warr1024, from https://github.com/minetest/minetest/issues/2419 , modified
void imageScaleNNAA(irr::video::IImage *src, irr::video::IImage *dest)
{
	double sx, sy, minsx, maxsx, minsy, maxsy, area, ra, ga, ba, aa, pw, ph, pa;
	u32 dy, dx;
	irr::video::SColor pxl;

	// Cache rectsngle boundaries.
	double sw = src->getDimension().Width * 1.0;
	double sh = src->getDimension().Height * 1.0;

	// Walk each destination image pixel.
	// Note: loop y around x for better cache locality.
	irr::core::dimension2d<u32> dim = dest->getDimension();
	for(dy = 0; dy < dim.Height; dy++)
		for(dx = 0; dx < dim.Width; dx++) {

			// Calculate floating-point source rectangle bounds.
			minsx = dx * sw / dim.Width;
			maxsx = minsx + sw / dim.Width;
			minsy = dy * sh / dim.Height;
			maxsy = minsy + sh / dim.Height;

			// Total area, and integral of r, g, b values over that area,
			// initialized to zero, to be summed up in next loops.
			area = 0;
			ra = 0;
			ga = 0;
			ba = 0;
			aa = 0;

			// Loop over the integral pixel positions described by those bounds.
			for(sy = floor(minsy); sy < maxsy; sy++)
				for(sx = floor(minsx); sx < maxsx; sx++) {

					// Calculate width, height, then area of dest pixel
					// that's covered by this source pixel.
					pw = 1;
					if(minsx > sx)
						pw += sx - minsx;
					if(maxsx < (sx + 1))
						pw += maxsx - sx - 1;
					ph = 1;
					if(minsy > sy)
						ph += sy - minsy;
					if(maxsy < (sy + 1))
						ph += maxsy - sy - 1;
					pa = pw * ph;

					// Get source pixel and add it to totals, weighted
					// by covered area and alpha.
					pxl = src->getPixel((u32)sx, (u32)sy);
					area += pa;
					ra += pa * pxl.getRed();
					ga += pa * pxl.getGreen();
					ba += pa * pxl.getBlue();
					aa += pa * pxl.getAlpha();
				}

			// Set the destination image pixel to the average color.
			if(area > 0) {
				pxl.setRed(ra / area + 0.5);
				pxl.setGreen(ga / area + 0.5);
				pxl.setBlue(ba / area + 0.5);
				pxl.setAlpha(aa / area + 0.5);
			} else {
				pxl.setRed(0);
				pxl.setGreen(0);
				pxl.setBlue(0);
				pxl.setAlpha(0);
			}
			dest->setPixel(dx, dy, pxl);
		}
}
irr::video::ITexture* ImageManager::GetTextureFromFile(char* file, s32 width, s32 height) {
	if(mainGame->gameConf.use_image_scale) {
		irr::video::ITexture* texture;
		irr::video::IImage* srcimg = driver->createImageFromFile(file);
		if(srcimg == NULL)
			return NULL;
		if(srcimg->getDimension() == irr::core::dimension2d<u32>(width, height)) {
			texture = driver->addTexture(file, srcimg);
		} else {
			video::IImage *destimg = driver->createImage(srcimg->getColorFormat(), irr::core::dimension2d<u32>(width, height));
			imageScaleNNAA(srcimg, destimg);
			texture = driver->addTexture(file, destimg);
			destimg->drop();
		}
		srcimg->drop();
		return texture;
	} else {
		return driver->getTexture(file);
	}
}
irr::video::ITexture* ImageManager::GetTexture(int code) {
	if(code == 0)
		return tUnknown;
	auto tit = tMap.find(code);
	if(tit == tMap.end()) {
		char file[256];
		sprintf(file, "expansions/pics/%d.jpg", code);
		irr::video::ITexture* img = GetTextureFromFile(file, CARD_IMG_WIDTH, CARD_IMG_HEIGHT);
		if(img == NULL) {
			sprintf(file, "pics/%d.jpg", code);
			img = GetTextureFromFile(file, CARD_IMG_WIDTH, CARD_IMG_HEIGHT);
		}
		if(img == NULL && !mainGame->gameConf.use_image_scale) {
			tMap[code] = NULL;
			return GetTextureThumb(code);
		}
		tMap[code] = img;
		return (img == NULL) ? tUnknown : img;
	}
	if(tit->second)
		return tit->second;
	else
		return mainGame->gameConf.use_image_scale ? tUnknown : GetTextureThumb(code);
}
irr::video::ITexture* ImageManager::GetTextureThumb(int code) {
	if(code == 0)
		return tUnknown;
	auto tit = tThumb.find(code);
	if(tit == tThumb.end()) {
		char file[256];
		sprintf(file, "expansions/pics/thumbnail/%d.jpg", code);
		irr::video::ITexture* img = GetTextureFromFile(file, CARD_THUMB_WIDTH, CARD_THUMB_HEIGHT);
		if(img == NULL) {
			sprintf(file, "pics/thumbnail/%d.jpg", code);
			img = GetTextureFromFile(file, CARD_THUMB_WIDTH, CARD_THUMB_HEIGHT);
		}
		if(img == NULL && mainGame->gameConf.use_image_scale) {
			sprintf(file, "expansions/pics/%d.jpg", code);
			img = GetTextureFromFile(file, CARD_THUMB_WIDTH, CARD_THUMB_HEIGHT);
			if(img == NULL) {
				sprintf(file, "pics/%d.jpg", code);
				img = GetTextureFromFile(file, CARD_THUMB_WIDTH, CARD_THUMB_HEIGHT);
			}
		}
		tThumb[code] = img;
		return (img == NULL) ? tUnknown : img;
	}
	if(tit->second)
		return tit->second;
	else
		return tUnknown;
}
irr::video::ITexture* ImageManager::GetTextureField(int code) {
	if(code == 0)
		return NULL;
	auto tit = tFields.find(code);
	if(tit == tFields.end()) {
		char file[256];
		sprintf(file, "expansions/pics/field/%d.png", code);
		irr::video::ITexture* img = GetTextureFromFile(file, 512, 512);
		if(img == NULL) {
			sprintf(file, "expansions/pics/field/%d.jpg", code);
			img = GetTextureFromFile(file, 512, 512);
		}
		if(img == NULL) {
			sprintf(file, "pics/field/%d.png", code);
			img = GetTextureFromFile(file, 512, 512);
		}
		if(img == NULL) {
			sprintf(file, "pics/field/%d.jpg", code);
			img = GetTextureFromFile(file, 512, 512);
			if(img == NULL) {
				tFields[code] = NULL;
				return NULL;
			} else {
				tFields[code] = img;
				return img;
			}
		} else {
			tFields[code] = img;
			return img;
		}
	}
	if(tit->second)
		return tit->second;
	else
		return NULL;
}
}
