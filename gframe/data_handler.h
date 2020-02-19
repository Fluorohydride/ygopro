#ifndef DATA_LOADER_H
#define DATA_LOADER_H
#include <memory>
#include "image_downloader.h"
#include "repo_manager.h"
#include "game_config.h"
#include "sound_manager.h"
#include "data_manager.h"

namespace irr {
class IrrlichtDevice;
namespace io {
class IFileSystem;
}
}

namespace ygo {

class DataHandler {
	irr::IrrlichtDevice* tmp_device = nullptr;
	irr::io::IFileSystem* filesystem = nullptr;
	void LoadDatabases();
	void LoadZipArchives();
	void LoadArchivesDB();
	void LoadPicUrls();

public:
	DataHandler();
	~DataHandler();
	std::shared_ptr<RepoManager> gitManager = nullptr;
	std::shared_ptr<GameConfig> configs = nullptr;
	std::shared_ptr<SoundManager> sounds = nullptr;
	std::shared_ptr<DataManager> dataManager = nullptr;
	std::shared_ptr<ImageDownloader> imageDownloader = nullptr;
};
}


#endif // DATA_LOADER_H
