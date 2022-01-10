#include "game_config.h"
#include <fstream>
#include <curl/curl.h>
#include <fmt/format.h>
#include "utils.h"
#include <IImage.h>
#include <IVideoDriver.h>
#include <IrrlichtDevice.h>
#include <IReadFile.h>
#include "logging.h"
#include "image_manager.h"
#include "image_downloader.h"
#include "game.h"

static int constexpr IMAGES_LOAD_PER_FRAME_MAX = 50;

#define BASE_PATH EPRO_TEXT("./textures/")

namespace ygo {

#define X(x) (textures_path + EPRO_TEXT(x)).data()
#define GET(obj,fun1,fun2) do {obj=fun1; if(!obj) obj=fun2; def_##obj=obj;}while(0)
#define GTFF(path,ext,w,h) GetTextureFromFile(X(path ext), mainGame->Scale(w), mainGame->Scale(h))
#define GET_TEXTURE_SIZED(obj,path,w,h) GET(obj,GTFF(path,".png",w,h),GTFF(path,".jpg",w,h))
#define GET_TEXTURE(obj,path) GET(obj,driver->getTexture(X(path ".png")),driver->getTexture(X(path ".jpg")))
#define CHECK_RETURN(what, name) do { if(!what) { throw std::runtime_error("Couldn't load texture: " name); }} while(0)
ImageManager::ImageManager() {
	stop_threads = false;
	obj_clear_thread = std::thread(&ImageManager::ClearFutureObjects, this);
	for(auto& thread : load_threads)
		thread = std::thread(&ImageManager::LoadPic, this);
}
ImageManager::~ImageManager() {
	stop_threads = true;
	obj_clear_lock.lock();
	cv_clear.notify_all();
	obj_clear_lock.unlock();
	obj_clear_thread.join();
	pic_load.lock();
	cv_load.notify_all();
	pic_load.unlock();
	for(auto& thread : load_threads)
		thread.join();
	for(auto& it : g_imgCache) {
		if(it.second)
			it.second->drop();
	}
	for(auto& it : g_txrCache) {
		if(it.second)
			driver->removeTexture(it.second);
	}
}
bool ImageManager::Initial() {
	timestamp_id = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	textures_path = BASE_PATH;
	GET_TEXTURE_SIZED(tCover[0], "cover", CARD_IMG_WIDTH, CARD_IMG_HEIGHT);
	CHECK_RETURN(tCover[0], "cover");
	GET_TEXTURE_SIZED(tCover[1], "cover2", CARD_IMG_WIDTH, CARD_IMG_HEIGHT);
	if(!tCover[1]){
		tCover[1] = tCover[0];
		def_tCover[1] = tCover[1];
	}
	GET_TEXTURE_SIZED(tUnknown, "unknown", CARD_IMG_WIDTH, CARD_IMG_HEIGHT);
	CHECK_RETURN(tUnknown, "unknown");
	GET_TEXTURE(tAct, "act");
	CHECK_RETURN(tAct, "act");
	GET_TEXTURE(tAttack, "attack");
	CHECK_RETURN(tAttack, "attack");
	GET_TEXTURE(tChain, "chain");
	CHECK_RETURN(tChain, "chain");
	GET_TEXTURE_SIZED(tNegated, "negated", 128, 128);
	CHECK_RETURN(tNegated, "negated");
	GET_TEXTURE_SIZED(tNumber, "number", 320, 256);
	CHECK_RETURN(tNumber, "number");
	GET_TEXTURE(tLPBar, "lp");
	CHECK_RETURN(tLPBar, "lp");
	GET_TEXTURE(tLPFrame, "lpf");
	CHECK_RETURN(tLPFrame, "lpf");
	GET_TEXTURE_SIZED(tMask, "mask", 254, 254);
	CHECK_RETURN(tMask, "mask");
	GET_TEXTURE(tEquip, "equip");
	CHECK_RETURN(tEquip, "equip");
	GET_TEXTURE(tTarget, "target");
	CHECK_RETURN(tTarget, "target");
	GET_TEXTURE(tChainTarget, "chaintarget");
	CHECK_RETURN(tChainTarget, "chaintarget");
	GET_TEXTURE(tLim, "lim");
	CHECK_RETURN(tLim, "lim");
	GET_TEXTURE(tOT, "ot");
	CHECK_RETURN(tOT, "ot");
	GET_TEXTURE_SIZED(tHand[0], "f1", 89, 128);
	CHECK_RETURN(tHand[0], "f1");
	GET_TEXTURE_SIZED(tHand[1], "f2", 89, 128);
	CHECK_RETURN(tHand[1], "f2");
	GET_TEXTURE_SIZED(tHand[2], "f3", 89, 128);
	CHECK_RETURN(tHand[2], "f3");
	GET_TEXTURE(tBackGround, "bg");
	CHECK_RETURN(tBackGround, "bg");
	GET_TEXTURE(tBackGround_menu, "bg_menu");
	if(!tBackGround_menu){
		tBackGround_menu = tBackGround;
		def_tBackGround_menu = tBackGround;
	}
	GET_TEXTURE(tBackGround_deck, "bg_deck");
	if(!tBackGround_deck){
		tBackGround_deck = tBackGround;
		def_tBackGround_deck = tBackGround;
	}
	GET_TEXTURE(tField[0][0], "field2");
	CHECK_RETURN(tField[0][0], "field2");
	GET_TEXTURE(tFieldTransparent[0][0], "field-transparent2");
	CHECK_RETURN(tFieldTransparent[0][0], "field-transparent2");
	GET_TEXTURE(tField[0][1], "field3");
	CHECK_RETURN(tField[0][1], "field3");
	GET_TEXTURE(tFieldTransparent[0][1], "field-transparent3");
	CHECK_RETURN(tFieldTransparent[0][1], "field-transparent3");
	GET_TEXTURE(tField[0][2], "field");
	CHECK_RETURN(tField[0][2], "field");
	GET_TEXTURE(tFieldTransparent[0][2], "field-transparent");
	CHECK_RETURN(tFieldTransparent[0][2], "field-transparent");
	GET_TEXTURE(tField[0][3], "field4");
	CHECK_RETURN(tField[0][3], "field4");
	GET_TEXTURE(tFieldTransparent[0][3], "field-transparent4");
	CHECK_RETURN(tFieldTransparent[0][3], "field-transparent4");
	GET_TEXTURE(tField[1][0], "fieldSP2");
	CHECK_RETURN(tField[1][0], "fieldSP2");
	GET_TEXTURE(tFieldTransparent[1][0], "field-transparentSP2");
	CHECK_RETURN(tFieldTransparent[1][0], "field-transparentSP2");
	GET_TEXTURE(tField[1][1], "fieldSP3");
	CHECK_RETURN(tField[1][1], "fieldSP3");
	GET_TEXTURE(tFieldTransparent[1][1], "field-transparentSP3");
	CHECK_RETURN(tFieldTransparent[1][1], "field-transparentSP3");
	GET_TEXTURE(tField[1][2], "fieldSP");
	CHECK_RETURN(tField[1][2], "fieldSP");
	GET_TEXTURE(tFieldTransparent[1][2], "field-transparentSP");
	CHECK_RETURN(tFieldTransparent[1][2], "field-transparentSP");
	GET_TEXTURE(tField[1][3], "fieldSP4");
	CHECK_RETURN(tField[1][3], "fieldSP4");
	GET_TEXTURE(tFieldTransparent[1][3], "field-transparentSP4");
	CHECK_RETURN(tFieldTransparent[1][3], "field-transparentSP4");
	GET_TEXTURE(tSettings, "settings");
	CHECK_RETURN(tSettings, "settings");

	// Not required to be present
	GET_TEXTURE(tCheckBox[0], "checkbox_16");
	GET_TEXTURE(tCheckBox[1], "checkbox_32");
	GET_TEXTURE(tCheckBox[2], "checkbox_64");

	sizes[0].first = sizes[1].first = CARD_IMG_WIDTH * gGameConfig->dpi_scale;
	sizes[0].second = sizes[1].second = CARD_IMG_HEIGHT * gGameConfig->dpi_scale;
	sizes[2].first = CARD_THUMB_WIDTH * gGameConfig->dpi_scale;
	sizes[2].second = CARD_THUMB_HEIGHT * gGameConfig->dpi_scale;
	return true;
}

#undef GET
#undef GET_TEXTURE
#undef GET_TEXTURE_SIZED
#define GET(to_set,fun1,fun2,fallback) do  {\
	irr::video::ITexture* tmp = fun1;\
	if(!tmp)\
		tmp = fun2;\
	if(!tmp)\
		tmp = fallback;\
	if(to_set != fallback)\
		driver->removeTexture(to_set);\
	to_set = tmp;\
} while(0)
#define GET_TEXTURE_SIZED(obj,path,w,h) GET(obj,GTFF(path,".png",w,h),GTFF(path,".jpg",w,h),def_##obj)
#define GET_TEXTURE(obj,path) GET(obj,driver->getTexture(X(path ".png")),driver->getTexture(X(path ".jpg")),def_##obj)
void ImageManager::ChangeTextures(epro::path_stringview _path) {
	if(_path == textures_path)
		return;
	textures_path.assign(_path.data(), _path.size());
	const bool is_base = textures_path == BASE_PATH;
	GET_TEXTURE(tAct, "act");
	GET_TEXTURE(tAttack, "attack");
	GET_TEXTURE(tChain, "chain");
	GET_TEXTURE_SIZED(tNegated, "negated", 128, 128);
	GET_TEXTURE_SIZED(tNumber, "number", 320, 256);
	GET_TEXTURE(tLPBar, "lp");
	GET_TEXTURE(tLPFrame, "lpf");
	GET_TEXTURE_SIZED(tMask, "mask", 254, 254);
	GET_TEXTURE(tEquip, "equip");
	GET_TEXTURE(tTarget, "target");
	GET_TEXTURE(tChainTarget, "chaintarget");
	GET_TEXTURE(tLim, "lim");
	GET_TEXTURE(tOT, "ot");
	GET_TEXTURE_SIZED(tHand[0], "f1", 89, 128);
	GET_TEXTURE_SIZED(tHand[1], "f2", 89, 128);
	GET_TEXTURE_SIZED(tHand[2], "f3", 89, 128);
	GET_TEXTURE(tBackGround, "bg");
	GET_TEXTURE(tBackGround_menu, "bg_menu");
	if(!is_base && tBackGround != def_tBackGround && tBackGround_menu == def_tBackGround_menu)
		tBackGround_menu = tBackGround;
	GET_TEXTURE(tBackGround_deck, "bg_deck");
	if(!is_base && tBackGround != def_tBackGround && tBackGround_deck == def_tBackGround_deck)
		tBackGround_deck = tBackGround;
	GET_TEXTURE(tField[0][0], "field2");
	GET_TEXTURE(tFieldTransparent[0][0], "field-transparent2");
	GET_TEXTURE(tField[0][1], "field3");
	GET_TEXTURE(tFieldTransparent[0][1], "field-transparent3");
	GET_TEXTURE(tField[0][2], "field");
	GET_TEXTURE(tFieldTransparent[0][2], "field-transparent");
	GET_TEXTURE(tField[0][3], "field4");
	GET_TEXTURE(tFieldTransparent[0][3], "field-transparent4");
	GET_TEXTURE(tField[1][0], "fieldSP2");
	GET_TEXTURE(tFieldTransparent[1][0], "field-transparentSP2");
	GET_TEXTURE(tField[1][1], "fieldSP3");
	GET_TEXTURE(tFieldTransparent[1][1], "field-transparentSP3");
	GET_TEXTURE(tField[1][2], "fieldSP");
	GET_TEXTURE(tFieldTransparent[1][2], "field-transparentSP");
	GET_TEXTURE(tField[1][3], "fieldSP4");
	GET_TEXTURE(tFieldTransparent[1][3], "field-transparentSP4");
	GET_TEXTURE(tSettings, "settings");
	GET_TEXTURE(tCheckBox[0], "checkbox_16");
	GET_TEXTURE(tCheckBox[1], "checkbox_32");
	GET_TEXTURE(tCheckBox[2], "checkbox_64");
	RefreshCovers();
}
void ImageManager::ResetTextures() {
	ChangeTextures(BASE_PATH);
}
void ImageManager::SetDevice(irr::IrrlichtDevice* dev) {
	device = dev;
	driver = dev->getVideoDriver();
}
void ImageManager::ClearTexture(bool resize) {
	auto ClearMap = [&](texture_map &map) {
		for(const auto& tit : map) {
			if(tit.second.texture) {
				driver->removeTexture(tit.second.texture);
			}
		}
		map.clear();
	};
	if(resize) {
		sizes[1].first = CARD_IMG_WIDTH * mainGame->window_scale.X * gGameConfig->dpi_scale;
		sizes[1].second = CARD_IMG_HEIGHT * mainGame->window_scale.Y * gGameConfig->dpi_scale;
		sizes[2].first = CARD_THUMB_WIDTH * mainGame->window_scale.X * gGameConfig->dpi_scale;
		sizes[2].second = CARD_THUMB_HEIGHT * mainGame->window_scale.Y * gGameConfig->dpi_scale;
		RefreshCovers();
	} else
		ClearCachedTextures();
	ClearMap(tMap[0]);
	ClearMap(tMap[1]);
	ClearMap(tThumb);
	ClearMap(tCovers);
	for(const auto& tit : tFields) {
		if(tit.second) {
			driver->removeTexture(tit.second);
		}
	}
	tFields.clear();
}
#undef GET_TEXTURE
#undef GET_TEXTURE_SIZED
#undef X
void ImageManager::RefreshCachedTextures() {
	auto LoadTexture = [this](int index, texture_map& dest, auto& size, imgType type) {
		auto& src = loaded_pics[index];
		std::vector<uint32_t> readd;
		for(int i = 0; i < IMAGES_LOAD_PER_FRAME_MAX; i++) {
			std::unique_lock<std::mutex> lck(pic_load);
			if(src.empty())
				break;
			auto loaded = std::move(src.front());
			src.pop_front();
			lck.unlock();
			auto& map_elem = dest[loaded.code];
			if(loaded.status == loadStatus::WAIT_DOWNLOAD) {
				map_elem.preload_status = preloadStatus::WAIT_DOWNLOAD;
				continue;
			}
			auto& ret_texture = map_elem.texture;
			map_elem.preload_status = preloadStatus::LOADED;
			if(loaded.status == loadStatus::LOAD_FAIL) {
				ret_texture = nullptr;
				continue;
			}
			auto* texture = loaded.texture;
			if(texture->getDimension().Width != size.first || texture->getDimension().Height != size.second) {
				readd.push_back(loaded.code);
				ret_texture = nullptr;
				continue;
			}
			ret_texture = driver->addTexture({ loaded.path.data(), static_cast<irr::u32>(loaded.path.size()) }, texture);
			texture->drop();
		}
		if(readd.size()) {
			std::unique_lock<std::mutex> lck(pic_load);
			for(auto& code : readd)
				to_load.emplace_front(code, type, index, std::ref(size.first), std::ref(size.second), timestamp_id, std::ref(timestamp_id));
			cv_load.notify_all();
		}
	};
	LoadTexture(0, tMap[0], sizes[0], imgType::ART);
	LoadTexture(1, tMap[1], sizes[1], imgType::ART);
	LoadTexture(2, tThumb, sizes[2], imgType::THUMB);
	LoadTexture(3, tCovers, sizes[1], imgType::COVER);
}
void ImageManager::ClearFutureObjects() {
	Utils::SetThreadName("ImgObjsClear");
	while(!stop_threads) {
		std::unique_lock<std::mutex> lck(obj_clear_lock);
		while(to_clear.empty()) {
			cv_clear.wait(lck);
			if(stop_threads)
				return;
		}
		auto img = std::move(to_clear.front());
		to_clear.pop_front();
		lck.unlock();
		if(img.texture)
			img.texture->drop();
	}
}
void ImageManager::RefreshCovers() {
	irr::video::ITexture* tmp_cover = nullptr;
#undef GET
#define GET(obj,fun1,fun2) do {obj=fun1; if(!obj) obj=fun2;} while(0)
#define X(x) BASE_PATH x
#define GET_TEXTURE_SIZED(obj,path) do {GET(tmp_cover,GetTextureFromFile(X( path".png"),sizes[1].first,sizes[1].second),GetTextureFromFile(X( path".jpg"),sizes[1].first,sizes[1].second));\
										if(tmp_cover) {\
											driver->removeTexture(obj); \
											obj = tmp_cover;\
										}} while(0)
	GET_TEXTURE_SIZED(tCover[0], "cover");
	tCover[1] = nullptr;
	GET_TEXTURE_SIZED(tCover[1], "cover2");
	if(!tCover[1]) {
		tCover[1] = tCover[0];
		def_tCover[1] = tCover[1];
	}
	GET_TEXTURE_SIZED(tUnknown, "unknown");
