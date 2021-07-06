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
		auto& config = *_config;
		auto it = config.find("urls");
		if(it != config.end() && it->is_array()) {
			for(auto& obj : *it) {
				try {
					const auto& type = obj.at("type").get_ref<std::string&>();
					const auto& url = obj.at("url").get_ref<std::string&>();
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
					ErrorLog(fmt::format("Exception occurred: {}", e.what()));
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
static void DeleteOutdatedDbs() {
#if defined(_MSC_VER) && _MSC_VER == 1900
	static const epro::path_stringview dbs[] = {
#else
	static constexpr epro::path_stringview dbs[] = {
#endif
		EPRO_TEXT("./expansions/BLVO-release.cdb"),
		EPRO_TEXT("./expansions/cards-rush-prerelease.cdb"),
		EPRO_TEXT("./expansions/cards-unofficial-new.cdb"),
		EPRO_TEXT("./expansions/cards-unofficial-removal.cdb"),
		EPRO_TEXT("./expansions/fix-errata-pre-errata.cdb"),
		EPRO_TEXT("./expansions/fixMisc-unofficial.cdb"),
		EPRO_TEXT("./expansions/fixOT-unofficial.cdb"),
		EPRO_TEXT("./expansions/fixString-unofficial.cdb"),
		EPRO_TEXT("./expansions/prerelease.cdb"),
		EPRO_TEXT("./expansions/prerelease_21PP.cdb"),
		EPRO_TEXT("./expansions/prerelease-etco.cdb"),
		EPRO_TEXT("./expansions/prerelease-liov.cdb"),
		EPRO_TEXT("./expansions/prerelease-PGB1.cdb"),
		EPRO_TEXT("./expansions/prerelease-unofficial-21PP.cdb"),
		EPRO_TEXT("./expansions/prerelease-unofficial-cp20.cdb"),
		EPRO_TEXT("./expansions/prerelease-unofficial-liov.cdb"),
		EPRO_TEXT("./expansions/prerelease-unofficial-selection10.cdb"),
		EPRO_TEXT("./expansions/Proxy_Horse.cdb"),
		EPRO_TEXT("./expansions/release.cdb"),
		EPRO_TEXT("./expansions/release-dbgi-ocg.cdb"),
		EPRO_TEXT("./expansions/release-phra-ocg.cdb"),
		EPRO_TEXT("./expansions/release-sd40-ibfp.cdb"),
		EPRO_TEXT("./expansions/release-selection10.cdb"),
		EPRO_TEXT("./expansions/release-sr11-ocg.cdb"),
	};
	for(const auto& db : dbs) {
		Utils::FileDelete(db);
	}
}
DataHandler::DataHandler(epro::path_stringview working_dir) {
	DeleteOutdatedDbs();
	configs = std::unique_ptr<GameConfig>(new GameConfig);
	gGameConfig = configs.get();
	tmp_device = nullptr;
#ifndef __ANDROID__
	tmp_device = GUIUtils::CreateDevice(configs.get());
	Utils::OSOperator = tmp_device->getGUIEnvironment()->getOSOperator();
	Utils::OSOperator->grab();
#else
	Utils::OSOperator = new irr::COSAndroidOperator();
	configs->ssl_certificate_path = fmt::format("{}/cacert.cer", porting::internal_storage);
#endif
	filesystem = new irr::io::CFileSystem();
	dataManager = std::unique_ptr<DataManager>(new DataManager());
	auto strings_loaded = dataManager->LoadStrings(EPRO_TEXT("./config/strings.conf"));
	strings_loaded = dataManager->LoadStrings(EPRO_TEXT("./expansions/strings.conf")) || strings_loaded;
	if(!strings_loaded)
		throw std::runtime_error("Failed to load strings!");
	Utils::filesystem = filesystem;
	Utils::working_dir = Utils::NormalizePath(working_dir);
	LoadZipArchives();
	deckManager = std::unique_ptr<DeckManager>(new DeckManager());
	gitManager = std::unique_ptr<RepoManager>(new RepoManager());
	sounds = std::unique_ptr<SoundManager>(new SoundManager(configs->soundVolume / 100.0, configs->musicVolume / 100.0, configs->enablesound, configs->enablemusic, Utils::working_dir));
	gitManager->LoadRepositoriesFromJson(configs->user_configs);
	gitManager->LoadRepositoriesFromJson(configs->configs);
	imageDownloader = std::unique_ptr<ImageDownloader>(new ImageDownloader());
	LoadDatabases();
	LoadPicUrls();
	deckManager->LoadLFList();
	WindBotPanel::absolute_deck_path = Utils::ToUnicodeIfNeeded(Utils::GetAbsolutePath(EPRO_TEXT("./deck")));
}
DataHandler::~DataHandler() {
	if(filesystem)
		filesystem->drop();
	if(Utils::OSOperator)
		Utils::OSOperator->drop();
}

}
