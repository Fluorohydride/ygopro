#include "image_downloader.h"
#include <fstream>
#include <curl/curl.h>
#include <fmt/format.h>
#include <errno.h>
#include "logging.h"
#include "utils.h"
#include "game_config.h"

#ifdef UNICODE
#define fileopen(file, mode) _wfopen(file, L##mode)
#else
#define fileopen(file, mode) fopen(file, mode)
#endif

namespace ygo {

struct curl_payload {
	FILE* stream;
	char header[8];
	int header_written;
};

ImageDownloader::ImageDownloader() : stop_threads(false) {
	for(auto& thread : download_threads) {
		thread = std::thread(&ImageDownloader::DownloadPic, this);
	}
}
ImageDownloader::~ImageDownloader() {
	std::unique_lock<std::mutex> lck(pic_download);
	stop_threads = true;
	cv.notify_all();
	lck.unlock();
	for(auto& thread : download_threads)
		thread.join();
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
	auto data = static_cast<curl_payload*>(userdata);
	if(data->header_written < sizeof(data->header)) {
		auto increase = std::min(nmemb * size, (size_t)(sizeof(data->header) - data->header_written));
		memcpy(&data->header[data->header_written], ptr, increase);
		data->header_written += increase;
		if(data->header_written == sizeof(data->header) && !CheckImageHeader(data->header))
			return -1;
	}
	FILE* out = data->stream;
	size_t nbytes = size * nmemb;
	fwrite(ptr, 1, nbytes, out);
	return nbytes;
}
const epro::path_char* GetExtension(char* header) {
	int res = CheckImageHeader(header);
	if(res == PNG_FILE)
		return EPRO_TEXT(".png");
	else if(res == JPG_FILE)
		return EPRO_TEXT(".jpg");
	return EPRO_TEXT("");
}
void ImageDownloader::DownloadPic() {
	Utils::SetThreadName("PicDownloader");
	curl_payload payload;
	char curl_error_buffer[CURL_ERROR_SIZE];
	CURL* curl = [&payload, &curl_error_buffer] {
		auto curl = curl_easy_init();
		if(!curl)
			return curl;
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curl_error_buffer);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &payload);
		curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1);
		if(gGameConfig->ssl_certificate_path.size() && Utils::FileExists(Utils::ToPathString(gGameConfig->ssl_certificate_path)))
			curl_easy_setopt(curl, CURLOPT_CAINFO, gGameConfig->ssl_certificate_path.data());
#ifdef _WIN32
		else
			curl_easy_setopt(curl, CURLOPT_SSL_OPTIONS, CURLSSLOPT_NATIVE_CA);
#endif
		return curl;
	}();
	if(curl == nullptr) {
		ErrorLog("Failed to start downloader thread");
		return;
	}
	auto SetPayloadAndUrl = [&payload,&curl](epro::stringview url, FILE* stream) {
		payload.stream = stream;
		memset(&payload.header, 0, sizeof(payload.header));
		payload.header_written = 0;
		curl_easy_setopt(curl, CURLOPT_URL, url.data());
	};
	while(true) {
		std::unique_lock<std::mutex> lck(pic_download);
		if(stop_threads) {
			curl_easy_cleanup(curl);
			return;
		}
		while(to_download.empty()) {
			cv.wait(lck);
			if(stop_threads) {
				curl_easy_cleanup(curl);
				return;
			}
		}
		downloading.push_back(std::move(to_download.front()));
		to_download.pop_front();
		auto& file = downloading.back();
		auto type = file.type;
		auto code = file.code;
		auto& map_elem = downloading_images[type][code];
		map_elem.status = downloadStatus::DOWNLOADING;
		lck.unlock();
		auto name = fmt::format(EPRO_TEXT("./pics/temp/{}"), code);
		if(type == imgType::THUMB)
			type = imgType::ART;
		const auto dest_folder = [type, &name, code]()->epro::path_string {
			const epro::path_char* dest = nullptr;
			switch(type) {
				default:
				case imgType::ART:
				case imgType::THUMB: {
					dest = EPRO_TEXT("./pics/{}");
					break;
				}
				case imgType::FIELD: {
					dest = EPRO_TEXT("./pics/field/{}");
					name.append(EPRO_TEXT("_f"));
					break;
				}
				case imgType::COVER: {
					dest = EPRO_TEXT("./pics/cover/{}");
					name.append(EPRO_TEXT("_c"));
					break;
				}
			}
			return fmt::format(dest, code);
		}();
		const epro::path_char* ext = nullptr;
		for(auto& src : pic_urls) {
			if(src.type != type)
				continue;
			auto fp = fileopen(name.data(), "wb");
			if(fp == nullptr) {
				ygo::ErrorLog(fmt::format("Failed opening {} for write.", Utils::ToUTF8IfNeeded(name)));
				ygo::ErrorLog(fmt::format("Error: {}.", strerror(errno)));
				continue;
			}
			SetPayloadAndUrl(fmt::format(src.url, code), fp);
			CURLcode res = curl_easy_perform(curl);
			fclose(fp);
			if(res == CURLE_OK) {
				ext = GetExtension(payload.header);
				if(!Utils::FileMove(name, dest_folder + ext))
					Utils::FileDelete(name);
				break;
			}
			if(gGameConfig->logDownloadErrors) {
				ygo::ErrorLog(fmt::format("Failed downloading pic for {}", code));
				ygo::ErrorLog(fmt::format("Curl error: ({}) {} ({})", res, curl_easy_strerror(res), curl_error_buffer));
			}
			Utils::FileDelete(name);
		}
		lck.lock();
		if(ext) {
			map_elem.status = downloadStatus::DOWNLOADED;
			map_elem.path = dest_folder + ext;
		} else
			map_elem.status = downloadStatus::DOWNLOAD_ERROR;
	}
}
void ImageDownloader::AddToDownloadQueue(uint32_t code, imgType type) {
	if(type == imgType::THUMB)
		type = imgType::ART;
	std::lock_guard<std::mutex> lck(pic_download);
	if(stop_threads)
		return;
	if(downloading_images[type].find(code) == downloading_images[type].end()) {
		downloading_images[type][code].status = downloadStatus::DOWNLOADING;
		to_download.emplace_back(downloadParam{ code, type, downloadStatus::NONE, EPRO_TEXT("") });
		cv.notify_one();
	}
}
ImageDownloader::downloadStatus ImageDownloader::GetDownloadStatus(uint32_t code, imgType type) {
	if(type == imgType::THUMB)
		type = imgType::ART;
	std::lock_guard<std::mutex> lk(pic_download);
	auto it = downloading_images[type].find(code);
	if(it == downloading_images[type].end())
		return downloadStatus::NONE;
	return it->second.status;
}
epro::path_stringview ImageDownloader::GetDownloadPath(uint32_t code, imgType type) {
	if(type == imgType::THUMB)
		type = imgType::ART;
	std::lock_guard<std::mutex> lk(pic_download);
	auto it = downloading_images[type].find(code);
	if(it == downloading_images[type].end())
		return EPRO_TEXT("");
	return it->second.path;
}
}
