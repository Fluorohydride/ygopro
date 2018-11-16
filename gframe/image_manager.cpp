#include "image_manager.h"
#include "game.h"

namespace ygo {

ImageManager imageManager;

bool ImageManager::Initial() {
	tCover[0] = driver->getTexture("textures/cover.jpg");
	tCover[1] = driver->getTexture("textures/cover2.jpg");
	if(!tCover[1])
		tCover[1] = tCover[0];
	tUnknown = driver->getTexture("textures/unknown.jpg");
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
	tChainTarget = driver->getTexture("textures/chaintarget.png");
	tLim = driver->getTexture("textures/lim.png");
	tHand[0] = driver->getTexture("textures/f1.jpg");
	tHand[1] = driver->getTexture("textures/f2.jpg");
	tHand[2] = driver->getTexture("textures/f3.jpg");
	tBackGround = driver->getTexture("textures/bg.jpg");
	tBackGround_menu = driver->getTexture("textures/bg_menu.jpg");
	if(!tBackGround_menu)
		tBackGround_menu = tBackGround;
	tBackGround_deck = driver->getTexture("textures/bg_deck.jpg");
	if(!tBackGround_deck)
		tBackGround_deck = tBackGround;
	tField[0][0] = driver->getTexture("textures/field2.png");
	tFieldTransparent[0][0] = driver->getTexture("textures/field-transparent2.png");
	tField[0][1] = driver->getTexture("textures/field3.png");
	tFieldTransparent[0][1] = driver->getTexture("textures/field-transparent3.png");
	tField[0][2] = driver->getTexture("textures/field.png");
	tFieldTransparent[0][2] = driver->getTexture("textures/field-transparent.png");
	tField[0][3] = driver->getTexture("textures/field4.png");
	tFieldTransparent[0][3] = driver->getTexture("textures/field-transparent4.png");
	tField[1][0] = driver->getTexture("textures/fieldSP2.png");
	tFieldTransparent[1][0] = driver->getTexture("textures/field-transparentSP2.png");
	tField[1][1] = driver->getTexture("textures/fieldSP3.png");
	tFieldTransparent[1][1] = driver->getTexture("textures/field-transparentSP3.png");
	tField[1][2] = driver->getTexture("textures/fieldSP.png");
	tFieldTransparent[1][2] = driver->getTexture("textures/field-transparentSP.png");
	tField[1][3] = driver->getTexture("textures/fieldSP4.png");
	tFieldTransparent[1][3] = driver->getTexture("textures/field-transparentSP4.png");
	return true;
}
void ImageManager::SetDevice(irr::IrrlichtDevice* dev) {
	device = dev;
	driver = dev->getVideoDriver();
}
void ImageManager::ClearTexture(bool resize) {
	auto f = [&](std::unordered_map<int, irr::video::ITexture*> &map) {
		for(auto tit = map.begin(); tit != map.end(); ++tit) {
			if(tit->second)
				driver->removeTexture(tit->second);
		}
		map.clear();
	};
	if(!resize) {
		f(tMap[0]);
	}
	f(tMap[1]);
	f(tThumb);
	f(tFields);
}
void ImageManager::RemoveTexture(int code) {
	for(auto map : { &tMap[0], &tMap[1] }) {
		auto tit = map->find(code);
		if(tit != map->end()) {
			if(tit->second)
				driver->removeTexture(tit->second);
			map->erase(tit);
		}
	}
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
irr::video::ITexture* ImageManager::GetTextureFromFile(const char* file, s32 width, s32 height) {
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
}
irr::video::ITexture* ImageManager::LoadCardTexture(int code, int width, int height) {
	irr::video::ITexture* img = nullptr;
	for(auto& path : mainGame->resource_dirs) {
		for(auto& extension : { ".png", ".jpg" }) {
			if(img = GetTextureFromFile((path + std::to_string(code) + extension).c_str(), width, height))
				return img;
		}
	}
	return nullptr;
}
irr::video::ITexture* ImageManager::GetTexture(int code, bool fit) {
	if(code == 0)
		return tUnknown;
	auto& map = tMap[fit ? 1 : 0];
	auto tit = map.find(code);
	if(tit == map.end()) {
		int width = CARD_IMG_WIDTH;
		int height = CARD_IMG_HEIGHT;
		if(fit) {
			width = width * mainGame->window_size.Width / 1024;
			height = height * mainGame->window_size.Height / 640;
		}
		map[code] = LoadCardTexture(code, width, height);
		return (!map[code]) ? tUnknown : map[code];
	}
	return (tit->second) ? tit->second : tUnknown;
}
irr::video::ITexture* ImageManager::GetTextureThumb(int code) {
	if(code == 0)
		return tUnknown;
	auto tit = tThumb.find(code);
	if(tit == tThumb.end()) {
		int width = CARD_THUMB_WIDTH * mainGame->window_size.Width / 1024;
		int height = CARD_THUMB_HEIGHT * mainGame->window_size.Height / 640;
		tThumb[code] = LoadCardTexture(code, width, height);
		return (!tThumb[code]) ? tUnknown : tThumb[code];
	}
	return (tit->second) ? tit->second : tUnknown;
}
irr::video::ITexture* ImageManager::GetTextureField(int code) {
	if(code == 0)
		return nullptr;
	auto tit = tFields.find(code);
	if(tit == tFields.end()) {
		irr::video::ITexture* img = nullptr;
		for(auto& path : mainGame->resource_dirs) {
			for(auto& extension : { ".png", ".jpg" }) {
				if(img = driver->getTexture((path + "field/" + std::to_string(code) + extension).c_str()))
					return img;
			}
		}
		tFields[code] = img;
		return img;
	}
	return (tit->second) ? tit->second : nullptr;
}
}