#undef X
#define X(x) (textures_path + EPRO_TEXT(x)).data()
	if(textures_path != BASE_PATH) {
		GET(tmp_cover, GetTextureFromFile(X("cover.png"), sizes[1].first, sizes[1].second), GetTextureFromFile(X("cover.jpg"), sizes[1].first, sizes[1].second));
		if(tmp_cover){
			driver->removeTexture(tCover[0]);
			tCover[0] = tmp_cover;
		}
		GET(tmp_cover, GetTextureFromFile(X("cover2.png"), sizes[1].first, sizes[1].second), GetTextureFromFile(X("cover2.jpg"), sizes[1].first, sizes[1].second));
		if(tmp_cover){
			driver->removeTexture(tCover[1]);
			tCover[1] = tmp_cover;
		}
		GET(tmp_cover, GetTextureFromFile(X("unknown.png"), sizes[1].first, sizes[1].second), GetTextureFromFile(X("unknown.jpg"), sizes[1].first, sizes[1].second));
		if(tmp_cover){
			driver->removeTexture(tUnknown);
			tUnknown = tmp_cover;
		}
	}
#undef GET_TEXTURE_SIZED
#undef GET
#undef GTFF
}
void ImageManager::LoadPic() {
	Utils::SetThreadName("PicLoader");
	while(!stop_threads) {
		std::unique_lock<std::mutex> lck(pic_load);
		while(to_load.empty()) {
			cv_load.wait(lck);
			if(stop_threads) {
				return;
			}
		}
		auto loaded = std::move(to_load.front());
		to_load.pop_front();
		lck.unlock();
		auto load_status = LoadCardTexture(loaded.code, loaded.type, loaded.reference_width, loaded.reference_height, loaded.timestamp, loaded.reference_timestamp);
		lck.lock();
		loaded_pics[loaded.index].push_front(std::move(load_status));
	}
}
void ImageManager::ClearCachedTextures() {
	timestamp_id = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	std::unique_lock<std::mutex> lck(obj_clear_lock);
	{
		std::unique_lock<std::mutex> lck2(pic_load);
		for(auto& map : loaded_pics) {
			to_clear.insert(to_clear.end(), std::make_move_iterator(map.begin()), std::make_move_iterator(map.end()));
			map.clear();
		}
		to_load.clear();
	}
	cv_clear.notify_one();
}
// function by Warr1024, from https://github.com/minetest/minetest/issues/2419 , modified
bool ImageManager::imageScaleNNAA(irr::video::IImage* src, irr::video::IImage* dest, irr::s32 width, irr::s32 height, chrono_time timestamp_id, const std::atomic<chrono_time>& source_timestamp_id) {
	// Cache rectsngle boundaries.
	auto& sdim = src->getDimension();
	const double sw = sdim.Width;
	const double sh = sdim.Height;

	// Walk each destination image pixel.
	// Note: loop y around x for better cache locality.
	const auto& dim = dest->getDimension();
	const auto divw = sw / dim.Width;
	const auto divh = sh / dim.Height;
	irr::u32 dy = 0;
	for(; dy < dim.Height && timestamp_id == source_timestamp_id; dy++) {
		for(irr::u32 dx = 0; dx < dim.Width; dx++) {
			// Calculate floating-point source rectangle bounds.
			const double minsx = dx * divw;
			const double maxsx = minsx + divw;
			const double minsy = dy * divh;
			const double maxsy = minsy + divh;

			// Total area, and integral of r, g, b values over that area,
			// initialized to zero, to be summed up in next loops.
			double area = 0, ra = 0, ga = 0, ba = 0, aa = 0;
			irr::video::SColor pxl;
			const auto csy = std::floor(minsy);
			const auto csx = std::floor(minsx);
			// Loop over the integral pixel positions described by those bounds.
			for(double sy = csy; sy < maxsy; sy++)
				for(double sx = csx; sx < maxsx; sx++) {
					// Calculate width, height, then area of dest pixel
					// that's covered by this source pixel.
					double pw = 1;
					if(minsx > sx)
						pw += sx - minsx;
					if(maxsx < (sx + 1))
						pw += maxsx - sx - 1;
					double ph = 1;
					if(minsy > sy)
						ph += sy - minsy;
					if(maxsy < (sy + 1))
						ph += maxsy - sy - 1;
					const double pa = pw * ph;

					// Get source pixel and add it to totals, weighted
					// by covered area and alpha.
					pxl = src->getPixel(sx, sy);
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
	return dy == dim.Height;
}
irr::video::IImage* ImageManager::GetScaledImage(irr::video::IImage* srcimg, int width, int height, chrono_time timestamp_id, const std::atomic<chrono_time>& source_timestamp_id) {
	if(width <= 0 || height <= 0)
		return nullptr;
	if(!srcimg || timestamp_id != source_timestamp_id.load())
		return nullptr;
	const irr::core::dimension2d<irr::u32> dim(width, height);
	if(srcimg->getDimension() == dim) {
		srcimg->grab();
		return srcimg;
	} else {
		irr::video::IImage* destimg = driver->createImage(srcimg->getColorFormat(), dim);
		if(timestamp_id != source_timestamp_id || !imageScaleNNAA(srcimg, destimg, width, height, timestamp_id, source_timestamp_id)) {
			destimg->drop();
			destimg = nullptr;
		}
		return destimg;
	}
}
irr::video::ITexture* ImageManager::GetTextureFromFile(const irr::io::path& file, int width, int height) {
	auto img = GetScaledImageFromFile(file, width, height);
	if(img) {
		auto texture = driver->addTexture(file, img);
		img->drop();
		if(texture)
			return texture;
	}
	return driver->getTexture(file);
}
ImageManager::load_return ImageManager::LoadCardTexture(uint32_t code, imgType type, const std::atomic<irr::s32>& _width, const std::atomic<irr::s32>& _height, chrono_time timestamp_id, const std::atomic<chrono_time>& source_timestamp_id) {
	int width = _width;
	int height = _height;
	if(type == imgType::THUMB)
		type = imgType::ART;
	load_return ret{ loadStatus::LOAD_FAIL, code };
	auto LoadImg = [&](irr::video::IImage* base_img)->irr::video::IImage* {
		if(!base_img)
			return nullptr;
		if(width != _width || height != _height) {
			width = _width;
			height = _height;
		}
		while(const auto img = GetScaledImage(base_img, width, height, timestamp_id, source_timestamp_id)) {
			if(timestamp_id != source_timestamp_id.load()) {
				img->drop();
				base_img->drop();
				return nullptr;
			}
			if(width != _width || height != _height) {
				img->drop();
				width = _width;
				height = _height;
				continue;
			}
			base_img->drop();
			return img;
		}
		base_img->drop();
		return nullptr;
	};

	irr::video::IImage* img;

	auto status = gImageDownloader->GetDownloadStatus(code, type);
	if(status == ImageDownloader::downloadStatus::DOWNLOADED) {
		if(timestamp_id != source_timestamp_id.load())
			return ret;
		const auto file = gImageDownloader->GetDownloadPath(code, type);
		if((img = LoadImg(driver->createImageFromFile({ file.data(), static_cast<irr::u32>(file.size()) }))) != nullptr) {
			ret.status = loadStatus::LOAD_OK;
			ret.path = epro::path_string{ file };
			ret.texture = img;
		}
		return ret;
	} else if(status == ImageDownloader::downloadStatus::NONE) {
		for(auto& path : (type == imgType::ART) ? mainGame->pic_dirs : mainGame->cover_dirs) {
			for(auto extension : { EPRO_TEXT(".png"), EPRO_TEXT(".jpg") }) {
				if(timestamp_id != source_timestamp_id.load())
					return ret;
				irr::video::IImage* base_img = nullptr;
				epro::path_string file;
				if(path == EPRO_TEXT("archives")) {
					auto archiveFile = Utils::FindFileInArchives(
						(type == imgType::ART) ? EPRO_TEXT("pics/") : EPRO_TEXT("pics/cover/"),
						fmt::format(EPRO_TEXT("{}{}"), code, extension));
					if(!archiveFile)
						continue;
					const auto& name = archiveFile->getFileName();
					file = { name.c_str(), name.size() };
					base_img = driver->createImageFromFile(archiveFile);
					archiveFile->drop();
				} else {
					file = fmt::format(EPRO_TEXT("{}{}{}"), path, code, extension);
					base_img = driver->createImageFromFile({ file.data(), static_cast<irr::u32>(file.size()) });
				}
				if((img = LoadImg(base_img)) != nullptr) {
					ret.status = loadStatus::LOAD_OK;
					ret.path = file;
					ret.texture = img;
					return ret;
				}
			}
		}
		gImageDownloader->AddToDownloadQueue(code, type);
		ret.status = loadStatus::WAIT_DOWNLOAD;
		return ret;
	}
	return ret;
}
irr::video::ITexture* ImageManager::GetTextureCard(uint32_t code, imgType type, bool wait, bool fit, int* chk) {
	if(chk)
		*chk = 1;
	irr::video::ITexture* ret_unk = tUnknown;
	int index;
	int size_index;
	auto& map = [&]()->texture_map& {
		switch(type) {
			case imgType::ART: {
				index = fit ? 1 : 0;
				size_index = index;
				return tMap[fit ? 1 : 0];
			}
			case imgType::THUMB: {
				index = 2;
				size_index = index;
				return tThumb;
			}
			case imgType::COVER: {
				ret_unk = tCover[0];
				index = 3;
				size_index = 0;
				return tCovers;
			}
			default:
				unreachable();
		}
	}();
	if(code == 0)
		return ret_unk;
	auto& elem = map[code];
	if(elem.preload_status != preloadStatus::LOADED) {
		auto status = gImageDownloader->GetDownloadStatus(code, type);
		if(status == ImageDownloader::downloadStatus::DOWNLOADING) {
			if(chk)
				*chk = 2;
			return ret_unk;
		}
		//pic will be loaded below instead
		/*if(status == ImageDownloader::DOWNLOADED) {
			map[code] = driver->getTexture(gImageDownloader->GetDownloadPath(code, type).data());
			return map[code] ? map[code] : ret_unk;
		}*/
		if(status == ImageDownloader::downloadStatus::DOWNLOAD_ERROR) {
			map[code].texture = nullptr;
			return ret_unk;
		}
		if(chk)
			*chk = 2;
		if(elem.preload_status == preloadStatus::NONE || (elem.preload_status == preloadStatus::WAIT_DOWNLOAD && status == ImageDownloader::downloadStatus::DOWNLOADED)) {
			elem.preload_status = preloadStatus::LOADING;
			if(wait) {
				auto load_result = LoadCardTexture(code, type, sizes[size_index].first, sizes[size_index].second, timestamp_id, timestamp_id);
				auto& rmap = map[code].texture;
				if(load_result.status == loadStatus::LOAD_OK) {
					rmap = driver->addTexture(load_result.path.data(), load_result.texture);
					load_result.texture->drop();
					if(chk)
						*chk = 1;
				} else {
					rmap = nullptr;
					if(chk)
						*chk = 0;
				}
				return (rmap) ? rmap : ret_unk;
			} else {
				std::unique_lock<std::mutex> lck(pic_load);
				to_load.emplace_front(code, type, index, std::ref(sizes[size_index].first), std::ref(sizes[size_index].second), timestamp_id.load(), std::ref(timestamp_id));
				cv_load.notify_one();
			}
		}
		return ret_unk;
	}
	auto* texture = elem.texture;
	if(chk && texture == nullptr)
		*chk = 0;
	if(texture)
		return texture;
	return ret_unk;
}
irr::video::ITexture* ImageManager::GetTextureField(uint32_t code) {
	if(code == 0)
		return nullptr;
	auto tit = tFields.find(code);
	if(tit != tFields.end())
		return tit->second;
	auto status = gImageDownloader->GetDownloadStatus(code, imgType::FIELD);
	if(status != ImageDownloader::downloadStatus::NONE) {
		if(status == ImageDownloader::downloadStatus::DOWNLOADED) {
			const auto path = gImageDownloader->GetDownloadPath(code, imgType::FIELD);
			auto downloaded = driver->getTexture({ path.data(), static_cast<irr::u32>(path.size()) });
			tFields.emplace(code, downloaded);
			return downloaded;
		}
		return nullptr;
	}
	for(auto& path : mainGame->field_dirs) {
		for(auto extension : { EPRO_TEXT(".png"), EPRO_TEXT(".jpg") }) {
			irr::video::ITexture* img;
			if(path == EPRO_TEXT("archives")) {
				auto archiveFile = Utils::FindFileInArchives(EPRO_TEXT("pics/field/"), fmt::format(EPRO_TEXT("{}{}"), code, extension));
				if(!archiveFile)
					continue;
				img = driver->getTexture(archiveFile);
				archiveFile->drop();
			} else
				img = driver->getTexture(fmt::format(EPRO_TEXT("{}{}{}"), path, code, extension).data());
			if(img) {
				tFields.emplace(code, img);
				return img;
			}
		}
	}
	gImageDownloader->AddToDownloadQueue(code, imgType::FIELD);
	return nullptr;
}

irr::video::ITexture* ImageManager::GetCheckboxScaledTexture(float scale) {
	if(scale > 3.5f && tCheckBox[2])
			return tCheckBox[2];
	if(scale > 2.0f && tCheckBox[1])
		return tCheckBox[1];
	return tCheckBox[0];
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
static void imageCleanTransparent(irr::video::IImage* src, irr::u32 threshold) {
	const auto& dim = src->getDimension();

	// Walk each pixel looking for fully transparent ones.
	// Note: loop y around x for better cache locality.
	for(irr::u32 ctry = 0; ctry < dim.Height; ctry++)
		for(irr::u32 ctrx = 0; ctrx < dim.Width; ctrx++) {

			// Ignore opaque pixels.
			auto c = src->getPixel(ctrx, ctry);
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
				const auto d = src->getPixel(sx, sy);
				if(d.getAlpha() <= threshold)
					continue;

				// Add RGB values weighted by alpha.
				const auto a = d.getAlpha();
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
static void imageScaleNNAAUnthreaded(irr::video::IImage* src, const irr::core::rect<irr::s32>& srcrect, irr::video::IImage* dest) {
	// Cache rectangle boundaries.
	const double sox = srcrect.UpperLeftCorner.X;
	const double soy = srcrect.UpperLeftCorner.Y;
	const double sw = srcrect.getWidth();
	const double sh = srcrect.getHeight();

	// Walk each destination image pixel.
	// Note: loop y around x for better cache locality.
	const auto& dim = dest->getDimension();
	const auto divw = sw / dim.Width;
	const auto divh = sh / dim.Height;
	for(irr::u32 dy = 0; dy < dim.Height; dy++)
		for(irr::u32 dx = 0; dx < dim.Width; dx++) {

			// Calculate floating-point source rectangle bounds.
			// Do some basic clipping, and for mirrored/flipped rects,
			// make sure min/max are in the right order.
			auto minsx = std::min(std::max(sox + (dx * divw), 0.0), sw + sox);
			auto maxsx = std::min(std::max(minsx + divw, 0.0), sw + sox);
			if(minsx > maxsx)
				std::swap(minsx, maxsx);
			auto minsy = std::min(std::max(soy + (dy * divh), 0.0), sh + soy);
			auto maxsy = std::min(std::max(minsy + divh, 0.0), sh + soy);
			if(minsy > maxsy)
				std::swap(minsy, maxsy);

			const auto csy = std::floor(minsy);
			const auto csx = std::floor(minsx);

			// Total area, and integral of r, g, b values over that area,
			// initialized to zero, to be summed up in next loops.
			double area = 0, ra = 0, ga = 0, ba = 0, aa = 0;
			irr::video::SColor pxl;

			// Loop over the integral pixel positions described by those bounds.
			for(double sy = csy; sy < maxsy; sy++)
				for(double sx = csx; sx < maxsx; sx++) {
					// Calculate width, height, then area of dest pixel
					// that's covered by this source pixel.

					double pw = 1.0;
					if(minsx > sx)
						pw += sx - minsx;
					if(maxsx < (sx + 1))
						pw += maxsx - sx - 1;
					double ph = 1.0;
					if(minsy > sy)
						ph += sy - minsy;
					if(maxsy < (sy + 1))
						ph += maxsy - sy - 1;
					const double pa = pw * ph;

					// Get source pixel and add it to totals, weighted
					// by covered area and alpha.
					pxl = src->getPixel(sx, sy);
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
static bool hasNPotSupport(irr::video::IVideoDriver* driver) {
	static const auto supported = [driver] {
		return driver->queryFeature(irr::video::EVDF_TEXTURE_NPOT);
	}();
	return supported;
}
// Compute next-higher power of 2 efficiently, e.g. for power-of-2 texture sizes.
// Public Domain: https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
static inline irr::u32 npot2(irr::u32 orig) {
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
irr::video::ITexture* ImageManager::guiScalingResizeCached(irr::video::ITexture* src, const irr::core::rect<irr::s32> &srcrect,
											const irr::core::rect<irr::s32> &destrect) {
	if(!src)
		return src;

	const auto& origname = src->getName().getPath();
	// Calculate scaled texture name.
	const auto scale_name = fmt::format(EPRO_TEXT("{}@guiScalingFilter:{}:{}:{}:{}:{}:{}"),
						 origname,
						 srcrect.UpperLeftCorner.X,
						 srcrect.UpperLeftCorner.Y,
						 srcrect.getWidth(),
						 srcrect.getHeight(),
						 destrect.getWidth(),
						 destrect.getHeight());

	// Search for existing scaled texture.
	irr::video::ITexture*& scaled = g_txrCache[scale_name];
	if(scaled)
		return scaled;

	// Try to find the texture converted to an image in the cache.
	// If the image was not found, try to extract it from the texture.
	irr::video::IImage* srcimg = g_imgCache[origname];
	if(!srcimg) {
		srcimg = driver->createImageFromData(src->getColorFormat(),
											 src->getSize(), src->lock(), false);
		src->unlock();
		g_imgCache[origname] = srcimg;
	}

	// Create a new destination image and scale the source into it.
	imageCleanTransparent(srcimg, 0);
	irr::video::IImage* destimg = driver->createImage(src->getColorFormat(),
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
	scaled = driver->addTexture({ scale_name.data(), static_cast<irr::u32>(scale_name.size()) }, destimg);
	destimg->drop();

	return scaled;
}
void ImageManager::draw2DImageFilterScaled(irr::video::ITexture* txr,
							 const irr::core::rect<irr::s32>& destrect, const irr::core::rect<irr::s32>& srcrect,
							 const irr::core::rect<irr::s32>* cliprect, const irr::video::SColor* const colors,
							 bool usealpha) {
	// Attempt to pre-scale image in software in high quality.
	irr::video::ITexture* scaled = guiScalingResizeCached(txr, srcrect, destrect);
	if(!scaled)
		return;

	// Correct source rect based on scaled image.
	const auto mysrcrect = (scaled != txr)
		? irr::core::rect<irr::s32>(0, 0, destrect.getWidth(), destrect.getHeight())
		: srcrect;

	driver->draw2DImage(scaled, destrect, mysrcrect, cliprect, colors, usealpha);
}
irr::video::IImage* ImageManager::GetScaledImageFromFile(const irr::io::path& file, int width, int height) {
	if(width <= 0 || height <= 0)
		return nullptr;

	auto* srcimg = driver->createImageFromFile(file);
	if(!srcimg)
		return nullptr;

	const irr::core::dimension2d<irr::u32> dim(width, height);
	const auto& srcdim = srcimg->getDimension();
	if(srcdim == dim) {
		return srcimg;
	} else {
		auto* destimg = driver->createImage(srcimg->getColorFormat(), dim);
		imageScaleNNAAUnthreaded(srcimg, { 0, 0, (irr::s32)srcdim.Width, (irr::s32)srcdim.Height }, destimg);
		srcimg->drop();
		return destimg;
	}
}

}
