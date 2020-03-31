#ifndef DATA_LOADER_H
#define DATA_LOADER_H
#include <memory>
#include "image_downloader.h"
#include "repo_manager.h"
#include "game_config.h"
#include "sound_manager.h"
#include "data_manager.h"
#include "deck_manager.h"

namespace irr {
class IrrlichtDevice;
namespace io {
class IFileSystem;
}
}

namespace ygo {

class DataHandler {
	irr::io::IFileSystem* filesystem = nullptr;
	void LoadDatabases();
	void LoadZipArchives();
	void LoadArchivesDB();
	void LoadPicUrls();

public:
	DataHandler();
	~DataHandler();
	irr::IrrlichtDevice* tmp_device;
	std::unique_ptr<DeckManager> deckManager = nullptr;
	std::unique_ptr<RepoManager> gitManager = nullptr;
	std::unique_ptr<GameConfig> configs = nullptr;
	std::unique_ptr<SoundManager> sounds = nullptr;
	std::unique_ptr<DataManager> dataManager = nullptr;
	std::unique_ptr<ImageDownloader> imageDownloader = nullptr;
};
}


#endif // DATA_LOADER_H
