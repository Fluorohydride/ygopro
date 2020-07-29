#include "data_handler.h"
#include <fstream>
#include <curl/curl.h>
#include <irrlicht.h>
#include "utils_gui.h"
#include "deck_manager.h"
#include "logging.h"
#include "utils.h"
#ifndef __ANDROID__
#include "IrrlichtCommonIncludes/CFileSystem.h"
#else
#include "IrrlichtCommonIncludes1.9/CFileSystem.h"
#include "Android/porting_android.h"
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
		std::lock_guard<std::mutex> guard(*archive.mutex);
		auto files = Utils::FindFiles(archive.archive, EPRO_TEXT(""), { EPRO_TEXT("cdb") }, 3);
		for(auto& index : files) {
			auto reader = archive.archive->createAndOpenFile(index);
			if(reader == nullptr)
				continue;
			buffer.resize(reader->getSize());
			reader->read(buffer.data(), buffer.size());
			std::string filename(irr::core::stringc(reader->getFileName()).c_str()); //the zip loader stores the names as utf8
			reader->drop();
			dataManager->LoadDBFromBuffer(buffer, filename);
		}
	}
}

void DataHandler::LoadPicUrls() {
	for(auto& _config : { std::ref(configs->user_configs), std::ref(configs->configs) }) {
		auto& config = _config.get();
		try {
			if(config.size() && config.at("urls").is_array()) {
				try {
					for(auto& obj : config["urls"].get<std::vector<nlohmann::json>>()) {
						auto type = obj["type"].get<std::string>();
						if(obj["url"].get<std::string>() == "default") {
							if(type == "pic") {
#ifdef DEFAULT_PIC_URL
								imageDownloader->AddDownloadResource({ DEFAULT_PIC_URL, ImageDownloader::ART });
#else
								continue;
#endif
							} else if(type == "field") {
#ifdef DEFAULT_FIELD_URL
								imageDownloader->AddDownloadResource({ DEFAULT_FIELD_URL, ImageDownloader::FIELD });
#else
								continue;
#endif
							} else if(type == "cover") {
#ifdef DEFAULT_COVER_URL
								imageDownloader->AddDownloadResource({ DEFAULT_COVER_URL, ImageDownloader::COVER });
#else
								continue;
#endif
							}
						} else {
							imageDownloader->AddDownloadResource({ obj["url"].get<std::string>(), type == "field" ?
																	ImageDownloader::FIELD : (type == "pic") ?
																	ImageDownloader::ART : ImageDownloader::COVER });
						}
					}
				}
				catch(std::exception& e) {
					ErrorLog(std::string("Exception occurred: ") + e.what());
				}
			}
		}
		catch(...) {}
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
	configs = std::unique_ptr<GameConfig>(new GameConfig);
	gGameConfig = configs.get();
	tmp_device = nullptr;
#ifndef __ANDROID__
	tmp_device = GUIUtils::CreateDevice(configs.get());
#endif
	filesystem = new irr::io::CFileSystem();
	Utils::filesystem = filesystem;
	LoadZipArchives();
	deckManager = std::unique_ptr<DeckManager>(new DeckManager());
	gitManager = std::unique_ptr<RepoManager>(new RepoManager());
#ifdef __ANDROID__
	configs->working_directory = porting::working_directory;
	configs->ssl_certificate_path = porting::internal_storage + "/cacert.cer";
#endif
	sounds = std::unique_ptr<SoundManager>(new SoundManager(configs->soundVolume / 100.0, configs->musicVolume / 100.0, configs->enablesound, configs->enablemusic, configs->working_directory));
	gitManager->LoadRepositoriesFromJson(configs->user_configs);
	gitManager->LoadRepositoriesFromJson(configs->configs);
	dataManager = std::unique_ptr<DataManager>(new DataManager());
	imageDownloader = std::unique_ptr<ImageDownloader>(new ImageDownloader());
	LoadDatabases();
	LoadPicUrls();
	deckManager->LoadLFList();
	auto strings_loaded = dataManager->LoadStrings(EPRO_TEXT("./config/strings.conf"));
	strings_loaded = dataManager->LoadStrings(EPRO_TEXT("./expansions/strings.conf")) || strings_loaded;
	if(!strings_loaded) {
		throw std::runtime_error("Failed to load strings!");
	}
}
DataHandler::~DataHandler() {
	if(filesystem)
		filesystem->drop();
}

}
