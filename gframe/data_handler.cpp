#include "data_handler.h"
#include <fstream>
#include <curl/curl.h>
#include <irrlicht.h>
#include "logging.h"
#include "utils.h"
#ifdef __ANDROID__
#include "porting_android.h"
#endif

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
						imageDownloader->AddDownloadResource({ DEFAULT_PIC_URL, ImageDownloader::ART });
#else
						continue;
#endif
					} else {
#ifdef DEFAULT_FIELD_URL
						imageDownloader->AddDownloadResource({ DEFAULT_FIELD_URL, ImageDownloader::FIELD });
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
		throw std::runtime_error("Failed to load strings!");
	}
}
DataHandler::~DataHandler() {
	if(tmp_device)
		tmp_device->drop();
}

}
