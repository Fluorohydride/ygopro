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
	cur_pic_res_width[0] = CARD_IMG_WIDTH;
	cur_pic_res_width[1] = CARD_IMG_WIDTH;
	cur_pic_res_width[2] = CARD_THUMB_WIDTH;
	cur_pic_res_height[0] = CARD_IMG_HEIGHT;
	cur_pic_res_height[1] = CARD_IMG_HEIGHT;
	cur_pic_res_height[2] = CARD_THUMB_HEIGHT;
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
	if(resize) {
		cur_pic_res_width[0] = CARD_IMG_WIDTH;
		cur_pic_res_width[1] = CARD_IMG_WIDTH * mainGame->window_size.Width / 1024;
		cur_pic_res_width[2] = CARD_THUMB_WIDTH * mainGame->window_size.Width / 1024;
		cur_pic_res_height[0] = CARD_IMG_HEIGHT;
		cur_pic_res_height[1] = CARD_IMG_HEIGHT * mainGame->window_size.Height / 640;
		cur_pic_res_height[2] = CARD_THUMB_HEIGHT * mainGame->window_size.Height / 640;
	}
	ClearCachedTextures(resize);
	if(!resize)
		f(tMap[0]);
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
void ImageManager::RefreshCachedTextures() {
	for(auto it = loading_pics[0].begin(); it != loading_pics[0].end();) {
		if(it->second.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
			auto pair = it->second.get();
			if(pair.first) {
				tMap[0][it->first] = driver->addTexture(pair.second, pair.first);
				pair.first->drop();
			} else
				tMap[0][it->first] = nullptr;
			it = loading_pics[0].erase(it);
			continue;
		}
		it++;
	}
	for(auto it = loading_pics[1].begin(); it != loading_pics[1].end();) {
		if(it->second.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
			auto pair = it->second.get();
			if(pair.first) {
				tMap[1][it->first] = driver->addTexture(pair.second, pair.first);
				pair.first->drop();
			} else
				tMap[1][it->first] = nullptr;
			it = loading_pics[1].erase(it);
			continue;
		}
		it++;
	}
	for(auto it = loading_pics[2].begin(); it != loading_pics[2].end();) {
		if(it->second.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
			auto pair = it->second.get();
			if(pair.first) {
				tThumb[it->first] = driver->addTexture(pair.second, pair.first);
				pair.first->drop();
			} else
				tThumb[it->first] = nullptr;
			it = loading_pics[2].erase(it);
			continue;
		}
		it++;
	}
}
void ImageManager::ClearCachedTextures(bool resize) {
	if(!resize) {
		cur_pic_res_width[0]--;
		for(auto it = loading_pics[0].begin(); it != loading_pics[0].end();) {
			auto pair = it->second.get();
			if(pair.first)
				pair.first->drop();
			it = loading_pics[0].erase(it);
		}
		cur_pic_res_width[0]++;
	}
	cur_pic_res_width[1]++;
	cur_pic_res_width[2]++;
	for(auto it = loading_pics[1].begin(); it != loading_pics[1].end();) {
		auto pair = it->second.get();
		if(pair.first)
			pair.first->drop();
		it = loading_pics[1].erase(it);
	}
	for(auto it = loading_pics[2].begin(); it != loading_pics[2].end();) {
		auto pair = it->second.get();
		if(pair.first)
			pair.first->drop();
		it = loading_pics[2].erase(it);
	}
	cur_pic_res_width[1]--;
	cur_pic_res_width[2]--;
}
// function by Warr1024, from https://github.com/minetest/minetest/issues/2419 , modified
bool imageScaleNNAA(irr::video::IImage *src, irr::video::IImage *dest, s32 width, s32 height, std::atomic_uint& source_width, std::atomic_uint& source_height) {
	double sx, sy, minsx, maxsx, minsy, maxsy, area, ra, ga, ba, aa, pw, ph, pa;
	u32 dy, dx;
	irr::video::SColor pxl;

	// Cache rectsngle boundaries.
	double sw = src->getDimension().Width * 1.0;
	double sh = src->getDimension().Height * 1.0;

	// Walk each destination image pixel.
	// Note: loop y around x for better cache locality.
	irr::core::dimension2d<u32> dim = dest->getDimension();
	if(width != source_width || height != source_height)
		return false;
	for(dy = 0; dy < dim.Height; dy++)
		for(dx = 0; dx < dim.Width; dx++) {
			if(width != source_width || height != source_height)
				return false;

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
					if(width != source_width || height != source_height)
						return false;

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
	return true;
}
irr::video::IImage* ImageManager::GetTextureFromFile(const char* file, s32 width, s32 height, std::atomic_uint& source_width, std::atomic_uint& source_height) {
	irr::video::IImage* srcimg = driver->createImageFromFile(file);
	if(srcimg == NULL || width != source_width || height != source_height)
		return NULL;
	if(srcimg->getDimension() == irr::core::dimension2d<u32>(width, height)) {
		return srcimg;
	} else {
		video::IImage *destimg = driver->createImage(srcimg->getColorFormat(), irr::core::dimension2d<u32>(width, height));
		if(width != source_width || height != source_height || !imageScaleNNAA(srcimg, destimg, width, height, std::ref(source_width), std::ref(source_height))) {
			destimg->drop();
			destimg = nullptr;
		}
		srcimg->drop();
		return destimg;
	}
}
std::pair<IImage*, const char*> ImageManager::LoadCardTexture(int code, int width, int height, std::atomic_uint& source_width, std::atomic_uint& source_height) {
	irr::video::IImage* img = nullptr;
	for(auto& path : mainGame->resource_dirs) {
		for(auto& extension : { ".png", ".jpg" }) {
			if(width != source_width || height != source_height)
				return std::make_pair(nullptr, "fail");
			auto file = path + std::to_string(code) + extension;
			if(img = GetTextureFromFile(file.c_str(), width, height, std::ref(source_width), std::ref(source_height))) {
				if(width != source_width || height != source_height)
					return std::make_pair(nullptr, "fail");
				return std::make_pair(img, file.c_str());
			}
		}
	}
	return std::make_pair(img, "");
}
irr::video::ITexture* ImageManager::GetTexture(int code, bool wait, bool fit, int* chk) {
	if(chk)
		*chk = 1;
	if(code == 0)
		return tUnknown;
	auto& map = tMap[fit ? 1 : 0];
	auto tit = map.find(code);
	if(tit == map.end()) {
		auto a = loading_pics[fit ? 1 : 0].find(code);
		if(chk)
			*chk = 2;
		if(a == loading_pics[fit ? 1 : 0].end()) {
			int width = CARD_IMG_WIDTH;
			int height = CARD_IMG_HEIGHT;
			if(fit) {
				width = width * mainGame->window_size.Width / 1024;
				height = height * mainGame->window_size.Height / 640;
			}
			/*if(wait) {
				auto img = LoadCardTexture(code, width, height);
				if(img.first) {
					map[code] = driver->addTexture(img.second, img.first);
					img.first->drop();
					if(chk && !map[code])
						*chk = 0;
					return (map[code]) ? map[code] : tUnknown;
				} else
					map[code] = nullptr;
			} else {*/
				loading_pics[fit ? 1 : 0][code] = std::async(std::launch::async, &ImageManager::LoadCardTexture, this, code, width, height, std::ref(cur_pic_res_width[fit ? 1 : 0]), std::ref(cur_pic_res_height[fit ? 1 : 0]));
			//}
		}
		return tUnknown;
	}
	if(chk && !tit->second)
		*chk = 0;
	return (tit->second) ? tit->second : tUnknown;
}
irr::video::ITexture* ImageManager::GetTextureThumb(int code, bool wait, int* chk) {
	if(chk)
		*chk = 1;
	if(code == 0)
		return tUnknown;
	auto tit = tThumb.find(code);
	if(tit == tThumb.end()) {
		auto a = loading_pics[2].find(code);
		if(chk)
			*chk = 2;
		if(a == loading_pics[2].end()) {
			int width = CARD_THUMB_WIDTH * mainGame->window_size.Width / 1024;
			int height = CARD_THUMB_HEIGHT * mainGame->window_size.Height / 640;
			//if(wait) {
				/*auto img = LoadCardTexture(code, width, height);
				if(img.first) {
					tThumb[code] = driver->addTexture(img.second, img.first);
					img.first->drop();
					if(chk && !tThumb[code])
						*chk = 0;
					return (tThumb[code]) ? tThumb[code] : tUnknown;
				} else
					tThumb[code] = nullptr;
			} else {*/
				loading_pics[2][code] = std::async(std::launch::async, &ImageManager::LoadCardTexture, this, code, width, height, std::ref(cur_pic_res_width[2]), std::ref(cur_pic_res_height[2]));
			//}
		}
		return tUnknown;
	}
	if(chk && !tit->second)
		*chk = 0;
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
