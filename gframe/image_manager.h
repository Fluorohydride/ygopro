#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include "config.h"
#include <path.h>
#include <unordered_map>
#include <atomic>
#include <future>
#include <queue>

namespace irr {
class IrrlichtDevice;
namespace io {
class IReadFile;
}
namespace video {
class IImage;
class ITexture;
class IVideoDriver;
class SColor;
}
}

namespace ygo {

class ImageManager {
public:
	enum imgType {
		ART,
		FIELD,
		COVER,
		THUMB
	};
	struct PicSource {
		std::string url;
		imgType type;
	};
private:
	using image_path = std::pair<irr::video::IImage*, path_string>;
	using loading_map = std::map<int, std::future<image_path>>;
	using chrono_time = unsigned long long;
	enum downloadStatus {
		DOWNLOADING,
		DOWNLOAD_ERROR,
		DOWNLOADED,
		NONE
	};
	struct downloadParam {
		int code;
		imgType type;
		downloadStatus status;
		path_string path;
	};
	using downloading_map = std::map<int/*code*/, downloadParam>; /*if the value is not found, the download hasn't started yet*/
	using texture_map = std::unordered_map<int, irr::video::ITexture*>;
public:
	ImageManager() {
		loading_pics[0] = new loading_map();
		loading_pics[1] = new loading_map();
		loading_pics[2] = new loading_map();
		loading_pics[3] = new loading_map();
	}
	~ImageManager() {
		stop_threads = true;
		cv.notify_all();
		for(int i = 0; i < 8; i++) {
			download_threads[i].join();
		}
		delete loading_pics[0];
		delete loading_pics[1];
		delete loading_pics[2];
		delete loading_pics[3];
	}
	void AddDownloadResource(PicSource src);
	bool Initial();
	void ChangeTextures(const path_string& path);
	void ResetTextures();
	void SetDevice(irr::IrrlichtDevice* dev);
	void ClearTexture(bool resize = false);
	void RemoveTexture(int code);
	void RefreshCachedTextures();
	void ClearCachedTextures(bool resize);
	bool imageScaleNNAA(irr::video::IImage *src, irr::video::IImage* dest, irr::s32 width, irr::s32 height, chrono_time timestamp_id, std::atomic<chrono_time>& source_timestamp_id);
	irr::video::IImage* GetTextureImageFromFile(const irr::io::path& file, int width, int height, chrono_time timestamp_id, std::atomic<chrono_time>& source_timestamp_id, irr::io::IReadFile* archivefile = nullptr);
	irr::video::ITexture* GetTextureFromFile(const irr::io::path& file, int width, int height);
	irr::video::ITexture* GetTextureCard(int code, imgType type, bool wait = false, bool fit = false, int* chk = nullptr);
	//irr::video::ITexture* GetTextureThumb(int code, bool wait = false, int* chk = nullptr);
	irr::video::ITexture* GetTextureField(int code);
	//irr::video::ITexture* GetTextureCustomCover(int code, bool wait = false, int* chk = nullptr);
	irr::video::ITexture* guiScalingResizeCached(irr::video::ITexture *src, const irr::core::rect<irr::s32> &srcrect,
												 const irr::core::rect<irr::s32> &destrect);
	void draw2DImageFilterScaled(irr::video::ITexture* txr,
								 const irr::core::rect<irr::s32>& destrect, const irr::core::rect<irr::s32>& srcrect,
								 const irr::core::rect<irr::s32>* cliprect = nullptr, const irr::video::SColor* const colors = nullptr,
								 bool usealpha = false);

	texture_map tMap[2];
	texture_map tThumb;
	texture_map tFields;
	texture_map tCovers;
	irr::IrrlichtDevice* device;
	irr::video::IVideoDriver* driver;
#define A(what) irr::video::ITexture* what;
	A(tCover[2])
	A(tUnknown)
	A(tAct)
	A(tAttack)
	A(tNegated)
	A(tChain)
	A(tNumber)
	A(tLPFrame)
	A(tLPBar)
	A(tMask)
	A(tEquip)
	A(tTarget)
	A(tChainTarget)
	A(tLim)
	A(tHand[3])
	A(tBackGround)
	A(tBackGround_menu)
	A(tBackGround_deck)
	A(tField[2][4])
	A(tFieldTransparent[2][4])
#undef A
private:
#define A(what) irr::video::ITexture* def_##what;
	A(tCover[2])
	A(tUnknown)
	A(tAct)
	A(tAttack)
	A(tNegated)
	A(tChain)
	A(tNumber)
	A(tLPFrame)
	A(tLPBar)
	A(tMask)
	A(tEquip)
	A(tTarget)
	A(tChainTarget)
	A(tLim)
	A(tHand[3])
	A(tBackGround)
	A(tBackGround_menu)
	A(tBackGround_deck)
	A(tField[2][4])
	A(tFieldTransparent[2][4])
#undef A
	void ClearFutureObjects(loading_map* map1, loading_map* map2, loading_map* map3, loading_map* map4);
	void DownloadPic();
	void AddToDownloadQueue(int code, imgType type);
	void RefreshCovers();
	downloadStatus GetDownloadStatus(int code, imgType type);
	path_string GetDownloadPath(int code, imgType type);
	image_path LoadCardTexture(int code, imgType type, std::atomic<irr::s32>& width, std::atomic<irr::s32>& height, chrono_time timestamp_id, std::atomic<chrono_time>& source_timestamp_id);
	loading_map* loading_pics[4];
	path_string textures_path;
	downloading_map downloading_images[3];
	std::queue<downloadParam> to_download;
	std::vector<downloadParam> downloading;
	std::pair<std::atomic<irr::s32>, std::atomic<irr::s32>> sizes[3];
	std::mutex pic_download;
	std::mutex field_download;
	std::mutex mtx;
	std::condition_variable cv;
	std::atomic<chrono_time> timestamp_id;
	std::atomic<bool> stop_threads;
	std::vector<PicSource> pic_urls;
	std::thread download_threads[8];
};

extern ImageManager imageManager;

#define CARD_IMG_WIDTH		177
#define CARD_IMG_HEIGHT		254
#define CARD_THUMB_WIDTH	44
#define CARD_THUMB_HEIGHT	64

}

#endif // IMAGEMANAGER_H
