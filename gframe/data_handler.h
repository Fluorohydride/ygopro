#ifndef DATA_LOADER_H
#define DATA_LOADER_H
#include <memory>
#include <vector>
#include <thread>
#include <atomic>
#include <queue>
#include "repo_manager.h"
#include "game_config.h"
#include "sound_manager.h"
#include "data_manager.h"

namespace irr {
class IrrlichtDevice;
namespace io {
class IFileSystem;
}
}

namespace ygo {

class ImageDownloader {
public:
	enum imgType {
		ART,
		FIELD,
		COVER,
		THUMB
	};
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
	struct PicSource {
		std::string url;
		imgType type;
	};
	using downloading_map = std::map<int/*code*/, downloadParam>; /*if the value is not found, the download hasn't started yet*/
	ImageDownloader();
	~ImageDownloader();
	void AddDownloadResource(PicSource src);
	downloadStatus GetDownloadStatus(int code, imgType type);
	path_string GetDownloadPath(int code, imgType type);
	void AddToDownloadQueue(int code, imgType type);
private:
	void DownloadPic();
	downloading_map downloading_images[3];
	std::queue<downloadParam> to_download;
	std::vector<downloadParam> downloading;
	std::pair<std::atomic<irr::s32>, std::atomic<irr::s32>> sizes[3];
	std::mutex pic_download;
	std::mutex field_download;
	std::mutex mtx;
	std::condition_variable cv;
	std::atomic<bool> stop_threads;
	std::vector<PicSource> pic_urls;
	std::thread download_threads[8];
};


class DataHandler {
	irr::IrrlichtDevice* tmp_device = nullptr;
	irr::io::IFileSystem* filesystem = nullptr;
	void LoadDatabases();
	void LoadZipArchives();
	void LoadArchivesDB();
	void LoadPicUrls();

public:
	DataHandler();
	~DataHandler();
	std::shared_ptr<RepoManager> gitManager = nullptr;
	std::shared_ptr<GameConfig> configs = nullptr;
	std::shared_ptr<SoundManager> sounds = nullptr;
	std::shared_ptr<DataManager> dataManager = nullptr;
	std::shared_ptr<ImageDownloader> imageDownloader = nullptr;
};
}


#endif // DATA_LOADER_H
