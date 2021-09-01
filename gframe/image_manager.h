#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include "config.h"
#include <path.h>
#include <rect.h>
#include <unordered_map>
#include <map>
#include <atomic>
#include <future>
#include <queue>
#include <mutex>
#include <condition_variable>

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

#ifndef IMGTYPE
#define IMGTYPE
enum imgType {
	ART,
	FIELD,
	COVER,
	THUMB
};
#endif

class ImageManager {
private:
	using image_path = std::pair<irr::video::IImage*, epro::path_string>;
	using loading_map = std::map<int, std::future<image_path>>;
	using chrono_time = uint64_t;
	using texture_map = std::unordered_map<uint32_t, irr::video::ITexture*>;
public:
	ImageManager();
	~ImageManager();
	bool Initial();
	void ChangeTextures(epro::path_stringview path);
	void ResetTextures();
	void SetDevice(irr::IrrlichtDevice* dev);
	void ClearTexture(bool resize = false);
	void RemoveTexture(uint32_t code);
	void RefreshCachedTextures();
	void ClearCachedTextures(bool resize);
	bool imageScaleNNAA(irr::video::IImage* src, irr::video::IImage* dest, irr::s32 width, irr::s32 height, chrono_time timestamp_id, std::atomic<chrono_time>& source_timestamp_id);
	irr::video::IImage* GetScaledImage(irr::video::IImage* srcimg, int width, int height, chrono_time timestamp_id, std::atomic<chrono_time>& source_timestamp_id);
	irr::video::IImage* GetScaledImageFromFile(const irr::io::path& file, int width, int height);
	irr::video::ITexture* GetTextureFromFile(const irr::io::path& file, int width, int height);
	irr::video::ITexture* GetTextureCard(uint32_t code, imgType type, bool wait = false, bool fit = false, int* chk = nullptr);
	irr::video::ITexture* GetTextureField(uint32_t code);
	irr::video::ITexture* guiScalingResizeCached(irr::video::ITexture* src, const irr::core::rect<irr::s32>& srcrect,
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
#define A(what) \
		public: \
		irr::video::ITexture* what;\
		private: \
		irr::video::ITexture* def_##what;
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
	A(tOT)
	A(tHand[3])
	A(tBackGround)
	A(tBackGround_menu)
	A(tBackGround_deck)
	A(tField[2][4])
	A(tFieldTransparent[2][4])
	A(tSettings)
#undef A
private:
	void ClearFutureObjects();
	void RefreshCovers();
	image_path LoadCardTexture(uint32_t code, imgType type, std::atomic<irr::s32>& width, std::atomic<irr::s32>& height, chrono_time timestamp_id, std::atomic<chrono_time>& source_timestamp_id);
	epro::path_string textures_path;
	std::pair<std::atomic<irr::s32>, std::atomic<irr::s32>> sizes[3];
	std::atomic<chrono_time> timestamp_id;
	std::mutex obj_clear_lock;
	std::thread obj_clear_thread;
	std::condition_variable cv;
	std::map<epro::path_string, irr::video::ITexture*> g_txrCache;
	std::map<irr::io::path, irr::video::IImage*> g_imgCache; //ITexture->getName returns a io::path
	loading_map loading_pics[4];
	std::deque<std::pair<loading_map::key_type, loading_map::mapped_type>> to_clear;
	std::atomic<bool> stop_threads;
};

#define CARD_IMG_WIDTH		177
#define CARD_IMG_HEIGHT		254
#define CARD_IMG_WIDTH_F	177.0f
#define CARD_IMG_HEIGHT_F	254.0f
#define CARD_THUMB_WIDTH	44
#define CARD_THUMB_HEIGHT	64

}

#endif // IMAGEMANAGER_H
