#include "image_manager.h"
#include "game.h"

#ifdef XDG_ENVIRONMENT
#define DATA(x) mainGame->FindDataFile(x).c_str()
#else
#define DATA(x) x
#endif

namespace ygo {

ImageManager imageManager;

bool ImageManager::Initial() {
#ifdef YGOPRO_ENVIRONMENT_PATHS
	const char *_image_path = getenv("YGOPRO_IMAGE_PATH");
	image_path = _image_path ? _image_path : "";
#endif
	tCover[0] = NULL;
	tCover[1] = NULL;
	tCover[2] = GetTextureFromFile(DATA("textures/cover.jpg"), CARD_IMG_WIDTH, CARD_IMG_HEIGHT);
	tCover[3] = GetTextureFromFile(DATA("textures/cover2.jpg"), CARD_IMG_WIDTH, CARD_IMG_HEIGHT);
	if(!tCover[3])
		tCover[3] = tCover[2];
	tUnknown = NULL;
	tUnknownFit = NULL;
	tUnknownThumb = NULL;
	tAct = driver->getTexture(DATA("textures/act.png"));
	tAttack = driver->getTexture(DATA("textures/attack.png"));
	tChain = driver->getTexture(DATA("textures/chain.png"));
	tNegated = driver->getTexture(DATA("textures/negated.png"));
	tNumber = driver->getTexture(DATA("textures/number.png"));
	tLPBar = driver->getTexture(DATA("textures/lp.png"));
	tLPFrame = driver->getTexture(DATA("textures/lpf.png"));
	tMask = driver->getTexture(DATA("textures/mask.png"));
	tEquip = driver->getTexture(DATA("textures/equip.png"));
	tTarget = driver->getTexture(DATA("textures/target.png"));
	tChainTarget = driver->getTexture(DATA("textures/chaintarget.png"));
	tLim = driver->getTexture(DATA("textures/lim.png"));
	tOT = driver->getTexture(DATA("textures/ot.png"));
	tHand[0] = driver->getTexture(DATA("textures/f1.jpg"));
	tHand[1] = driver->getTexture(DATA("textures/f2.jpg"));
	tHand[2] = driver->getTexture(DATA("textures/f3.jpg"));
	tBackGround = NULL;
	tBackGround_menu = NULL;
	tBackGround_deck = NULL;
	tField[0] = driver->getTexture(DATA("textures/field2.png"));
	tFieldTransparent[0] = driver->getTexture(DATA("textures/field-transparent2.png"));
	tField[1] = driver->getTexture(DATA("textures/field3.png"));
	tFieldTransparent[1] = driver->getTexture(DATA("textures/field-transparent3.png"));
	ResizeTexture();
	return true;
}
void ImageManager::SetDevice(irr::IrrlichtDevice* dev) {
	device = dev;
	driver = dev->getVideoDriver();
}
void ImageManager::ClearTexture() {
	for(auto tit = tMap[0].begin(); tit != tMap[0].end(); ++tit) {
		if(tit->second)
			driver->removeTexture(tit->second);
	}
	for(auto tit = tMap[1].begin(); tit != tMap[1].end(); ++tit) {
		if(tit->second)
			driver->removeTexture(tit->second);
	}
	for(auto tit = tThumb.begin(); tit != tThumb.end(); ++tit) {
		if(tit->second)
			driver->removeTexture(tit->second);
	}
	tMap[0].clear();
	tMap[1].clear();
	tThumb.clear();
	tFields.clear();
}
void ImageManager::RemoveTexture(int code) {
	auto tit = tMap[0].find(code);
	if(tit != tMap[0].end()) {
		if(tit->second)
			driver->removeTexture(tit->second);
		tMap[0].erase(tit);
	}
	tit = tMap[1].find(code);
	if(tit != tMap[1].end()) {
		if(tit->second)
			driver->removeTexture(tit->second);
		tMap[1].erase(tit);
	}
}
void ImageManager::ResizeTexture() {
	irr::s32 imgWidth = CARD_IMG_WIDTH * mainGame->xScale;
	irr::s32 imgHeight = CARD_IMG_HEIGHT * mainGame->yScale;
	irr::s32 imgWidthThumb = CARD_THUMB_WIDTH * mainGame->xScale;
	irr::s32 imgHeightThumb = CARD_THUMB_HEIGHT * mainGame->yScale;
	float mul = (mainGame->xScale > mainGame->yScale) ? mainGame->yScale : mainGame->xScale;
	irr::s32 imgWidthFit = CARD_IMG_WIDTH * mul;
	irr::s32 imgHeightFit = CARD_IMG_HEIGHT * mul;
	irr::s32 bgWidth = 1024 * mainGame->xScale;
	irr::s32 bgHeight = 640 * mainGame->yScale;
	driver->removeTexture(tCover[0]);
	driver->removeTexture(tCover[1]);
	tCover[0] = GetTextureFromFile(DATA("textures/cover.jpg"), imgWidth, imgHeight);
	tCover[1] = GetTextureFromFile(DATA("textures/cover2.jpg"), imgWidth, imgHeight);
	if(!tCover[1])
		tCover[1] = tCover[0];
	driver->removeTexture(tUnknown);
	driver->removeTexture(tUnknownFit);
	driver->removeTexture(tUnknownThumb);
	tUnknown = GetTextureFromFile(DATA("textures/unknown.jpg"), CARD_IMG_WIDTH, CARD_IMG_HEIGHT);
	tUnknownFit = GetTextureFromFile(DATA("textures/unknown.jpg"), imgWidthFit, imgHeightFit);
	tUnknownThumb = GetTextureFromFile(DATA("textures/unknown.jpg"), imgWidthThumb, imgHeightThumb);
	driver->removeTexture(tBackGround);
	tBackGround = GetTextureFromFile(DATA("textures/bg.jpg"), bgWidth, bgHeight);
	driver->removeTexture(tBackGround_menu);
	tBackGround_menu = GetTextureFromFile(DATA("textures/bg_menu.jpg"), bgWidth, bgHeight);
	if(!tBackGround_menu)
		tBackGround_menu = tBackGround;
	driver->removeTexture(tBackGround_deck);
	tBackGround_deck = GetTextureFromFile(DATA("textures/bg_deck.jpg"), bgWidth, bgHeight);
	if(!tBackGround_deck)
		tBackGround_deck = tBackGround;
}
// function by Warr1024, from https://github.com/minetest/minetest/issues/2419 , modified
void imageScaleNNAA(irr::video::IImage *src, irr::video::IImage *dest) {
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
#ifdef YGOPRO_ENVIRONMENT_PATHS
irr::video::ITexture* ImageManager::GetTextureFromImagePath(const char* file, s32 width, s32 height) {
	irr::video::ITexture* img = NULL;
	path_foreach<char>(image_path, ':',
					   [&](const std::string& prefix) {
						   std::string full_path = prefix + "/" + file;
						   if(!img && FileSystem::IsFileExists(full_path.c_str()))
							   img = GetTextureFromFile(full_path.c_str(), width, height);
					   });
	return img;
}
#endif
irr::video::ITexture* ImageManager::GetTextureFromFile(const char* file, s32 width, s32 height) {
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
irr::video::ITexture* ImageManager::GetTexture(int code, bool fit) {
	if(code == 0)
		return fit ? tUnknownFit : tUnknown;
	int width = CARD_IMG_WIDTH;
	int height = CARD_IMG_HEIGHT;
	if(fit) {
		float mul = mainGame->xScale;
		if(mainGame->xScale > mainGame->yScale)
			mul = mainGame->yScale;
		width = width * mul;
		height = height * mul;
	}
	auto tit = tMap[fit ? 1 : 0].find(code);
	if(tit == tMap[fit ? 1 : 0].end()) {
		irr::video::ITexture* img;
#ifdef YGOPRO_ENVIRONMENT_PATHS
		char file[32];
		sprintf(file, "%d.jpg", code);
		img = GetTextureFromImagePath(file, width, height);
#else
		char file[256];
		sprintf(file, "expansions/pics/%d.jpg", code);
		img = GetTextureFromFile(file, width, height);
		if(img == NULL) {
			sprintf(file, "pics/%d.jpg", code);
			img = GetTextureFromFile(file, width, height);
		}
#endif
		if(img == NULL && !mainGame->gameConf.use_image_scale) {
			tMap[fit ? 1 : 0][code] = NULL;
			return GetTextureThumb(code);
		}
		tMap[fit ? 1 : 0][code] = img;
		return (img == NULL) ? (fit ? tUnknownFit : tUnknown) : img;
	}
	if(tit->second)
		return tit->second;
	else
		return mainGame->gameConf.use_image_scale ? (fit ? tUnknownFit : tUnknown) : GetTextureThumb(code);
}
irr::video::ITexture* ImageManager::GetTextureThumb(int code) {
	if(code == 0)
		return tUnknownThumb;
	auto tit = tThumb.find(code);
	int width = CARD_THUMB_WIDTH * mainGame->xScale;
	int height = CARD_THUMB_HEIGHT * mainGame->yScale;
	if(tit == tThumb.end()) {
		irr::video::ITexture* img;
#ifdef YGOPRO_ENVIRONMENT_PATHS
		char file[64];
		sprintf(file, "thumbnail/%d.jpg", code);
		img = GetTextureFromImagePath(file, width, height);
#else
		char file[256];
		sprintf(file, "expansions/pics/thumbnail/%d.jpg", code);
		img = GetTextureFromFile(file, width, height);
		if(img == NULL) {
			sprintf(file, "pics/thumbnail/%d.jpg", code);
			img = GetTextureFromFile(file, width, height);
		}
#endif
		if(img == NULL && mainGame->gameConf.use_image_scale) {
#ifdef YGOPRO_ENVIRONMENT_PATHS
			sprintf(file, "%d.jpg", code);
			img = GetTextureFromImagePath(file, width, height);
#else
			sprintf(file, "expansions/pics/%d.jpg", code);
			img = GetTextureFromFile(file, width, height);
			if(img == NULL) {
				sprintf(file, "pics/%d.jpg", code);
				img = GetTextureFromFile(file, width, height);
			}
#endif
		}
		tThumb[code] = img;
		return (img == NULL) ? tUnknownThumb : img;
	}
	if(tit->second)
		return tit->second;
	else
		return tUnknownThumb;
}
irr::video::ITexture* ImageManager::GetTextureField(int code) {
	if(code == 0)
		return NULL;
	auto tit = tFields.find(code);
	if(tit == tFields.end()) {
		irr::video::ITexture* img;
#ifdef YGOPRO_ENVIRONMENT_PATHS
		char png_file[64];
		char jpg_file[64];
		sprintf(png_file, "field/%d.png", code);
		sprintf(jpg_file, "field/%d.jpg", code);
		img = GetTextureFromImagePath(png_file, 512 * mainGame->xScale, 512 * mainGame->yScale);
		if (img == NULL)
			img = GetTextureFromImagePath(jpg_file, 512 * mainGame->xScale, 512 * mainGame->yScale);
#else
		char file[256];
		sprintf(file, "expansions/pics/field/%d.png", code);
		img = GetTextureFromFile(file, 512 * mainGame->xScale, 512 * mainGame->yScale);
		if(img == NULL) {
			sprintf(file, "expansions/pics/field/%d.jpg", code);
			img = GetTextureFromFile(file, 512 * mainGame->xScale, 512 * mainGame->yScale);
		}
		if(img == NULL) {
			sprintf(file, "pics/field/%d.png", code);
			img = GetTextureFromFile(file, 512 * mainGame->xScale, 512 * mainGame->yScale);
		}
		if(img == NULL) {
			sprintf(file, "pics/field/%d.jpg", code);
			img = GetTextureFromFile(file, 512 * mainGame->xScale, 512 * mainGame->yScale);
		}
#endif
		tFields[code] = img;
		return img;
	}
	if(tit->second)
		return tit->second;
	else
		return NULL;
}
}

#undef DATA
