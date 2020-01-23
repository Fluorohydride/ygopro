#include <IImage.h>
#include <IVideoDriver.h>
#include <IrrlichtDevice.h>
#include <IReadFile.h>
#include "image_manager.h"
#include "game.h"
#include <fstream>
#include <curl/curl.h>
#ifdef __ANDROID__
#include "porting_android.h"
#endif

namespace ygo {

ImageManager imageManager;

void ImageManager::AddDownloadResource(PicSource src) {
	pic_urls.push_back(src);
}

bool ImageManager::Initial() {
	timestamp_id = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	tCover[0] = GetTextureFromFile(EPRO_TEXT("textures/cover.jpg"), mainGame->Scale(CARD_IMG_WIDTH), mainGame->Scale(CARD_IMG_HEIGHT));
	tCover[1] = GetTextureFromFile(EPRO_TEXT("textures/cover2.jpg"), mainGame->Scale(CARD_IMG_WIDTH), mainGame->Scale(CARD_IMG_HEIGHT));
	if(!tCover[1])
		tCover[1] = tCover[0];
	tUnknown = driver->getTexture(EPRO_TEXT("textures/unknown.jpg"));
	tAct = driver->getTexture(EPRO_TEXT("textures/act.png"));
	tAttack = driver->getTexture(EPRO_TEXT("textures/attack.png"));
	tChain = driver->getTexture(EPRO_TEXT("textures/chain.png"));
	tNegated = driver->getTexture(EPRO_TEXT("textures/negated.png"));
	tNumber = driver->getTexture(EPRO_TEXT("textures/number.png"));
	tLPBar = driver->getTexture(EPRO_TEXT("textures/lp.png"));
	tLPFrame = driver->getTexture(EPRO_TEXT("textures/lpf.png"));
	tMask = driver->getTexture(EPRO_TEXT("textures/mask.png"));
	tEquip = driver->getTexture(EPRO_TEXT("textures/equip.png"));
	tTarget = driver->getTexture(EPRO_TEXT("textures/target.png"));
	tChainTarget = driver->getTexture(EPRO_TEXT("textures/chaintarget.png"));
	tLim = driver->getTexture(EPRO_TEXT("textures/lim.png"));
	tHand[0] = GetTextureFromFile(EPRO_TEXT("textures/f1.jpg"), mainGame->Scale(89), mainGame->Scale(128));
	tHand[1] = GetTextureFromFile(EPRO_TEXT("textures/f2.jpg"), mainGame->Scale(89), mainGame->Scale(128));
	tHand[2] = GetTextureFromFile(EPRO_TEXT("textures/f3.jpg"), mainGame->Scale(89), mainGame->Scale(128));
	tBackGround = driver->getTexture(EPRO_TEXT("textures/bg.jpg"));
	tBackGround_menu = driver->getTexture(EPRO_TEXT("textures/bg_menu.jpg"));
	if(!tBackGround_menu)
		tBackGround_menu = tBackGround;
	tBackGround_deck = driver->getTexture(EPRO_TEXT("textures/bg_deck.jpg"));
	if(!tBackGround_deck)
		tBackGround_deck = tBackGround;
	tField[0][0] = driver->getTexture(EPRO_TEXT("textures/field2.png"));
	tFieldTransparent[0][0] = driver->getTexture(EPRO_TEXT("textures/field-transparent2.png"));
	tField[0][1] = driver->getTexture(EPRO_TEXT("textures/field3.png"));
	tFieldTransparent[0][1] = driver->getTexture(EPRO_TEXT("textures/field-transparent3.png"));
	tField[0][2] = driver->getTexture(EPRO_TEXT("textures/field.png"));
	tFieldTransparent[0][2] = driver->getTexture(EPRO_TEXT("textures/field-transparent.png"));
	tField[0][3] = driver->getTexture(EPRO_TEXT("textures/field4.png"));
	tFieldTransparent[0][3] = driver->getTexture(EPRO_TEXT("textures/field-transparent4.png"));
	tField[1][0] = driver->getTexture(EPRO_TEXT("textures/fieldSP2.png"));
	tFieldTransparent[1][0] = driver->getTexture(EPRO_TEXT("textures/field-transparentSP2.png"));
	tField[1][1] = driver->getTexture(EPRO_TEXT("textures/fieldSP3.png"));
	tFieldTransparent[1][1] = driver->getTexture(EPRO_TEXT("textures/field-transparentSP3.png"));
	tField[1][2] = driver->getTexture(EPRO_TEXT("textures/fieldSP.png"));
	tFieldTransparent[1][2] = driver->getTexture(EPRO_TEXT("textures/field-transparentSP.png"));
	tField[1][3] = driver->getTexture(EPRO_TEXT("textures/fieldSP4.png"));
	tFieldTransparent[1][3] = driver->getTexture(EPRO_TEXT("textures/field-transparentSP4.png"));
	sizes[0].first = CARD_IMG_WIDTH * mainGame->gameConf.dpi_scale;
	sizes[0].second = CARD_IMG_HEIGHT * mainGame->gameConf.dpi_scale;
	sizes[1].first = CARD_IMG_WIDTH * mainGame->window_scale.X * mainGame->gameConf.dpi_scale;
	sizes[1].second = CARD_IMG_HEIGHT * mainGame->window_scale.Y * mainGame->gameConf.dpi_scale;
	sizes[2].first = CARD_THUMB_WIDTH * mainGame->window_scale.X * mainGame->gameConf.dpi_scale;
	sizes[2].second = CARD_THUMB_HEIGHT * mainGame->window_scale.Y * mainGame->gameConf.dpi_scale;
	stop_threads = false;
	for(int i = 0; i < 8; i++) {
		download_threads[i] = std::thread(&ImageManager::DownloadPic, this);
	}
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
		sizes[1].first = CARD_IMG_WIDTH * mainGame->window_scale.X * mainGame->gameConf.dpi_scale;
		sizes[1].second = CARD_IMG_HEIGHT * mainGame->window_scale.Y * mainGame->gameConf.dpi_scale;
		sizes[2].first = CARD_THUMB_WIDTH * mainGame->window_scale.X * mainGame->gameConf.dpi_scale;
		sizes[2].second = CARD_THUMB_HEIGHT * mainGame->window_scale.Y * mainGame->gameConf.dpi_scale;
		auto tmp_cover = GetTextureFromFile(EPRO_TEXT("textures/cover.jpg"), sizes[1].first, sizes[1].second);
		if(tmp_cover) {
			driver->removeTexture(tCover[0]);
			tCover[0] = tmp_cover;
		}
		tmp_cover = GetTextureFromFile(EPRO_TEXT("textures/cover2.jpg"), sizes[1].first, sizes[1].second);
		if(tmp_cover) {
			driver->removeTexture(tCover[1]);
			tCover[1] = tmp_cover;
		}
	}
	if(!resize) {
		ClearCachedTextures(resize);
	}
	f(tMap[0]);
	f(tMap[1]);
	f(tThumb);
	f(tFields);
	f(tCovers);
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
#define LOAD_LOOP(src, dest, index, type)for(auto it = src->begin(); it != src->end();) {\
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
			} else if(pair.second != EPRO_TEXT("wait for download"))\
				dest[it->first] = nullptr;\
			it = src->erase(it);\
			continue;\
		}\
		it++;\
	}\
	for(auto& code : readd) {\
		(*src)[code] = std::async(std::launch::async, &ImageManager::LoadCardTexture, this, code, type, std::ref(sizes[index].first), std::ref(sizes[index].second), timestamp_id.load(), std::ref(timestamp_id));\
	}\
	readd.clear();
