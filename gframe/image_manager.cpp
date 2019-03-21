#include "image_manager.h"
#include "game.h"
#include <curl/curl.h>

namespace ygo {

ImageManager imageManager;

void ImageManager::AddDownloadResource(PicSource src) {
	pic_urls.push_back(src);
}

bool ImageManager::Initial() {
	timestamp_id = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
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
	sizes[0].first = CARD_IMG_WIDTH;
	sizes[0].second = CARD_IMG_HEIGHT;
	sizes[1].first = CARD_IMG_WIDTH * mainGame->window_size.Width / 1024;
	sizes[1].second = CARD_IMG_HEIGHT * mainGame->window_size.Height / 640;
	sizes[2].first = CARD_THUMB_WIDTH * mainGame->window_size.Width / 1024;
	sizes[2].second = CARD_THUMB_HEIGHT * mainGame->window_size.Height / 640;
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
		sizes[1].first = CARD_IMG_WIDTH * mainGame->window_size.Width / 1024;
		sizes[1].second = CARD_IMG_HEIGHT * mainGame->window_size.Height / 640;
		sizes[2].first = CARD_THUMB_WIDTH * mainGame->window_size.Width / 1024;
		sizes[2].second = CARD_THUMB_HEIGHT * mainGame->window_size.Height / 640;
	}
	if(!resize) {
		ClearCachedTextures(resize);
	}
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
#define LOAD_LOOP(src, dest, index)for(auto it = src->begin(); it != src->end();) {\
		if(it->second.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {\
			auto pair = it->second.get();\
			if(pair.first) {\
				if(pair.first->getDimension().Width != sizes[index].first || pair.first->getDimension().Height != sizes[index].second) {\
					readd.push_back(it->first);\
					dest[it->first] = nullptr;\
					it = src->erase(it);\
					continue;\
				}\
				dest[it->first] = driver->addTexture(pair.second.c_str(), pair.first);\
				pair.first->drop();\
			} else if(pair.second != "wait for download")\
				dest[it->first] = nullptr;\
			it = src->erase(it);\
			continue;\
		}\
		it++;\
	}\
	for(auto& code : readd) {\
		(*src)[code] = std::async(std::launch::async, &ImageManager::LoadCardTexture, this, code, std::ref(sizes[index].first), std::ref(sizes[index].second), timestamp_id.load(), std::ref(timestamp_id));\
	}\
	readd.clear();
void ImageManager::RefreshCachedTextures() {
	std::vector<int> readd;
	LOAD_LOOP(loading_pics[0], tMap[0], 0)
	LOAD_LOOP(loading_pics[1], tMap[1], 1)
	LOAD_LOOP(loading_pics[2], tThumb, 2)
}
#undef LOAD_LOOP
void ImageManager::ClearFutureObjects(loading_map* map) {
	for(auto it = map->begin(); it != map->end();) {
		auto pair = it->second.get();
		if(pair.first)
			pair.first->drop();
		it = map->erase(it);
	}
	delete map;
}
int CheckImageHeader(char* header) {
	unsigned char pngheader[] = { 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a }; //png header
	unsigned char jpgheader[] = { 0xff, 0xd8, 0xff }; //jpg header
	if(!memcmp(pngheader, header, sizeof(pngheader))) {
		return 1;
	} else if(!memcmp(jpgheader, header, sizeof(jpgheader))) {
		return 2;
	} else
		return 0;
}
size_t write_data(char *ptr, size_t size, size_t nmemb, void *userdata) {
	struct payload {
		std::ofstream* stream;
		char header[8];
		int header_written;
	};
	auto data = static_cast<payload*>(userdata);
	if(data->header_written < 8) {
		auto increase = std::min(nmemb * size, (size_t)(8 - data->header_written));
		memcpy(&data->header[data->header_written], ptr, increase);
		data->header_written += increase;
		if(data->header_written == 8 && !CheckImageHeader(data->header))
			return -1;
	}
	std::ofstream *out = data->stream;
	size_t nbytes = size * nmemb;
	out->write(ptr, nbytes);
	return nbytes;
}
const char* GetExtension(char* header) {
	std::string extension;
	int res = CheckImageHeader((char*)header);
	if(res == 1)
		return ".png";
	else if(res == 2)
		return ".jpg";
	return "";
}
void ImageManager::DownloadPic(int code) {
	auto id = std::to_string(code);
	std::string dest_folder = "./pics/" + id;
	auto name = "./pics/temp/" + id;
	std::string ext;
	pic_download.lock();
	if(downloading_pics.find(code) == downloading_pics.end()) {
		downloading_pics[code].first = 0;
		pic_download.unlock();
		for(auto& src : pic_urls) {
			if(src.type == "field")
				continue;
			CURL *curl = NULL;
			struct {
				std::ofstream* stream;
				char header[8] = { 0 };
				int header_written = 0;
			} payload;
			std::ofstream fp(name, std::ofstream::binary);
			if(fp.is_open()) {
				payload.stream = &fp;
				CURLcode res;
				curl = curl_easy_init();
				if(curl) {
					curl_easy_setopt(curl, CURLOPT_URL, fmt::format(src.url, code).c_str());
					curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
					curl_easy_setopt(curl, CURLOPT_WRITEDATA, &payload);
					res = curl_easy_perform(curl);
					curl_easy_cleanup(curl);
					fp.close();
					if(res == CURLE_OK) {
						ext = GetExtension(payload.header);
						Utils::Movefile(name, dest_folder + ext);
						break;
					} else {
						Utils::Deletefile(name);
					}
				}
			}
		}
	} else {
		pic_download.unlock();
		return;
	}
	pic_download.lock();
	if(ext.size()) {
		downloading_pics[code].first = 2;
		downloading_pics[code].second = dest_folder + ext;
	} else
		downloading_pics[code].first = 1;
	pic_download.unlock();
}
void ImageManager::DownloadField(int code) {
	auto id = std::to_string(code);
	std::string dest_folder = "./pics/field/" + id;
	auto name = "./pics/temp/" + id + "_f";
	std::string ext;
	field_download.lock();
	if(downloading_fields.find(code) == downloading_fields.end()) {
		downloading_fields[code].first = 0;
		field_download.unlock();
		for(auto& src : pic_urls) {
			if(src.type == "pic")
				continue;
			CURL *curl = NULL;
			struct {
				std::ofstream* stream;
				char header[8] = { 0 };
				int header_written = 0;
			} payload;
			std::ofstream fp(name, std::ofstream::binary);
			payload.stream = &fp;
			CURLcode res;
			curl = curl_easy_init();
			if(curl) {
				curl_easy_setopt(curl, CURLOPT_URL, fmt::format(src.url, code).c_str());
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, &payload);
				res = curl_easy_perform(curl);
				curl_easy_cleanup(curl);
				fp.close();
				if(res == CURLE_OK) {
					ext = GetExtension(payload.header);
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
	if(ext.size()) {
		downloading_fields[code].first = 2;
		downloading_fields[code].second = dest_folder + ext;
	} else
		downloading_fields[code].first = 1;
	field_download.unlock();
}
#define CHANGE_LIST(list)if(list->size()) {\
							std::thread(&ImageManager::ClearFutureObjects, this, list).detach();\
							list = new loading_map();\
						}
void ImageManager::ClearCachedTextures(bool resize) {
	timestamp_id = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	CHANGE_LIST(loading_pics[0])
	CHANGE_LIST(loading_pics[1])
	CHANGE_LIST(loading_pics[2])
}
#undef CHANGE_LIST
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
irr::video::IImage* ImageManager::GetTextureFromFile(const char* file, int width, int height, chrono_time timestamp_id, std::atomic<chrono_time>& source_timestamp_id) {
	irr::video::IImage* srcimg = driver->createImageFromFile(file);
	if(srcimg == NULL || timestamp_id != source_timestamp_id.load()) {
		if(srcimg)
			srcimg->drop();
		return NULL;
	}
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
ImageManager::image_path ImageManager::LoadCardTexture(int code, std::atomic<s32>& _width, std::atomic<s32>& _height, chrono_time timestamp_id, std::atomic<chrono_time>& source_timestamp_id) {
	irr::video::IImage* img = nullptr;
	int width = _width;
	int height = _height;
	for(auto& path : mainGame->pic_dirs) {
		for(auto& extension : { ".png", ".jpg" }) {
			if(timestamp_id != source_timestamp_id.load())
				return std::make_pair(nullptr, "fail");
			auto file = path + std::to_string(code) + extension;
			if(width != _width || height != _height) {
				width = _width;
				height = _height;
			}
			__repeat:
			if(img = GetTextureFromFile(file.c_str(), width, height, timestamp_id, std::ref(source_timestamp_id))) {
				if(timestamp_id != source_timestamp_id.load()) {
					img->drop();
					return std::make_pair(nullptr, "fail");
				}
				if(width != _width || height != _height) {
					img->drop();
					width = _width;
					height = _height;
					goto __repeat;
				}
				return std::make_pair(img, file);
			}
			if(timestamp_id != source_timestamp_id.load())
				return std::make_pair(nullptr, "fail");
		}
	}
	pic_download.lock();
	if(downloading_pics.find(code) == downloading_pics.end()) {
		std::thread(&ImageManager::DownloadPic, this, code).detach();
	}
	pic_download.unlock();
	return std::make_pair(img, "wait for download");
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
		pic_download.lock();
		if(downloading_pics.find(code) != downloading_pics.end()) {
			if(downloading_pics[code].first == 0 || downloading_pics[code].first == 1) {
				pic_download.unlock();
				if(!downloading_pics[code].first && chk)
					*chk = 2;
				return tUnknown;
			}
		}
		pic_download.unlock();
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
				auto tmp_img = LoadCardTexture(code, std::ref(sizes[index].first), std::ref(sizes[index].second), timestamp_id.load(), std::ref(timestamp_id));
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
				(*loading_pics[index])[code] = std::async(std::launch::async, &ImageManager::LoadCardTexture, this, code, std::ref(sizes[index].first), std::ref(sizes[index].second), timestamp_id.load(), std::ref(timestamp_id));
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
		pic_download.lock();
		if(downloading_pics.find(code) != downloading_pics.end()) {
			if(downloading_pics[code].first == 0 || downloading_pics[code].first == 1) {
				pic_download.unlock();
				if(!downloading_pics[code].first && chk)
					*chk = 2;
				return tUnknown;
			}
		}
		pic_download.unlock();
		auto a = loading_pics[2]->find(code);
		if(chk)
			*chk = 2;
		if(a == loading_pics[2]->end()) {
			if(wait) {
				auto tmp_img = LoadCardTexture(code, std::ref(sizes[2].first), std::ref(sizes[2].second), timestamp_id.load(), std::ref(timestamp_id));
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
				(*loading_pics[2])[code] = std::async(std::launch::async, &ImageManager::LoadCardTexture, this, code, std::ref(sizes[2].first), std::ref(sizes[2].second), timestamp_id.load(), std::ref(timestamp_id));
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
			if(downloading_fields[code].first == 2) {
				img = driver->getTexture(downloading_fields[code].second.c_str());
			} else
				return nullptr;
		} else {
			for(auto& path : mainGame->field_dirs) {
				for(auto& extension : { ".png", ".jpg" }) {
					if(img = driver->getTexture((path + std::to_string(code) + extension).c_str()))
						break;
				}
			}
		}
		if(should_download && !img)
			std::thread(&ImageManager::DownloadField, this, code).detach();
		else
			tFields[code] = img;
		return img;
	}
	return (tit->second) ? tit->second : nullptr;
}
}
