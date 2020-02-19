#ifndef IMAGE_DOWNLOADER_H
#define IMAGE_DOWNLOADER_H
#include <memory>
#include <mutex>
#include <vector>
#include <thread>
#include <atomic>
#include <queue>
#include <map>
#include "text_types.h"

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
	std::pair<std::atomic<int>, std::atomic<int>> sizes[3];
	std::mutex pic_download;
	std::mutex field_download;
	std::mutex mtx;
	std::condition_variable cv;
	std::atomic<bool> stop_threads;
	std::vector<PicSource> pic_urls;
	std::thread download_threads[8];
};

extern std::shared_ptr<ImageDownloader> gImageDownloader;

}


#endif //IMAGE_DOWNLOADER_H
