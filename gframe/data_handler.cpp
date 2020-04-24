#include "data_handler.h"
#include <fstream>
#include <curl/curl.h>
#include <irrlicht.h>
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
	for(auto& _config : { std::ref(configs->user_configs), std::ref(configs->configs) }) {
		auto& config = _config.get();
		try {
			if(config.size() && config["urls"].is_array()) {
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
						imageDownloader->AddDownloadResource({ obj["url"].get<std::string>(), type == "field" ? ImageDownloader::FIELD : (type == "pic") ? ImageDownloader::ART : ImageDownloader::COVER });
					}
				}
			}
		}
		catch(std::exception& e) {
			ErrorLog(std::string("Exception occurred: ") + e.what());
		}
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
	tmp_device = nullptr;
#ifndef __ANDROID__
	irr::SIrrlichtCreationParameters params = irr::SIrrlichtCreationParameters();
	params.AntiAlias = configs->antialias;
#ifdef _IRR_COMPILE_WITH_DIRECT3D_9_
	if(configs->use_d3d)
		params.DriverType = irr::video::EDT_DIRECT3D9;
	else
#endif
		params.DriverType = irr::video::EDT_OPENGL;
	params.WindowSize = irr::core::dimension2d<irr::u32>(1024 * configs->dpi_scale, 640 * configs->dpi_scale);
	params.Vsync = configs->vsync;
	tmp_device = irr::createDeviceEx(params);
	if(!tmp_device) {
		throw std::runtime_error("Failed to create Irrlicht Engine device!");
	}
#endif
	filesystem = new irr::io::CFileSystem();
	LoadZipArchives();
	deckManager = std::unique_ptr<DeckManager>(new DeckManager());
	gitManager = std::unique_ptr<RepoManager>(new RepoManager());
#ifdef __ANDROID__
	configs->working_directory = porting::working_directory;
#endif
	sounds = std::unique_ptr<SoundManager>(new SoundManager(configs->soundVolume / 100.0, configs->musicVolume / 100.0, configs->enablesound, configs->enablemusic / 100.0, configs->working_directory));
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
