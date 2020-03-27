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
private:
	using image_path = std::pair<irr::video::IImage*, path_string>;
	using loading_map = std::map<int, std::future<image_path>>;
	using chrono_time = unsigned long long;
	using texture_map = std::unordered_map<uint32_t, irr::video::ITexture*>;
public:
	ImageManager() {
		loading_pics[0] = new loading_map();
		loading_pics[1] = new loading_map();
		loading_pics[2] = new loading_map();
		loading_pics[3] = new loading_map();
	}
	~ImageManager() {
		delete loading_pics[0];
		delete loading_pics[1];
		delete loading_pics[2];
		delete loading_pics[3];
	}
	bool Initial();
	void ChangeTextures(const path_string& path);
	void ResetTextures();
	void SetDevice(irr::IrrlichtDevice* dev);
	void ClearTexture(bool resize = false);
	void RemoveTexture(uint32_t code);
	void RefreshCachedTextures();
	void ClearCachedTextures(bool resize);
	bool imageScaleNNAA(irr::video::IImage *src, irr::video::IImage* dest, irr::s32 width, irr::s32 height, chrono_time timestamp_id, std::atomic<chrono_time>& source_timestamp_id);
	irr::video::IImage* GetTextureImageFromFile(const irr::io::path& file, int width, int height, chrono_time timestamp_id, std::atomic<chrono_time>& source_timestamp_id, irr::io::IReadFile* archivefile = nullptr);
	irr::video::ITexture* GetTextureFromFile(const irr::io::path& file, int width, int height);
	irr::video::ITexture* GetTextureCard(uint32_t code, imgType type, bool wait = false, bool fit = false, int* chk = nullptr);
	irr::video::ITexture* GetTextureField(uint32_t code);
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
	A(tOT)
	A(tHand[3])
	A(tBackGround)
	A(tBackGround_menu)
	A(tBackGround_deck)
	A(tField[2][4])
	A(tFieldTransparent[2][4])
	A(tSettings)
#undef A
	void ClearFutureObjects(loading_map* map1, loading_map* map2, loading_map* map3, loading_map* map4);
	void RefreshCovers();
	image_path LoadCardTexture(uint32_t code, imgType type, std::atomic<irr::s32>& width, std::atomic<irr::s32>& height, chrono_time timestamp_id, std::atomic<chrono_time>& source_timestamp_id);
	loading_map* loading_pics[4];
	path_string textures_path;
	std::pair<std::atomic<irr::s32>, std::atomic<irr::s32>> sizes[3];
	std::atomic<chrono_time> timestamp_id;
	std::map<irr::io::path, irr::video::ITexture*> g_txrCache;
	std::map<irr::io::path, irr::video::IImage*> g_imgCache;
};

#define CARD_IMG_WIDTH		177
#define CARD_IMG_HEIGHT		254
#define CARD_THUMB_WIDTH	44
#define CARD_THUMB_HEIGHT	64

}

#endif // IMAGEMANAGER_H
