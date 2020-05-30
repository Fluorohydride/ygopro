#include "game_config.h"
#include <IImage.h>
#include <IVideoDriver.h>
#include <IrrlichtDevice.h>
#include <IReadFile.h>
#include "image_manager.h"
#include "image_downloader.h"
#include "game.h"
#include <fstream>
#include <curl/curl.h>

#define BASE_PATH EPRO_TEXT("./textures/")

namespace ygo {

#define X(x) (textures_path + EPRO_TEXT(x)).c_str()
#define GET(obj,fun1,fun2) obj=fun1; if(!obj) obj=fun2;
#define GTFF(path,ext,w,h) GetTextureFromFile(X(path ext), mainGame->Scale(w), mainGame->Scale(h))
#define GET_TEXTURE_SIZED(obj,path,w,h) GET(obj,GTFF(path,".png",w,h),GTFF(path,".jpg",w,h)) def_##obj=obj;
#define GET_TEXTURE(obj,path) GET(obj,driver->getTexture(X(path ".png")),driver->getTexture(X(path ".jpg"))) def_##obj=obj;
bool ImageManager::Initial() {
	timestamp_id = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	textures_path = BASE_PATH;
	GET_TEXTURE_SIZED(tCover[0],"cover", CARD_IMG_WIDTH, CARD_IMG_HEIGHT)
	GET_TEXTURE_SIZED(tCover[1],"cover2", CARD_IMG_WIDTH, CARD_IMG_HEIGHT)
	if(!tCover[1]){
		tCover[1] = tCover[0];
		def_tCover[1] = tCover[1];
	}
	GET_TEXTURE_SIZED(tUnknown, "unknown", 177, 254)
	GET_TEXTURE(tAct, "act")
	GET_TEXTURE(tAttack, "attack")
	GET_TEXTURE(tChain, "chain")
	GET_TEXTURE_SIZED(tNegated, "negated", 128, 128)
	GET_TEXTURE_SIZED(tNumber, "number", 320, 256)
	GET_TEXTURE(tLPBar, "lp")
	GET_TEXTURE(tLPFrame, "lpf")
	GET_TEXTURE_SIZED(tMask, "mask", 254, 254)
	GET_TEXTURE(tEquip, "equip")
	GET_TEXTURE(tTarget, "target")
	GET_TEXTURE(tChainTarget, "chaintarget")
	GET_TEXTURE(tLim, "lim")
	GET_TEXTURE(tOT, "ot")
	GET_TEXTURE_SIZED(tHand[0], "f1", 89, 128)
	GET_TEXTURE_SIZED(tHand[1], "f2", 89, 128)
	GET_TEXTURE_SIZED(tHand[2], "f3", 89, 128)
	GET_TEXTURE(tBackGround, "bg")
	GET_TEXTURE(tBackGround_menu, "bg_menu")
	if(!tBackGround_menu)
		tBackGround_menu = tBackGround;
	GET_TEXTURE(tBackGround_deck, "bg_deck")
	if(!tBackGround_deck)
		tBackGround_deck = tBackGround;
	GET_TEXTURE(tField[0][0], "field2")
	GET_TEXTURE(tFieldTransparent[0][0], "field-transparent2")
	GET_TEXTURE(tField[0][1], "field3")
	GET_TEXTURE(tFieldTransparent[0][1], "field-transparent3")
	GET_TEXTURE(tField[0][2], "field")
	GET_TEXTURE(tFieldTransparent[0][2], "field-transparent")
	GET_TEXTURE(tField[0][3], "field4")
	GET_TEXTURE(tFieldTransparent[0][3], "field-transparent4")
	GET_TEXTURE(tField[1][0], "fieldSP2")
	GET_TEXTURE(tFieldTransparent[1][0], "field-transparentSP2")
	GET_TEXTURE(tField[1][1], "fieldSP3")
	GET_TEXTURE(tFieldTransparent[1][1], "field-transparentSP3")
	GET_TEXTURE(tField[1][2], "fieldSP")
	GET_TEXTURE(tFieldTransparent[1][2], "field-transparentSP")
	GET_TEXTURE(tField[1][3], "fieldSP4")
	GET_TEXTURE(tFieldTransparent[1][3], "field-transparentSP4")
	GET_TEXTURE(tSettings, "settings")
	sizes[0].first = CARD_IMG_WIDTH * gGameConfig->dpi_scale;
	sizes[0].second = CARD_IMG_HEIGHT * gGameConfig->dpi_scale;
	sizes[1].first = CARD_IMG_WIDTH * mainGame->window_scale.X * gGameConfig->dpi_scale;
	sizes[1].second = CARD_IMG_HEIGHT * mainGame->window_scale.Y * gGameConfig->dpi_scale;
	sizes[2].first = CARD_THUMB_WIDTH * mainGame->window_scale.X * gGameConfig->dpi_scale;
	sizes[2].second = CARD_THUMB_HEIGHT * mainGame->window_scale.Y * gGameConfig->dpi_scale;
	return true;
}

#undef GET
#undef GET_TEXTURE
#undef GET_TEXTURE_SIZED
#define GET(obj,fun1,fun2,fallback) obj=fun1; if(!obj) obj=fun2; if(!obj) obj=fallback;
#define GET_TEXTURE_SIZED(obj,path,w,h) GET(obj,GTFF(path,".png",w,h),GTFF(path,".jpg",w,h),def_##obj)
#define GET_TEXTURE(obj,path) GET(obj,driver->getTexture(X(path ".png")),driver->getTexture(X(path ".jpg")),def_##obj)
void ImageManager::ChangeTextures(const path_string & _path) {
	if(_path == textures_path)
		return;
	textures_path = _path;
	GET_TEXTURE_SIZED(tUnknown, "unknown", 177, 254)
	GET_TEXTURE(tAct, "act")
	GET_TEXTURE(tAttack, "attack")
	GET_TEXTURE(tChain, "chain")
	GET_TEXTURE_SIZED(tNegated, "negated", 128, 128)
	GET_TEXTURE_SIZED(tNumber, "number", 320, 256)
	GET_TEXTURE(tLPBar, "lp")
	GET_TEXTURE(tLPFrame, "lpf")
	GET_TEXTURE_SIZED(tMask, "mask", 254, 254)
	GET_TEXTURE(tEquip, "equip")
	GET_TEXTURE(tTarget, "target")
	GET_TEXTURE(tChainTarget, "chaintarget")
	GET_TEXTURE(tLim, "lim")
	GET_TEXTURE(tOT, "ot")
	GET_TEXTURE_SIZED(tHand[0], "f1", 89, 128)
	GET_TEXTURE_SIZED(tHand[1], "f2", 89, 128)
	GET_TEXTURE_SIZED(tHand[2], "f3", 89, 128)
	GET_TEXTURE(tBackGround, "bg")
	GET_TEXTURE(tBackGround_menu, "bg_menu")
	if(!tBackGround_menu)
		tBackGround_menu = tBackGround;
	GET_TEXTURE(tBackGround_deck, "bg_deck")
	if(!tBackGround_deck)
		tBackGround_deck = tBackGround;
	GET_TEXTURE(tField[0][0], "field2")
	GET_TEXTURE(tFieldTransparent[0][0], "field-transparent2")
	GET_TEXTURE(tField[0][1], "field3")
	GET_TEXTURE(tFieldTransparent[0][1], "field-transparent3")
	GET_TEXTURE(tField[0][2], "field")
	GET_TEXTURE(tFieldTransparent[0][2], "field-transparent")
	GET_TEXTURE(tField[0][3], "field4")
	GET_TEXTURE(tFieldTransparent[0][3], "field-transparent4")
	GET_TEXTURE(tField[1][0], "fieldSP2")
	GET_TEXTURE(tFieldTransparent[1][0], "field-transparentSP2")
	GET_TEXTURE(tField[1][1], "fieldSP3")
	GET_TEXTURE(tFieldTransparent[1][1], "field-transparentSP3")
	GET_TEXTURE(tField[1][2], "fieldSP")
	GET_TEXTURE(tFieldTransparent[1][2], "field-transparentSP")
	GET_TEXTURE(tField[1][3], "fieldSP4")
	GET_TEXTURE(tFieldTransparent[1][3], "field-transparentSP4")
	GET_TEXTURE(tSettings, "settings")
	RefreshCovers();
}
void ImageManager::ResetTextures() {
	ChangeTextures(BASE_PATH);
}
#undef GET_TEXTURE
#undef GET_TEXTURE_SIZED
#undef X
void ImageManager::SetDevice(irr::IrrlichtDevice* dev) {
	device = dev;
	driver = dev->getVideoDriver();
}
void ImageManager::ClearTexture(bool resize) {
	auto f = [&](texture_map &map) {
		for(auto tit = map.begin(); tit != map.end(); ++tit) {
			if(tit->second)
				driver->removeTexture(tit->second);
		}
		map.clear();
	};
	if(resize) {
		sizes[1].first = CARD_IMG_WIDTH * mainGame->window_scale.X * gGameConfig->dpi_scale;
		sizes[1].second = CARD_IMG_HEIGHT * mainGame->window_scale.Y * gGameConfig->dpi_scale;
		sizes[2].first = CARD_THUMB_WIDTH * mainGame->window_scale.X * gGameConfig->dpi_scale;
		sizes[2].second = CARD_THUMB_HEIGHT * mainGame->window_scale.Y * gGameConfig->dpi_scale;
		RefreshCovers();
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
void ImageManager::RemoveTexture(uint32_t code) {
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
	auto StartLoad = [this](loading_map* src, texture_map& dest, int index, imgType type) {
		std::vector<int> readd;
		for (auto it = src->begin(); it != src->end();) {
			if (it->second.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
				auto pair = it->second.get();
				if (pair.first) {
					if (pair.first->getDimension().Width != sizes[index].first || pair.first->getDimension().Height != sizes[index].second) {
						readd.push_back(it->first);
						dest[it->first] = nullptr;
						it = src->erase(it);
						continue;
					}
					dest[it->first] = driver->addTexture(pair.second.c_str(), pair.first);
					pair.first->drop();
				} else if (pair.second != EPRO_TEXT("wait for download")) {
					dest[it->first] = nullptr;
				}
				it = src->erase(it);
				continue;
			}
			it++;
		}
		for (auto& code : readd) {
			(*src)[code] = std::async(std::launch::async, &ImageManager::LoadCardTexture, this, code, type, std::ref(sizes[index].first), std::ref(sizes[index].second), timestamp_id.load(), std::ref(timestamp_id));
		}
	};
	StartLoad(loading_pics[0], tMap[0], 0, ART);
	StartLoad(loading_pics[1], tMap[1], 1, ART);
	StartLoad(loading_pics[2], tThumb, 2, THUMB);
	StartLoad(loading_pics[3], tCovers, 1, COVER);
}
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
void ImageManager::RefreshCovers() {
	irr::video::ITexture* tmp_cover = nullptr;
#undef GET
#define GET(obj,fun1,fun2) obj=fun1; if(!obj) obj=fun2;
#define X(x) BASE_PATH x
#define GET_TEXTURE_SIZED(obj,path) GET(tmp_cover,GetTextureFromFile(X( path".png"),sizes[1].first,sizes[1].second),GetTextureFromFile(X( path".jpg"),sizes[1].first,sizes[1].second))\
										if(tmp_cover) {\
											obj = tmp_cover;\
										}
	GET_TEXTURE_SIZED(tCover[0], "cover")
	GET_TEXTURE_SIZED(tCover[1], "cover2")
#undef X
#define X(x) (textures_path + EPRO_TEXT(x)).c_str()
	if(textures_path != path_string(BASE_PATH)) {
		GET(tmp_cover, GetTextureFromFile(X("cover.png"), sizes[1].first, sizes[1].second), GetTextureFromFile(X("cover.jpg"), sizes[1].first, sizes[1].second))
		if(tmp_cover){
			tCover[0] = tmp_cover;
		}
		GET(tmp_cover, GetTextureFromFile(X("cover2.png"), sizes[1].first, sizes[1].second), GetTextureFromFile(X("cover2.jpg"), sizes[1].first, sizes[1].second))
		if(tmp_cover){
			tCover[1] = tmp_cover;
		}
	}
#undef GET_TEXTURE_SIZED
#undef GET
#undef GTFF
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
irr::video::IImage* ImageManager::GetTextureImageFromFile(const irr::io::path& file, int width, int height, chrono_time timestamp_id, std::atomic<chrono_time>& source_timestamp_id, irr::video::IImage* archivefile) {
	irr::video::IImage* srcimg = nullptr;
	if(archivefile) {
		archivefile->grab();
		srcimg = archivefile;
	} else
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
ImageManager::image_path ImageManager::LoadCardTexture(uint32_t code, imgType type, std::atomic<irr::s32>& _width, std::atomic<irr::s32>& _height, chrono_time timestamp_id, std::atomic<chrono_time>& source_timestamp_id) {
	irr::video::IImage* img = nullptr;
	int width = _width;
	int height = _height;
	if(type == THUMB)
		type = ART;
	auto status = gImageDownloader->GetDownloadStatus(code, static_cast<ImageDownloader::imgType>(type));
	if(status == ImageDownloader::DOWNLOADED) {
		if(timestamp_id != source_timestamp_id.load())
			return std::make_pair(nullptr, EPRO_TEXT("fail"));
		if(width != _width || height != _height) {
			width = _width;
			height = _height;
		}
		auto file = gImageDownloader->GetDownloadPath(code, static_cast<ImageDownloader::imgType>(type));
		__repeat2:
		if((img = GetTextureImageFromFile(file.c_str(), width, height, timestamp_id, std::ref(source_timestamp_id), nullptr))) {
			if(timestamp_id != source_timestamp_id.load()) {
				img->drop();
				return std::make_pair(nullptr, EPRO_TEXT("fail"));
			}
			if(width != _width || height != _height) {
				img->drop();
				width = _width;
				height = _height;
				goto __repeat2;
			}
			return std::make_pair(img, file);
		}
		if(timestamp_id != source_timestamp_id.load()) {
			return std::make_pair(nullptr, EPRO_TEXT("fail"));
		}
	} else {
		for(auto& path : (type == ART) ? mainGame->pic_dirs : mainGame->cover_dirs) {
			for(auto extension : { EPRO_TEXT(".png"), EPRO_TEXT(".jpg") }) {
				if(timestamp_id != source_timestamp_id.load())
					return std::make_pair(nullptr, EPRO_TEXT("fail"));
				irr::video::IImage* readerimg = nullptr;
				path_string file;
				if(path == EPRO_TEXT("archives")) {
					auto lockedIrrFile = Utils::FindFileInArchives(
						(type == ART) ? EPRO_TEXT("pics/") : EPRO_TEXT("pics/cover/"),
						fmt::format(EPRO_TEXT("{}{}"), code, extension));
					if(!lockedIrrFile)
						continue;
					file = lockedIrrFile.reader->getFileName().c_str();
					readerimg = driver->createImageFromFile(lockedIrrFile.reader);
				} else {
					file = fmt::format(EPRO_TEXT("{}{}{}"), path, code, extension);
				}
				if(width != _width || height != _height) {
					width = _width;
					height = _height;
				}
			__repeat:
				if((img = GetTextureImageFromFile(file.c_str(), width, height, timestamp_id, std::ref(source_timestamp_id), readerimg))) {
					if(timestamp_id != source_timestamp_id.load()) {
						img->drop();
						if(readerimg) {
							readerimg->drop();
						}
						return std::make_pair(nullptr, EPRO_TEXT("fail"));
					}
					if(width != _width || height != _height) {
						img->drop();
						width = _width;
						height = _height;
						goto __repeat;
					}
					if(readerimg) {
						readerimg->drop();
					}
					return std::make_pair(img, file);
				}
				if(timestamp_id != source_timestamp_id.load()) {
					if(readerimg) {
						readerimg->drop();
					}
					return std::make_pair(nullptr, EPRO_TEXT("fail"));
				}
				if(readerimg) {
					readerimg->drop();
				}
			}
		}
	}
	if(status == ImageDownloader::NONE) {
		gImageDownloader->AddToDownloadQueue(code, static_cast<ImageDownloader::imgType>(type));
	}
	return std::make_pair(nullptr, EPRO_TEXT("wait for download"));
}
irr::video::ITexture* ImageManager::GetTextureCard(uint32_t code, imgType type, bool wait, bool fit, int* chk) {
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
			// Should never come to these last cases
			case FIELD:
			default:
				return tMap[0];
		}
	}();
	if(code == 0)
		return ret_unk;
	auto tit = map.find(code);
	if(tit == map.end()) {
		auto status = gImageDownloader->GetDownloadStatus(code, static_cast<ImageDownloader::imgType>(type));
		if(status == ImageDownloader::DOWNLOADING) {
			if(chk)
				*chk = 2;
			return ret_unk;
		}
		//pic will be loaded below instead
		/*if(status == ImageDownloader::DOWNLOADED) {
			map[code] = driver->getTexture(gImageDownloader->GetDownloadPath(code, static_cast<ImageDownloader::imgType>(type)).c_str());
			return map[code] ? map[code] : ret_unk;
		}*/
		if(status == ImageDownloader::DOWNLOAD_ERROR) {
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
irr::video::ITexture* ImageManager::GetTextureField(uint32_t code) {
	if(code == 0)
		return nullptr;
	auto tit = tFields.find(code);
	if(tit == tFields.end()) {
		auto status = gImageDownloader->GetDownloadStatus(code, ImageDownloader::FIELD);
		bool should_download = status == ImageDownloader::NONE;
		irr::video::ITexture* img = nullptr;
		if(!should_download) {
			if(status == ImageDownloader::DOWNLOADED) {
				img = driver->getTexture(gImageDownloader->GetDownloadPath(code, ImageDownloader::FIELD).c_str());
			} else
				return nullptr;
		} else {
			for(auto& path : mainGame->field_dirs) {
				for(auto extension : { EPRO_TEXT(".png"), EPRO_TEXT(".jpg") }) {
					if(path == EPRO_TEXT("archives")) {
						auto lockedIrrFile = Utils::FindFileInArchives(EPRO_TEXT("pics/field/"), fmt::format(EPRO_TEXT("{}{}"), code, extension));
						if (!lockedIrrFile)
							continue;
						if ((img = driver->getTexture(lockedIrrFile.reader)))
							break;
					} else {
						if((img = driver->getTexture(fmt::format(EPRO_TEXT("{}{}{}"), path, code, extension).c_str())))
							break;
					}
				}
			}
		}
		if(should_download && !img)
			gImageDownloader->AddToDownloadQueue(code, ImageDownloader::FIELD);
		else
			tFields[code] = img;
		return img;
	}
	return (tit->second) ? tit->second : nullptr;
}


/*
From minetest: Copyright (C) 2015 Aaron Suen <warr1024@gmail.com>
https://github.com/minetest/minetest/blob/5506e97ed897dde2d4820fe1b021a4622bae03b3/src/client/guiscalingfilter.cpp
originally under LGPL2.1+
*/



/* Fill in RGB values for transparent pixels, to correct for odd colors
 * appearing at borders when blending.  This is because many PNG optimizers
 * like to discard RGB values of transparent pixels, but when blending then
 * with non-transparent neighbors, their RGB values will shpw up nonetheless.
 *
 * This function modifies the original image in-place.
 *
 * Parameter "threshold" is the alpha level below which pixels are considered
 * transparent.  Should be 127 for 3d where alpha is threshold, but 0 for
 * 2d where alpha is blended.
 */
void imageCleanTransparent(irr::video::IImage *src, irr::u32 threshold) {
	irr::core::dimension2d<irr::u32> dim = src->getDimension();

	// Walk each pixel looking for fully transparent ones.
	// Note: loop y around x for better cache locality.
	for(irr::u32 ctry = 0; ctry < dim.Height; ctry++)
		for(irr::u32 ctrx = 0; ctrx < dim.Width; ctrx++) {

			// Ignore opaque pixels.
			irr::video::SColor c = src->getPixel(ctrx, ctry);
			if(c.getAlpha() > threshold)
				continue;

			// Sample size and total weighted r, g, b values.
			irr::u32 ss = 0, sr = 0, sg = 0, sb = 0;

			// Walk each neighbor pixel (clipped to image bounds).
			for(irr::u32 sy = (ctry < 1) ? 0 : (ctry - 1);
				sy <= (ctry + 1) && sy < dim.Height; sy++)
				for(irr::u32 sx = (ctrx < 1) ? 0 : (ctrx - 1);
					sx <= (ctrx + 1) && sx < dim.Width; sx++) {

				// Ignore transparent pixels.
				irr::video::SColor d = src->getPixel(sx, sy);
				if(d.getAlpha() <= threshold)
					continue;

				// Add RGB values weighted by alpha.
				irr::u32 a = d.getAlpha();
				ss += a;
				sr += a * d.getRed();
				sg += a * d.getGreen();
				sb += a * d.getBlue();
			}

			// If we found any neighbor RGB data, set pixel to average
			// weighted by alpha.
			if(ss > 0) {
				c.setRed(sr / ss);
				c.setGreen(sg / ss);
				c.setBlue(sb / ss);
				src->setPixel(ctrx, ctry, c);
			}
		}
}

/* Scale a region of an image into another image, using nearest-neighbor with
 * anti-aliasing; treat pixels as crisp rectangles, but blend them at boundaries
 * to prevent non-integer scaling ratio artifacts.  Note that this may cause
 * some blending at the edges where pixels don't line up perfectly, but this
 * filter is designed to produce the most accurate results for both upscaling
 * and downscaling.
 */
void imageScaleNNAAUnthreaded(irr::video::IImage *src, const irr::core::rect<irr::s32> &srcrect, irr::video::IImage *dest) {
	double sx, sy, minsx, maxsx, minsy, maxsy, area, ra, ga, ba, aa, pw, ph, pa;
	irr::u32 dy, dx;
	irr::video::SColor pxl;

	// Cache rectsngle boundaries.
	double sox = srcrect.UpperLeftCorner.X * 1.0;
	double soy = srcrect.UpperLeftCorner.Y * 1.0;
	double sw = srcrect.getWidth() * 1.0;
	double sh = srcrect.getHeight() * 1.0;

	// Walk each destination image pixel.
	// Note: loop y around x for better cache locality.
	irr::core::dimension2d<irr::u32> dim = dest->getDimension();
	for(dy = 0; dy < dim.Height; dy++)
		for(dx = 0; dx < dim.Width; dx++) {

			// Calculate floating-point source rectangle bounds.
			// Do some basic clipping, and for mirrored/flipped rects,
			// make sure min/max are in the right order.
			minsx = sox + (dx * sw / dim.Width);
			minsx = std::min(std::max(minsx, 0.0), sw + sox);
			maxsx = minsx + sw / dim.Width;
			maxsx = std::min(std::max(maxsx, 0.0), sw + sox);
			if(minsx > maxsx)
				std::swap(minsx, maxsx);
			minsy = soy + (dy * sh / dim.Height);
			minsy = std::min(std::max(minsy, 0.0), sh + soy);
			maxsy = minsy + sh / dim.Height;
			maxsy = std::min(std::max(maxsy, 0.0), sh + soy);
			if(minsy > maxsy)
				std::swap(minsy, maxsy);

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
}
#ifdef __ANDROID__
bool hasNPotSupport(irr::video::IVideoDriver* driver) {
	auto check = [](irr::video::IVideoDriver* driver)->bool {
		return driver->queryFeature(irr::video::EVDF_TEXTURE_NPOT);
	};
	static const bool supported = check(driver);
	return supported;
}
// Compute next-higher power of 2 efficiently, e.g. for power-of-2 texture sizes.
// Public Domain: https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
inline irr::u32 npot2(irr::u32 orig) {
	orig--;
	orig |= orig >> 1;
	orig |= orig >> 2;
	orig |= orig >> 4;
	orig |= orig >> 8;
	orig |= orig >> 16;
	return orig + 1;
}
#endif
/* Get a cached, high-quality pre-scaled texture for display purposes.  If the
 * texture is not already cached, attempt to create it.  Returns a pre-scaled texture,
 * or the original texture if unable to pre-scale it.
 */
irr::video::ITexture* ImageManager::guiScalingResizeCached(irr::video::ITexture *src, const irr::core::rect<irr::s32> &srcrect,
											const irr::core::rect<irr::s32> &destrect) {
	if(src == NULL)
		return src;

	// Calculate scaled texture name.
	irr::io::path rectstr = fmt::sprintf(EPRO_TEXT("%d:%d:%d:%d:%d:%d"),
						 srcrect.UpperLeftCorner.X,
						 srcrect.UpperLeftCorner.Y,
						 srcrect.getWidth(),
						 srcrect.getHeight(),
						 destrect.getWidth(),
						 destrect.getHeight()).c_str();
	irr::io::path origname = src->getName().getPath();
	irr::io::path scalename = origname + "@guiScalingFilter:" + rectstr;

	// Search for existing scaled texture.
	irr::video::ITexture *scaled = g_txrCache[scalename];
	if(scaled)
		return scaled;

	// Try to find the texture converted to an image in the cache.
	// If the image was not found, try to extract it from the texture.
	irr::video::IImage* srcimg = g_imgCache[origname];
	if(srcimg == NULL) {
		srcimg = driver->createImageFromData(src->getColorFormat(),
											 src->getSize(), src->lock(), false);
		src->unlock();
		g_imgCache[origname] = srcimg;
	}

	// Create a new destination image and scale the source into it.
	imageCleanTransparent(srcimg, 0);
	irr::video::IImage *destimg = driver->createImage(src->getColorFormat(),
													  irr::core::dimension2d<irr::u32>((irr::u32)destrect.getWidth(),
													 (irr::u32)destrect.getHeight()));
	imageScaleNNAAUnthreaded(srcimg, srcrect, destimg);

#ifdef __ANDROID__
	// Some platforms are picky about textures being powers of 2, so expand
	// the image dimensions to the next power of 2, if necessary.
	if(!hasNPotSupport(driver)) {
		irr::video::IImage *po2img = driver->createImage(src->getColorFormat(),
														 irr::core::dimension2d<irr::u32>(npot2((irr::u32)destrect.getWidth()),
																		   npot2((irr::u32)destrect.getHeight())));
		po2img->fill(irr::video::SColor(0, 0, 0, 0));
		destimg->copyTo(po2img);
		destimg->drop();
		destimg = po2img;
	}
#endif

	// Convert the scaled image back into a texture.
	scaled = driver->addTexture(scalename, destimg, NULL);
	destimg->drop();
	g_txrCache[scalename] = scaled;

	return scaled;
}
void ImageManager::draw2DImageFilterScaled(irr::video::ITexture* txr,
							 const irr::core::rect<irr::s32>& destrect, const irr::core::rect<irr::s32>& srcrect,
							 const irr::core::rect<irr::s32>* cliprect, const irr::video::SColor* const colors,
							 bool usealpha) {
	// Attempt to pre-scale image in software in high quality.
	irr::video::ITexture *scaled = guiScalingResizeCached(txr, srcrect, destrect);
	if(scaled == NULL)
		return;

	// Correct source rect based on scaled image.
	const irr::core::rect<irr::s32> mysrcrect = (scaled != txr)
		? irr::core::rect<irr::s32>(0, 0, destrect.getWidth(), destrect.getHeight())
		: srcrect;

	driver->draw2DImage(scaled, destrect, mysrcrect, cliprect, colors, usealpha);
}

}
