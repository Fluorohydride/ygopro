#include "image_downloader.h"
#include <fstream>
#include <curl/curl.h>
#include <fmt/format.h>
#include "logging.h"
#include "utils.h"
#include "game_config.h"

namespace ygo {

ImageDownloader::ImageDownloader() {
	stop_threads = false;
	for(auto& thread : download_threads) {
		thread = std::thread(&ImageDownloader::DownloadPic, this);
	}
}
ImageDownloader::~ImageDownloader() {
	stop_threads = true;
	cv.notify_all();
	for(auto& thread : download_threads) {
		thread.join();
	}
}
void ImageDownloader::AddDownloadResource(PicSource src) {
	pic_urls.push_back(src);
}
static constexpr uint8_t pngheader[] = { 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a };
static constexpr uint8_t jpgheader[] = { 0xff, 0xd8, 0xff };
#define PNG_FILE 1
#define JPG_FILE 2
int CheckImageHeader(void* header) {
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
const path_char* GetExtension(char* header) {
	int res = CheckImageHeader(header);
	if(res == PNG_FILE)
		return EPRO_TEXT(".png");
	else if(res == JPG_FILE)
		return EPRO_TEXT(".jpg");
	return EPRO_TEXT("");
}
void ImageDownloader::DownloadPic() {
	Utils::SetThreadName("PicDownloader");
	path_string dest_folder;// = fmt::format(EPRO_TEXT("./pics/{}"), code);
	path_string name;// = fmt::format(EPRO_TEXT("./pics/temp/{}"), code);
	path_string ext;
	while(!stop_threads) {
		std::unique_lock<std::mutex> lck(pic_download);
		while(to_download.size() == 0) {
			if(stop_threads)
				return;
			cv.wait(lck);
		}
		auto file = std::move(to_download.front());
		to_download.pop();
		auto type = file.type;
		auto code = file.code;
		downloading_images[type][file.code].status = DOWNLOADING;
		downloading.push_back(std::move(file));
		lck.unlock();
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
			CURL* curl = nullptr;
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
					curl_easy_setopt(curl, CURLOPT_URL, fmt::format(src.url, code).data());
					curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
					curl_easy_setopt(curl, CURLOPT_WRITEDATA, &payload);
					if(gGameConfig->ssl_certificate_path.size())
						curl_easy_setopt(curl, CURLOPT_CAINFO, gGameConfig->ssl_certificate_path.data());
#ifdef _WIN32
					else
						curl_easy_setopt(curl, CURLOPT_SSL_OPTIONS, CURLSSLOPT_NATIVE_CA);
#endif
					res = curl_easy_perform(curl);
					curl_easy_cleanup(curl);
					fp.close();
					if(res == CURLE_OK) {
						ext = GetExtension(payload.header);
						if(!Utils::FileMove(name, dest_folder + ext)) {
							Utils::FileDelete(name);
							ext.clear();
						}
						break;
					} else {
						Utils::FileDelete(name);
					}
				}
			}
		}
		lck.lock();
		if(ext.size()) {
			map[code].status = DOWNLOADED;
			map[code].path = dest_folder + ext;
		} else
			map[code].status = DOWNLOAD_ERROR;
	}
}
void ImageDownloader::AddToDownloadQueue(uint32_t code, imgType type) {
	if(type == THUMB)
		type = ART;
	int index = static_cast<int>(type);
	std::lock_guard<std::mutex> lck(pic_download);
	if(downloading_images[index].find(code) == downloading_images[index].end()) {
		downloading_images[index][code].status = DOWNLOADING;
		to_download.push(downloadParam{ code, type, NONE, EPRO_TEXT("") });
	}
	cv.notify_one();
}
ImageDownloader::downloadStatus ImageDownloader::GetDownloadStatus(uint32_t code, imgType type) {
	if(type == THUMB)
		type = ART;
	int index = static_cast<int>(type);
	std::lock_guard<std::mutex> lk(pic_download);
	if(downloading_images[index].find(code) == downloading_images[index].end())
		return NONE;
	return downloading_images[index][code].status;
}
path_string ImageDownloader::GetDownloadPath(uint32_t code, imgType type) {
	if(type == THUMB)
		type = ART;
	int index = static_cast<int>(type);
	std::lock_guard<std::mutex> lk(pic_download);
	if(downloading_images[index].find(code) == downloading_images[index].end())
		return EPRO_TEXT("");
	return downloading_images[index][code].path;
}
}
