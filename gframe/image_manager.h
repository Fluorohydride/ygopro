#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include "config.h"
#include "data_manager.h"
#include <unordered_map>
#include <atomic>
#include <future>

namespace ygo {

struct PicSource {
	std::string url;
	std::string type;
};

class ImageManager {
private:
	using image_path = std::pair<IImage*, std::string>;
	using loading_map = std::map<int, std::future<image_path>>;
	using chrono_time = unsigned long long;
	using downloading_map = std::map<int/*code*/, std::pair<int /*status: value not found: not yet downloaded, 0: downloading, 1: not correctly downloaded, 2: correctly downloaded*/, std::string>>;
public:
	ImageManager() {
		loading_pics[0] = new loading_map();
		loading_pics[1] = new loading_map();
		loading_pics[2] = new loading_map();
	}
	~ImageManager() {
		delete loading_pics[0];
		delete loading_pics[1];
		delete loading_pics[2];
	}
	void AddDownloadResource(PicSource src);
	bool Initial();
	void SetDevice(irr::IrrlichtDevice* dev);
	void ClearTexture(bool resize = false);
	void RemoveTexture(int code);
	void RefreshCachedTextures();
	void ClearCachedTextures(bool resize);
	bool imageScaleNNAA(irr::video::IImage *src, irr::video::IImage *dest, s32 width, s32 height, chrono_time timestamp_id, std::atomic<chrono_time>& source_timestamp_id);
	irr::video::IImage* GetTextureFromFile(const io::path& file, int width, int height, chrono_time timestamp_id, std::atomic<chrono_time>& source_timestamp_id);
	irr::video::ITexture* GetTexture(int code, bool wait = false, bool fit = false, int* chk = nullptr);
	irr::video::ITexture* GetTextureThumb(int code, bool wait = false, int* chk = nullptr);
	irr::video::ITexture* GetTextureField(int code);

	std::unordered_map<int, irr::video::ITexture*> tMap[2];
	std::unordered_map<int, irr::video::ITexture*> tThumb;
	std::unordered_map<int, irr::video::ITexture*> tFields;
	irr::IrrlichtDevice* device;
	irr::video::IVideoDriver* driver;
	irr::video::ITexture* tCover[2];
	irr::video::ITexture* tUnknown;
	irr::video::ITexture* tAct;
	irr::video::ITexture* tAttack;
	irr::video::ITexture* tNegated;
	irr::video::ITexture* tChain;
	irr::video::ITexture* tNumber;
	irr::video::ITexture* tLPFrame;
	irr::video::ITexture* tLPBar;
	irr::video::ITexture* tMask;
	irr::video::ITexture* tEquip;
	irr::video::ITexture* tTarget;
	irr::video::ITexture* tChainTarget;
	irr::video::ITexture* tLim;
	irr::video::ITexture* tHand[3];
	irr::video::ITexture* tBackGround;
	irr::video::ITexture* tBackGround_menu;
	irr::video::ITexture* tBackGround_deck;
	irr::video::ITexture* tField[2][4];
	irr::video::ITexture* tFieldTransparent[2][4];
private:
	void ClearFutureObjects(loading_map* map);
	void DownloadPic(int code);
	void DownloadField(int code);
	image_path LoadCardTexture(int code, std::atomic<s32>& width, std::atomic<s32>& height, chrono_time timestamp_id, std::atomic<chrono_time>& source_timestamp_id);
	loading_map* loading_pics[3];
	downloading_map downloading_pics;
	downloading_map downloading_fields;
	std::pair<std::atomic<s32>, std::atomic<s32>> sizes[3];
	std::mutex pic_download;
	std::mutex field_download;
	std::atomic<chrono_time> timestamp_id;
	std::vector<PicSource> pic_urls;
};

extern ImageManager imageManager;

}

#endif // IMAGEMANAGER_H
