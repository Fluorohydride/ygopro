#ifndef IMAGE_DOWNLOADER_H
#define IMAGE_DOWNLOADER_H
#include <memory>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <thread>
#include <atomic>
#include <deque>
#include <map>
#include "text_types.h"

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

class ImageDownloader {
public:
	enum class downloadStatus {
		DOWNLOADING,
		DOWNLOAD_ERROR,
		DOWNLOADED,
		NONE
	};
	struct downloadParam {
		uint32_t code;
		imgType type;
		downloadStatus status;
		epro::path_string path;
	};
	struct PicSource {
		std::string url;
		imgType type;
	};
	using downloading_map = std::map<uint32_t/*code*/, downloadParam>; /*if the value is not found, the download hasn't started yet*/
	ImageDownloader();
	~ImageDownloader();
	void AddDownloadResource(PicSource src);
	downloadStatus GetDownloadStatus(uint32_t code, imgType type);
	epro::path_string GetDownloadPath(uint32_t code, imgType type);
	void AddToDownloadQueue(uint32_t code, imgType type);
private:
	void DownloadPic();
	downloading_map downloading_images[3];
	std::deque<downloadParam> to_download;
	std::vector<downloadParam> downloading;
	std::pair<std::atomic<int>, std::atomic<int>> sizes[3];
	std::mutex pic_download;
	std::condition_variable cv;
	bool stop_threads;
	std::vector<PicSource> pic_urls;
	std::thread download_threads[8];
};

extern ImageDownloader* gImageDownloader;

}


#endif //IMAGE_DOWNLOADER_H