void ImageManager::RefreshCachedTextures() {
	std::vector<int> readd;
	LOAD_LOOP(loading_pics[0], tMap[0], 0, ART)
	LOAD_LOOP(loading_pics[1], tMap[1], 1, ART)
	LOAD_LOOP(loading_pics[2], tThumb, 2, THUMB)
		for(auto it = loading_pics[3]->begin(); it != loading_pics[3]->end();) {
			if(it->second.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
				auto pair = it->second.get();
				if(pair.first) {
					if(pair.first->getDimension().Width != sizes[1].first || pair.first->getDimension().Height != sizes[1].second) {
						readd.push_back(it->first);
						tCovers[it->first] = nullptr;
						it = loading_pics[3]->erase(it);
						continue;
					}
					tCovers[it->first] = driver->addTexture(pair.second.c_str(), pair.first);
					pair.first->drop();
				} else if(pair.second != EPRO_TEXT("wait for download"))
					tCovers[it->first] = nullptr;
				it = loading_pics[3]->erase(it);
				continue;
			}
			it++;
		}
	for(auto& code : readd) {
		(*loading_pics[3])[code] = std::async(std::launch::async, &ImageManager::LoadCardTexture, this, code, COVER, std::ref(sizes[1].first), std::ref(sizes[1].second), timestamp_id.load(), std::ref(timestamp_id));
	}
	readd.clear();
}
#undef LOAD_LOOP
void ImageManager::ClearFutureObjects(loading_map* map1, loading_map* map2, loading_map* map3, loading_map* map4) {
	for(auto& map : { &map1, &map2, &map3 }) {
		if(*map) {
			for(auto it = (*map)->begin(); it != (*map)->end();) {
				auto pair = it->second.get();
				if(pair.first)
					pair.first->drop();
				it = (*map)->erase(it);
			}
			delete (*map);
		}
	}
}
#define PNG_HEADER 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a
#define PNG_FILE 1
#define JPG_HEADER 0xff, 0xd8, 0xff
#define JPG_FILE 2
int CheckImageHeader(char* header) {
	static unsigned char pngheader[] = { PNG_HEADER }; //png header
	static unsigned char jpgheader[] = { JPG_HEADER }; //jpg header
	if(!memcmp(pngheader, header, sizeof(pngheader))) {
		return PNG_FILE;
	} else if(!memcmp(jpgheader, header, sizeof(jpgheader))) {
		return JPG_FILE;
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
	if(data->header_written < sizeof(data->header)) {
		auto increase = std::min(nmemb * size, (size_t)(sizeof(data->header) - data->header_written));
		memcpy(&data->header[data->header_written], ptr, increase);
		data->header_written += increase;
		if(data->header_written == sizeof(data->header) && !CheckImageHeader(data->header))
			return -1;
	}
	std::ofstream* out = data->stream;
	size_t nbytes = size * nmemb;
	out->write(ptr, nbytes);
	return nbytes;
}
const irr::fschar_t* GetExtension(char* header) {
	int res = CheckImageHeader(header);
	if(res == PNG_FILE)
		return EPRO_TEXT(".png");
	else if(res == JPG_FILE)
		return EPRO_TEXT(".jpg");
	return EPRO_TEXT("");
}
void ImageManager::DownloadPic() {
	path_string dest_folder;// = fmt::format(EPRO_TEXT("./pics/{}"), code);
	path_string name;// = fmt::format(EPRO_TEXT("./pics/temp/{}"), code);
	path_string ext;
	while(!stop_threads) {
		if(to_download.size() == 0) {
			std::unique_lock<std::mutex> lck(mtx);
			cv.wait(lck);
		}
		pic_download.lock();
		if(to_download.size() == 0) {
			pic_download.unlock();
			continue;
		}
		auto file = to_download.front();
		to_download.pop();
		auto type = file.type;
		auto code = file.code;
		downloading_images[type][file.code].status = DOWNLOADING;
		downloading.push_back(std::move(file));
		pic_download.unlock();
		name = fmt::format(EPRO_TEXT("./pics/temp/{}"), code);
		if(type == THUMB)
			type = ART;
		switch(type) {
			case ART:
			case THUMB: {
				dest_folder = fmt::format(EPRO_TEXT("./pics/{}"), code);
				break;
			}
			case FIELD: {
				dest_folder = fmt::format(EPRO_TEXT("./pics/field/{}"), code);
				name.append(EPRO_TEXT("_f"));
				break;
			}
			case COVER: {
				dest_folder = fmt::format(EPRO_TEXT("./pics/cover/{}"), code);
				name.append(EPRO_TEXT("_c"));
				break;
			}
		}
		auto& map = downloading_images[static_cast<int>(type)];
		for(auto& src : pic_urls) {
			if(src.type != type)
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
#ifdef __ANDROID__
					curl_easy_setopt(curl, CURLOPT_CAINFO, (porting::internal_storage + "/cacert.cer").c_str());
#endif
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
		pic_download.lock();
		if(ext.size()) {
			map[code].status = DOWNLOADED;
			map[code].path = dest_folder + ext;
		} else
			map[code].status = DOWNLOAD_ERROR;
		pic_download.unlock();
	}
}
void ImageManager::AddToDownloadQueue(int code, imgType type) {
	if(type == THUMB)
		type = ART;
	int index = static_cast<int>(type);
	pic_download.lock();
	if(downloading_images[index].find(code) == downloading_images[index].end()) {
		downloading_images[index][code].status = DOWNLOADING;
		to_download.push(downloadParam{ code, type, NONE, EPRO_TEXT("") });
	}
	pic_download.unlock();
	cv.notify_one();
}
ImageManager::downloadStatus ImageManager::GetDownloadStatus(int code, imgType type) {
	if(type == THUMB)
		type = ART;
	int index = static_cast<int>(type);
	std::lock_guard<std::mutex> lk(pic_download);
	if(downloading_images[index].find(code) == downloading_images[index].end())
		return NONE;
	return downloading_images[index][code].status;
}
path_string ImageManager::GetDownloadPath(int code, imgType type) {
	if(type == THUMB)
		type = ART;
	int index = static_cast<int>(type);
	std::lock_guard<std::mutex> lk(pic_download);
	if(downloading_images[index].find(code) == downloading_images[index].end())
		return EPRO_TEXT("");
	return downloading_images[index][code].path;
}
void ImageManager::ClearCachedTextures(bool resize) {
	timestamp_id = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	if(loading_pics[0]->size() + loading_pics[1]->size() + loading_pics[2]->size() + loading_pics[3]->size()) {
		std::thread(&ImageManager::ClearFutureObjects, this, loading_pics[0], loading_pics[1], loading_pics[2], loading_pics[3]).detach();
		loading_pics[0] = new loading_map();
		loading_pics[1] = new loading_map();
		loading_pics[2] = new loading_map();
		loading_pics[3] = new loading_map();
	}
}
// function by Warr1024, from https://github.com/minetest/minetest/issues/2419 , modified
bool ImageManager::imageScaleNNAA(irr::video::IImage *src, irr::video::IImage *dest, irr::s32 width, irr::s32 height, chrono_time timestamp_id, std::atomic<chrono_time>& source_timestamp_id) {
	double sx, sy, minsx, maxsx, minsy, maxsy, area, ra, ga, ba, aa, pw, ph, pa;
	irr::u32 dy, dx;
	irr::video::SColor pxl;

	// Cache rectsngle boundaries.
	double sw = src->getDimension().Width * 1.0;
	double sh = src->getDimension().Height * 1.0;

	// Walk each destination image pixel.
	// Note: loop y around x for better cache locality.
	irr::core::dimension2d<irr::u32> dim = dest->getDimension();
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
					pxl = src->getPixel((irr::u32)sx, (irr::u32)sy);
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
irr::video::IImage* ImageManager::GetTextureImageFromFile(const irr::io::path& file, int width, int height, chrono_time timestamp_id, std::atomic<chrono_time>& source_timestamp_id, irr::io::IReadFile* archivefile) {
	irr::video::IImage* srcimg = nullptr;
	if(archivefile)
		srcimg = driver->createImageFromFile(archivefile);
	else
		srcimg = driver->createImageFromFile(file);
	if(srcimg == NULL || timestamp_id != source_timestamp_id.load()) {
		if(srcimg)
			srcimg->drop();
		return NULL;
	}
	if(srcimg->getDimension() == irr::core::dimension2d<irr::u32>(width, height)) {
		return srcimg;
	} else {
		irr::video::IImage *destimg = driver->createImage(srcimg->getColorFormat(), irr::core::dimension2d<irr::u32>(width, height));
		if(timestamp_id != source_timestamp_id.load() || !imageScaleNNAA(srcimg, destimg, width, height, timestamp_id, std::ref(source_timestamp_id))) {
			destimg->drop();
			destimg = nullptr;
		}
		srcimg->drop();
		return destimg;
	}
}
irr::video::ITexture* ImageManager::GetTextureFromFile(const irr::io::path & file, int width, int height) {
	auto img = GetTextureImageFromFile(file, width, height, timestamp_id.load(), std::ref(timestamp_id));
	if(img) {
		auto texture = driver->addTexture(file, img);
		img->drop();
		if(texture)
			return texture;
	}
	return driver->getTexture(file);
}
ImageManager::image_path ImageManager::LoadCardTexture(int code, imgType type, std::atomic<irr::s32>& _width, std::atomic<irr::s32>& _height, chrono_time timestamp_id, std::atomic<chrono_time>& source_timestamp_id) {
	irr::video::IImage* img = nullptr;
	int width = _width;
	int height = _height;
	if(type == THUMB)
		type = ART;
	for(auto& path : (type == ART) ? mainGame->pic_dirs : mainGame->cover_dirs) {
		for(auto extension : { EPRO_TEXT(".png"), EPRO_TEXT(".jpg") }) {
			if(timestamp_id != source_timestamp_id.load())
				return std::make_pair(nullptr, EPRO_TEXT("fail"));
			irr::io::IReadFile* reader = nullptr;
			if(path == EPRO_TEXT("archives")) {
				reader = Utils::FindandOpenFileFromArchives((type == ART) ? EPRO_TEXT("pics/") : EPRO_TEXT("pics/cover/"), fmt::format(EPRO_TEXT("{}{}"), code, extension));
				if(!reader)
					continue;
			}
			if(width != _width || height != _height) {
				width = _width;
				height = _height;
			}
			auto file = reader ? reader->getFileName().c_str() : fmt::format(EPRO_TEXT("{}{}{}"), path, code, extension);
			__repeat:
			if(img = GetTextureImageFromFile(file.c_str(), width, height, timestamp_id, std::ref(source_timestamp_id), reader)) {
				if(timestamp_id != source_timestamp_id.load()) {
					img->drop();
					if(reader) {
						reader->drop();
						reader = nullptr;
					}
					return std::make_pair(nullptr, EPRO_TEXT("fail"));
				}
				if(width != _width || height != _height) {
					img->drop();
					width = _width;
					height = _height;
					goto __repeat;
				}
				if(reader) {
					reader->drop();
					reader = nullptr;
				}
				return std::make_pair(img, Utils::ParseFilename(file));
			}
			if(timestamp_id != source_timestamp_id.load()) {
				if(reader) {
					reader->drop();
					reader = nullptr;
				}
				return std::make_pair(nullptr, EPRO_TEXT("fail"));
			}
			if(reader) {
				reader->drop();
				reader = nullptr;
			}
		}
	}
	if(GetDownloadStatus(code, type) == NONE) {
		AddToDownloadQueue(code, type);
	}
	return std::make_pair(nullptr, EPRO_TEXT("wait for download"));
}
irr::video::ITexture* ImageManager::GetTextureCard(int code, imgType type, bool wait, bool fit, int* chk) {
	if(chk)
		*chk = 1;
	irr::video::ITexture* ret_unk = tUnknown;
	int index;
	int size_index;
	auto& map = [&]()->texture_map& {
		switch(type) {
			case ART: {
				index = fit ? 1 : 0;
				size_index = index;
				return tMap[fit ? 1 : 0];
			}
			case THUMB:	{
				index = 2;
				size_index = index;
				return tThumb;
			}
			case COVER:	{
				ret_unk = tCover[0];
				index = 3;
				size_index = 0;
				return tCovers;
			}
			case FIELD: /*should never come here*/ {
				return tMap[0];
			}
		}
	}();
	if(code == 0)
		return ret_unk;
	auto tit = map.find(code);
	if(tit == map.end()) {
		auto status = GetDownloadStatus(code, type);
		if(status == DOWNLOADING) {
			if(chk)
				*chk = 2;
			return ret_unk;
		}
		if(status == DOWNLOADED) {
			map[code] = driver->getTexture(GetDownloadPath(code, type).c_str());
			return map[code] ? map[code] : ret_unk;
		}
		if(status == DOWNLOAD_ERROR) {
			map[code] = nullptr;
			return ret_unk;
		}
		auto a = loading_pics[index]->find(code);
		if(chk)
			*chk = 2;
		if(a == loading_pics[index]->end()) {
			if(wait) {
				auto tmp_img = LoadCardTexture(code, type, std::ref(sizes[size_index].first), std::ref(sizes[size_index].second), timestamp_id.load(), std::ref(timestamp_id));
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
				return (map[code]) ? map[code] : ret_unk;
			} else {
				(*loading_pics[index])[code] = std::async(std::launch::async, &ImageManager::LoadCardTexture, this, code, type, std::ref(sizes[size_index].first), std::ref(sizes[size_index].second), timestamp_id.load(), std::ref(timestamp_id));
			}
		}
		return ret_unk;
	}
	if(chk && !tit->second)
		*chk = 0;
	return (tit->second) ? tit->second : ret_unk;
}
irr::video::ITexture* ImageManager::GetTextureField(int code) {
	if(code == 0)
		return nullptr;
	auto tit = tFields.find(code);
	if(tit == tFields.end()) {
		auto status = GetDownloadStatus(code, FIELD);
		bool should_download = status == NONE;
		irr::video::ITexture* img = nullptr;
		if(!should_download) {
			if(status == DOWNLOADED) {
				img = driver->getTexture(GetDownloadPath(code, FIELD).c_str());
			} else
				return nullptr;
		} else {
			for(auto& path : mainGame->field_dirs) {
				for(auto extension : { EPRO_TEXT(".png"), EPRO_TEXT(".jpg") }) {
					irr::io::IReadFile* reader = nullptr;
					if(path == EPRO_TEXT("archives")) {
						reader = Utils::FindandOpenFileFromArchives(EPRO_TEXT("pics/field/"), fmt::format(EPRO_TEXT("{}{}"), code, extension));
						if(!reader)
							continue;
						img = driver->getTexture(reader);
						reader->drop();
						if(img)
							break;
					} else {
						if(img = driver->getTexture(fmt::format(EPRO_TEXT("{}{}{}"), path, code, extension).c_str()))
							break;
					}
				}
			}
		}
		if(should_download && !img)
			AddToDownloadQueue(code, FIELD);
		else
			tFields[code] = img;
		return img;
	}
	return (tit->second) ? tit->second : nullptr;
}
}
