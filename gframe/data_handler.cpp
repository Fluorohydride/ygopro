#include "data_handler.h"
#include <fstream>
#include <curl/curl.h>
#include <irrlicht.h>
#include "logging.h"
#include "utils.h"

namespace ygo {
void DataHandler::LoadDatabases() {
	if(std::ifstream("cards.cdb").good())
		dataManager->LoadDB(EPRO_TEXT("cards.cdb"));
	for(auto& file : Utils::FindFiles(EPRO_TEXT("./expansions/"), { EPRO_TEXT("cdb") }, 2))
		dataManager->LoadDB(EPRO_TEXT("./expansions/") + file);
	LoadArchivesDB();
}
void DataHandler::LoadArchivesDB() {
	std::vector<char> buffer;
	for(auto& archive : Utils::archives) {
		auto files = Utils::FindFiles(archive, EPRO_TEXT(""), { EPRO_TEXT("cdb") }, 3);
		for(auto& index : files) {
			auto reader = archive->createAndOpenFile(index);
			if(reader == nullptr)
				continue;
			buffer.resize(reader->getSize());
			reader->read(buffer.data(), buffer.size());
			reader->drop();
			dataManager->LoadDBFromBuffer(buffer);
		}
	}
}

void DataHandler::LoadPicUrls() {
	try {
		if(configs->configs.size() && configs->configs["urls"].is_array()) {
			for(auto& obj : configs->configs["urls"].get<std::vector<nlohmann::json>>()) {
				if(obj["url"].get<std::string>() == "default") {
					if(obj["type"].get<std::string>() == "pic") {
#ifdef DEFAULT_PIC_URL
						ImageDownloader.AddDownloadResource({ DEFAULT_PIC_URL, ImageDownloader::ART });
#else
						continue;
#endif
					} else {
#ifdef DEFAULT_FIELD_URL
						ImageDownloader.AddDownloadResource({ DEFAULT_FIELD_URL, ImageDownloader::FIELD });
#else
						continue;
#endif
					}
				} else {
					auto type = obj["type"].get<std::string>();
					imageDownloader->AddDownloadResource({ obj["url"].get<std::string>(), type == "field" ? ImageDownloader::FIELD : (type == "pic") ? ImageDownloader::ART : ImageDownloader::COVER });
				}
			}
		}
	}
	catch(std::exception& e) {
		ErrorLog(std::string("Exception occurred: ") + e.what());
	}
}
void DataHandler::LoadZipArchives() {
	irr::io::IFileArchive* tmp_archive = nullptr;
	for(auto& file : Utils::FindFiles(EPRO_TEXT("./expansions/"), { EPRO_TEXT("zip") })) {
		filesystem->addFileArchive((EPRO_TEXT("./expansions/") + file).c_str(), true, false, irr::io::EFAT_ZIP, "", &tmp_archive);
		if(tmp_archive) {
			Utils::archives.emplace_back(tmp_archive);
		}
	}
}
DataHandler::DataHandler() {
	tmp_device = irr::createDevice(irr::video::EDT_NULL);
	filesystem = tmp_device->getFileSystem();
	LoadZipArchives();
	gitManager = std::make_shared<RepoManager>();
	configs = std::make_shared<GameConfig>();
#ifdef __ANDROID__
	configs->working_directory = porting::working_directory;
#endif
	sounds = std::make_shared<SoundManager>(configs->soundVolume, configs->musicVolume, configs->enablesound, configs->enablemusic, configs->working_directory);
	gitManager->LoadRepositoriesFromJson(configs->configs);
	dataManager = std::make_shared<DataManager>();
	imageDownloader = std::make_shared<ImageDownloader>();
	LoadDatabases();
	auto strings_loaded = dataManager->LoadStrings(EPRO_TEXT("./config/strings.conf"));
	strings_loaded = dataManager->LoadStrings(EPRO_TEXT("./expansions/strings.conf")) || strings_loaded;
	if(!strings_loaded) {
		throw std::exception("Failed to load strings!");
	}
}
DataHandler::~DataHandler() {
	if(tmp_device)
		tmp_device->drop();
}
ImageDownloader::ImageDownloader() {
	stop_threads = false;
	for(int i = 0; i < 8; i++) {
		download_threads[i] = std::thread(&ImageDownloader::DownloadPic, this);
	}
}
ImageDownloader::~ImageDownloader() {
	stop_threads = true;
	cv.notify_all();
	for(int i = 0; i < 8; i++) {
		download_threads[i].join();
	}
}
void ImageDownloader::AddDownloadResource(PicSource src) {
	pic_urls.push_back(src);
}
#define PNG_HEADER 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a
#define PNG_FILE 1
#define JPG_HEADER 0xff, 0xd8, 0xff
#define JPG_FILE 2
int CheckImageHeader(char* header) {
	static unsigned char pngheader[] = { PNG_HEADER }; //png header
	static unsigned char jpgheader[] = { JPG_HEADER }; //jpg header
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
const irr::fschar_t* GetExtension(char* header) {
	int res = CheckImageHeader(header);
	if(res == PNG_FILE)
		return EPRO_TEXT(".png");
	else if(res == JPG_FILE)
		return EPRO_TEXT(".jpg");
	return EPRO_TEXT("");
}
void ImageDownloader::DownloadPic() {
	path_string dest_folder;// = fmt::format(EPRO_TEXT("./pics/{}"), code);
	path_string name;// = fmt::format(EPRO_TEXT("./pics/temp/{}"), code);
	path_string ext;
	while(!stop_threads) {
		if(to_download.size() == 0) {
			std::unique_lock<std::mutex> lck(mtx);
			cv.wait(lck);
		}
		pic_download.lock();
		if(to_download.size() == 0) {
			pic_download.unlock();
			continue;
		}
		auto file = to_download.front();
		to_download.pop();
		auto type = file.type;
		auto code = file.code;
		downloading_images[type][file.code].status = DOWNLOADING;
		downloading.push_back(std::move(file));
		pic_download.unlock();
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
			CURL *curl = NULL;
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
					curl_easy_setopt(curl, CURLOPT_URL, fmt::format(src.url, code).c_str());
					curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
					curl_easy_setopt(curl, CURLOPT_WRITEDATA, &payload);
#ifdef __ANDROID__
					curl_easy_setopt(curl, CURLOPT_CAINFO, (porting::internal_storage + "/cacert.cer").c_str());
#endif
					res = curl_easy_perform(curl);
					curl_easy_cleanup(curl);
					fp.close();
					if(res == CURLE_OK) {
						ext = GetExtension(payload.header);
						Utils::FileMove(name, dest_folder + ext);
						break;
					} else {
						Utils::FileDelete(name);
					}
				}
			}
		}
		pic_download.lock();
		if(ext.size()) {
			map[code].status = DOWNLOADED;
			map[code].path = dest_folder + ext;
		} else
			map[code].status = DOWNLOAD_ERROR;
		pic_download.unlock();
	}
}
void ImageDownloader::AddToDownloadQueue(int code, imgType type) {
	if(type == THUMB)
		type = ART;
	int index = static_cast<int>(type);
	pic_download.lock();
	if(downloading_images[index].find(code) == downloading_images[index].end()) {
		downloading_images[index][code].status = DOWNLOADING;
		to_download.push(downloadParam{ code, type, NONE, EPRO_TEXT("") });
	}
	pic_download.unlock();
	cv.notify_one();
}
ImageDownloader::downloadStatus ImageDownloader::GetDownloadStatus(int code, imgType type) {
	if(type == THUMB)
		type = ART;
	int index = static_cast<int>(type);
	std::lock_guard<std::mutex> lk(pic_download);
	if(downloading_images[index].find(code) == downloading_images[index].end())
		return NONE;
	return downloading_images[index][code].status;
}
path_string ImageDownloader::GetDownloadPath(int code, imgType type) {
	if(type == THUMB)
		type = ART;
	int index = static_cast<int>(type);
	std::lock_guard<std::mutex> lk(pic_download);
	if(downloading_images[index].find(code) == downloading_images[index].end())
		return EPRO_TEXT("");
	return downloading_images[index][code].path;
}
}
