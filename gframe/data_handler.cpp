#include "data_handler.h"
#include <fstream>
#include <curl/curl.h>
#include <irrlicht.h>
#include "utils_gui.h"
#include "deck_manager.h"
#include "logging.h"
#include "utils.h"
#include "windbot.h"
#include "windbot_panel.h"
#if IRRLICHT_VERSION_MAJOR==1 && IRRLICHT_VERSION_MINOR==9
#include "IrrlichtCommonIncludes1.9/CFileSystem.h"
#else
#include "IrrlichtCommonIncludes/CFileSystem.h"
#endif
#ifdef __ANDROID__
#include "Android/COSAndroidOperator.h"
#include "Android/porting_android.h"
#endif

namespace ygo {

void DataHandler::LoadDatabases() {
	if(Utils::FileExists(EPRO_TEXT("./cards.cdb"))) {
		if(dataManager->LoadDB(EPRO_TEXT("./cards.cdb")))
			WindBot::AddDatabase(EPRO_TEXT("./cards.cdb"));
	}
	for(auto& file : Utils::FindFiles(EPRO_TEXT("./expansions/"), { EPRO_TEXT("cdb") }, 2)) {
		epro::path_string db = EPRO_TEXT("./expansions/") + file;
		if(dataManager->LoadDB(db))
			WindBot::AddDatabase(db);
	}
	LoadArchivesDB();
}
void DataHandler::LoadArchivesDB() {
	for(auto& archive : Utils::archives) {
		std::lock_guard<std::mutex> guard(*archive.mutex);
		auto files = Utils::FindFiles(archive.archive, EPRO_TEXT(""), { EPRO_TEXT("cdb") }, 3);
		for(auto& index : files) {
			auto reader = archive.archive->createAndOpenFile(index);
			if(reader == nullptr)
				continue;
			dataManager->LoadDB(reader);
			reader->drop();
		}
	}
}

void DataHandler::LoadPicUrls() {
	for(auto& _config : { &configs->user_configs, &configs->configs }) {
		const auto& config = *_config;
		auto it = config.find("urls");
		if(it != config.end() && it->is_array()) {
			for(const auto& obj : *it) {
				try {
					const auto& type = obj.at("type").get_ref<const std::string&>();
					const auto& url = obj.at("url").get_ref<const std::string&>();
					if(url == "default") {
						if(type == "pic") {
#ifdef DEFAULT_PIC_URL
							imageDownloader->AddDownloadResource({ DEFAULT_PIC_URL, imgType::ART });
#else
							continue;
#endif
						} else if(type == "field") {
#ifdef DEFAULT_FIELD_URL
							imageDownloader->AddDownloadResource({ DEFAULT_FIELD_URL, imgType::FIELD });
#else
							continue;
#endif
						} else if(type == "cover") {
#ifdef DEFAULT_COVER_URL
							imageDownloader->AddDownloadResource({ DEFAULT_COVER_URL, imgType::COVER });
#else
							continue;
#endif
						}
					} else {
						imageDownloader->AddDownloadResource({ url, type == "field" ?
																imgType::FIELD : (type == "pic") ?
																imgType::ART : imgType::COVER });
					}
				}
				catch(const std::exception& e) {
					ErrorLog("Exception occurred: {}", e.what());
				}
			}
		}
	}
}
void DataHandler::LoadZipArchives() {
	irr::io::IFileArchive* tmp_archive = nullptr;
	for(auto& file : Utils::FindFiles(EPRO_TEXT("./expansions/"), { EPRO_TEXT("zip") })) {
		filesystem->addFileArchive(fmt::format(EPRO_TEXT("./expansions/{}"), file).data(), true, false, irr::io::EFAT_ZIP, "", &tmp_archive);
		if(tmp_archive) {
			Utils::archives.emplace_back(tmp_archive);
		}
	}
}
DataHandler::DataHandler() {
	configs = std::unique_ptr<GameConfig>(new GameConfig());
	gGameConfig = configs.get();
	tmp_device = nullptr;
#ifndef __ANDROID__
	tmp_device = GUIUtils::CreateDevice(configs.get());
	Utils::OSOperator = tmp_device->getGUIEnvironment()->getOSOperator();
	Utils::OSOperator->grab();
	if(configs->override_ssl_certificate_path.size()) {
		if(configs->override_ssl_certificate_path != "none" && Utils::FileExists(Utils::ToPathString(configs->override_ssl_certificate_path)))
			configs->ssl_certificate_path = configs->override_ssl_certificate_path;
	} else
		configs->ssl_certificate_path = fmt::format("{}/cacert.pem", Utils::ToUTF8IfNeeded(Utils::GetWorkingDirectory()));
#else
	Utils::OSOperator = new irr::COSAndroidOperator();
	configs->ssl_certificate_path = fmt::format("{}/cacert.pem", porting::internal_storage);
#endif
	filesystem = new irr::io::CFileSystem();
	dataManager = std::unique_ptr<DataManager>(new DataManager());
	auto strings_loaded = dataManager->LoadStrings(EPRO_TEXT("./config/strings.conf"));
	strings_loaded = dataManager->LoadStrings(EPRO_TEXT("./expansions/strings.conf")) || strings_loaded;
	if(!strings_loaded)
		throw std::runtime_error("Failed to load strings!");
	Utils::filesystem = filesystem;
	LoadZipArchives();
	deckManager = std::unique_ptr<DeckManager>(new DeckManager());
	gitManager = std::unique_ptr<RepoManager>(new RepoManager());
	sounds = std::unique_ptr<SoundManager>(new SoundManager(configs->soundVolume / 100.0, configs->musicVolume / 100.0, configs->enablesound, configs->enablemusic));
	gitManager->LoadRepositoriesFromJson(configs->user_configs);
	gitManager->LoadRepositoriesFromJson(configs->configs);
	if(gitManager->TerminateIfNothingLoaded())
		deckManager->StopDummyLoading();
	imageDownloader = std::unique_ptr<ImageDownloader>(new ImageDownloader());
	LoadDatabases();
	LoadPicUrls();
	deckManager->LoadLFList();
	dataManager->LoadIdsMapping(EPRO_TEXT("./config/mappings.json"));
	WindBotPanel::absolute_deck_path = Utils::ToUnicodeIfNeeded(Utils::GetAbsolutePath(EPRO_TEXT("./deck")));
}
DataHandler::~DataHandler() {
	if(filesystem)
		filesystem->drop();
	if(Utils::OSOperator)
		Utils::OSOperator->drop();
}

}
