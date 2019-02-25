#include "image_manager.h"
#include "game.h"
#include <curl/curl.h>

namespace ygo {

ImageManager imageManager;

void ImageManager::AddDownloadResource(PicSource src) {
	pic_urls.push_back(src);
}

bool ImageManager::Initial() {
	timestamp_id[0] = timestamp_id[1] = timestamp_id[2] = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
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
void ImageManager::ClearTexture() {
	auto f = [&](std::unordered_map<int, irr::video::ITexture*> &map) {
		for(auto tit = map.begin(); tit != map.end(); ++tit) {
			if(tit->second)
				driver->removeTexture(tit->second);
		}
		map.clear();
	};
	ClearCachedTextures();
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
	for(auto it = loading_pics[0]->begin(); it != loading_pics[0]->end();) {
		if(it->second.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
			auto pair = it->second.get();
			if(pair.first) {
				tMap[0][it->first] = driver->addTexture(pair.second.c_str(), pair.first);
				pair.first->drop();
			} else
				tMap[0][it->first] = nullptr;
			it = loading_pics[0]->erase(it);
			continue;
		}
		it++;
	}
	for(auto it = loading_pics[1]->begin(); it != loading_pics[1]->end();) {
		if(it->second.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
			auto pair = it->second.get();
			if(pair.first) {
				tMap[1][it->first] = driver->addTexture(pair.second.c_str(), pair.first);
				pair.first->drop();
			} else
				tMap[1][it->first] = nullptr;
			it = loading_pics[1]->erase(it);
			continue;
		}
		it++;
	}
	for(auto it = loading_pics[2]->begin(); it != loading_pics[2]->end();) {
		if(it->second.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
			auto pair = it->second.get();
			if(pair.first) {
				tThumb[it->first] = driver->addTexture(pair.second.c_str(), pair.first);
				pair.first->drop();
			} else
				tThumb[it->first] = nullptr;
			it = loading_pics[2]->erase(it);
			continue;
		}
		it++;
	}
}
void ImageManager::ClearFutureObjects(loading_map* map) {
	for(auto it = map->begin(); it != map->end();) {
		auto pair = it->second.get();
		if(pair.first)
			pair.first->drop();
		it = map->erase(it);
	}
	delete map;
}
size_t write_data(char *ptr, size_t size, size_t nmemb, void *userdata) {
	std::ofstream *out = static_cast<std::ofstream *>(userdata);
	size_t nbytes = size * nmemb;
	out->write(ptr, nbytes);
	return nbytes;
}
bool IsValidImage(const std::string& file, std::string& extension) {
	extension.clear();
	std::ifstream pic(file, std::ifstream::binary);
	unsigned char pngheader[] = { 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a }; //png header
	unsigned char jpgheader[] = { 0xff, 0xd8, 0xff }; //jpg header
	unsigned char header[8];
	pic.read((char*)header, 8);
	if(!memcmp(pngheader, header, sizeof(pngheader))) {
		extension = ".png";
	} else if(!memcmp(jpgheader, header, sizeof(jpgheader))) {
		extension = ".jpg";
	} else
		return false;
	return true;
}
void ImageManager::DownloadPic(int code) {
	auto id = std::to_string(code);
	std::string dest_folder = "./pics/" + id;
	auto name = "./pics/temp/" + id;
	std::string ext;
	pic_download.lock();
	if(downloading_pics.find(code) == downloading_pics.end()) {
		downloading_pics[code].first = false;
		pic_download.unlock();
		for(auto& src : pic_urls) {
			if(src.type == "field")
				continue;
			CURL *curl = NULL;
			std::ofstream fp(name, std::ofstream::binary);
			CURLcode res;
			curl = curl_easy_init();
			if(curl) {
				curl_easy_setopt(curl, CURLOPT_URL, fmt::format(src.url, code).c_str());
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, &fp);
				res = curl_easy_perform(curl);
				curl_easy_cleanup(curl);
				fp.close();
				if(res == CURLE_OK && IsValidImage(name, ext)) {
					Utils::Movefile(name, dest_folder + ext);
					break;
				} else {
					Utils::Deletefile(name);
				}
			}
		}
	} else {
		pic_download.unlock();
		return;
	}
	pic_download.lock();
	downloading_pics[code].first = true;
	downloading_pics[code].second = dest_folder + ext;
	pic_download.unlock();
}
void ImageManager::DownloadField(int code) {
	auto id = std::to_string(code);
	std::string dest_folder = "./pics/field/" + id;
	auto name = "./pics/temp/" + id + "_f";
	std::string ext;
	field_download.lock();
	if(downloading_fields.find(code) == downloading_fields.end()) {
		downloading_fields[code].first = false;
		field_download.unlock();
		for(auto& src : pic_urls) {
			if(src.type == "pic")
				continue;
			CURL *curl = NULL;
			std::ofstream fp(name, std::ofstream::binary);
			CURLcode res;
			curl = curl_easy_init();
			if(curl) {
				curl_easy_setopt(curl, CURLOPT_URL, fmt::format(src.url, code).c_str());
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, &fp);
				res = curl_easy_perform(curl);
				curl_easy_cleanup(curl);
				fp.close();
				if(res == CURLE_OK && IsValidImage(name, ext)) {
					Utils::Movefile(name, dest_folder + ext);
					break;
				} else {
					Utils::Deletefile(name);
				}
			}
		}
	} else {
		field_download.unlock();
		return;
	}
	field_download.lock();
	downloading_fields[code].first = true;
	downloading_fields[code].second = dest_folder + ext;
	field_download.unlock();
}
void ImageManager::ClearCachedTextures() {
	timestamp_id[0] = timestamp_id[1] = timestamp_id[2] = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	if(loading_pics[0]->size()) {
		std::thread(&ImageManager::ClearFutureObjects, this, loading_pics[0]).detach();
		loading_pics[0] = new loading_map();
	}
	if(loading_pics[1]->size()) {
		std::thread(&ImageManager::ClearFutureObjects, this, loading_pics[1]).detach();
		loading_pics[1] = new loading_map();
	}
	if(loading_pics[2]->size()) {
		std::thread(&ImageManager::ClearFutureObjects, this, loading_pics[2]).detach();
		loading_pics[2] = new loading_map();
	}
}
// function by Warr1024, from https://github.com/minetest/minetest/issues/2419 , modified
bool ImageManager::imageScaleNNAA(irr::video::IImage *src, irr::video::IImage *dest, s32 width, s32 height, chrono_time timestamp_id, std::atomic<chrono_time>& source_timestamp_id) {
	double sx, sy, minsx, maxsx, minsy, maxsy, area, ra, ga, ba, aa, pw, ph, pa;
	u32 dy, dx;
	irr::video::SColor pxl;

	// Cache rectsngle boundaries.
	double sw = src->getDimension().Width * 1.0;
	double sh = src->getDimension().Height * 1.0;

	// Walk each destination image pixel.
	// Note: loop y around x for better cache locality.
	irr::core::dimension2d<u32> dim = dest->getDimension();
	if(timestamp_id != source_timestamp_id.load())
		return false;
	for(dy = 0; dy < dim.Height; dy++)
		for(dx = 0; dx < dim.Width; dx++) {
			if(timestamp_id != source_timestamp_id.load())
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
					if(timestamp_id != source_timestamp_id.load())
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
irr::video::IImage* ImageManager::GetTextureFromFile(const char* file, s32 width, s32 height, chrono_time timestamp_id, std::atomic<chrono_time>& source_timestamp_id) {
	irr::video::IImage* srcimg = driver->createImageFromFile(file);
	if(srcimg == NULL || timestamp_id != source_timestamp_id.load())
		return NULL;
	if(srcimg->getDimension() == irr::core::dimension2d<u32>(width, height)) {
		return srcimg;
	} else {
		video::IImage *destimg = driver->createImage(srcimg->getColorFormat(), irr::core::dimension2d<u32>(width, height));
		if(timestamp_id != source_timestamp_id.load() || !imageScaleNNAA(srcimg, destimg, width, height, timestamp_id, std::ref(source_timestamp_id))) {
			destimg->drop();
			destimg = nullptr;
		}
		srcimg->drop();
		return destimg;
	}
}
ImageManager::image_path ImageManager::LoadCardTexture(int code, int width, int height, chrono_time timestamp_id, std::atomic<chrono_time>& source_timestamp_id) {
	irr::video::IImage* img = nullptr;
	pic_download.lock();
	bool should_download = downloading_pics.find(code) == downloading_pics.end();
	pic_download.unlock();
	if(!should_download) {
		while(!downloading_pics[code].first) {
			if(timestamp_id != source_timestamp_id.load()) {
				return std::make_pair(nullptr, "fail");
			}
		}
	}
	for(auto& path : mainGame->pic_dirs) {
		for(auto& extension : { ".png", ".jpg" }) {
			if(timestamp_id != source_timestamp_id.load())
				return std::make_pair(nullptr, "fail");
			auto file = path + std::to_string(code) + extension;
			if(img = GetTextureFromFile(file.c_str(), width, height, timestamp_id, std::ref(source_timestamp_id))) {
				if(timestamp_id != source_timestamp_id.load()) {
					img->drop();
					return std::make_pair(nullptr, "fail");
				}
				return std::make_pair(img, file);
			}
		}
	}
	if(should_download) {
		std::async(std::launch::async, &ImageManager::DownloadPic, this, code);
		while(!downloading_pics[code].first) {
			if(timestamp_id != source_timestamp_id.load()) {
				return std::make_pair(nullptr, "fail");
			}
		}
		auto file = downloading_pics[code].second;
		if(img = GetTextureFromFile(file.c_str(), width, height, timestamp_id, std::ref(source_timestamp_id))) {
			if(timestamp_id != source_timestamp_id.load()) {
				img->drop();
				return std::make_pair(nullptr, "fail");
			}
			return std::make_pair(img, file);
		}
	}
	return std::make_pair(img, "");
}
irr::video::ITexture* ImageManager::GetTexture(int code, bool wait, bool fit, int* chk) {
	if(chk)
		*chk = 1;
	if(code == 0)
		return tUnknown;
	int index = fit ? 1 : 0;
	auto& map = tMap[index];
	auto tit = map.find(code);
	if(tit == map.end()) {
		auto a = loading_pics[index]->find(code);
		if(chk)
			*chk = 2;
		if(a == loading_pics[index]->end()) {
			int width = CARD_IMG_WIDTH;
			int height = CARD_IMG_HEIGHT;
			if(fit) {
				width = width * mainGame->window_size.Width / 1024;
				height = height * mainGame->window_size.Height / 640;
			}
			if(wait) {
				auto tmp_img = LoadCardTexture(code, width, height, timestamp_id[index].load(), timestamp_id[index]);
				if(tmp_img.first) {
					map[code] = driver->addTexture(tmp_img.second.c_str(), tmp_img.first);
					tmp_img.first->drop();
					if(chk)
						*chk = 1;
				} else {
					map[code] = nullptr;
					if(chk)
						*chk = 0;
				}
				return (map[code]) ? map[code] : tUnknown;
			} else {
				(*loading_pics[index])[code] = std::async(std::launch::async, &ImageManager::LoadCardTexture, this, code, width, height, timestamp_id[index].load(), std::ref(timestamp_id[index]));
			}
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
		auto a = loading_pics[2]->find(code);
		if(chk)
			*chk = 2;
		if(a == loading_pics[2]->end()) {
			int width = CARD_THUMB_WIDTH * mainGame->window_size.Width / 1024;
			int height = CARD_THUMB_HEIGHT * mainGame->window_size.Height / 640;
			if(wait) {
				auto tmp_img = LoadCardTexture(code, width, height, timestamp_id[2].load(), timestamp_id[2]);
				if(tmp_img.first) {
					tThumb[code] = driver->addTexture(tmp_img.second.c_str(), tmp_img.first);
					tmp_img.first->drop();
					if(chk)
						*chk = 1;
				} else {
					tThumb[code] = nullptr;
					if(chk)
						*chk = 0;
				}
				return (tThumb[code]) ? tThumb[code] : tUnknown;
			} else {
				(*loading_pics[2])[code] = std::async(std::launch::async, &ImageManager::LoadCardTexture, this, code, width, height, timestamp_id[2].load(), std::ref(timestamp_id[2]));
			}
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
		field_download.lock();
		bool should_download = downloading_fields.find(code) == downloading_fields.end();
		field_download.unlock();
		irr::video::ITexture* img = nullptr;
		if(!should_download) {
			if(downloading_fields[code].first) {
				img = driver->getTexture(downloading_fields[code].second.c_str());
			}
		} else {
			for(auto& path : mainGame->field_dirs) {
				for(auto& extension : { ".png", ".jpg" }) {
					if(img = driver->getTexture((path + std::to_string(code) + extension).c_str()))
						return img;
				}
			}
		}
		if(should_download)
			std::async(std::launch::async, &ImageManager::DownloadField, this, code);
		else
			tFields[code] = img;
		return img;
	}
	return (tit->second) ? tit->second : nullptr;
}
}
