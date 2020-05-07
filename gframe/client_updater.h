#ifndef CLIENT_UPDATER_H
#define CLIENT_UPDATER_H

#ifdef UPDATE_URL
#include <vector>
#include <atomic>
#endif
#include <functional>
#include "utils.h"

struct UnzipperPayload {
	int cur;
	int tot;
	const path_char* filename;
	void* payload;
};

using update_callback = std::function<void(int percentage, int cur, int tot, const char* filename, bool is_new, void* payload)>;

namespace ygo {

class ClientUpdater {
public:
	ClientUpdater();
	~ClientUpdater();
	bool StartUpdate(update_callback callback, void* payload, const path_string& dest = EPRO_TEXT("./updates/"));
	void StartUnzipper(unzip_callback callback, void* payload, const path_string& src = EPRO_TEXT("./updates/"));
	void CheckUpdates();
#ifdef UPDATE_URL
	bool HasUpdate() {
		return has_update;
	}
	bool UpdateDownloaded() {
		return downloaded;
	}
#ifdef _WIN32
	using lock_type = void*;
#else
	using lock_type = size_t;
#endif
private:
	void CheckUpdate();
	void DownloadUpdate(path_string dest_path, void* payload, update_callback callback);
	void Unzip(path_string src, void* payload, unzip_callback callback);
	struct DownloadInfo {
		std::string name;
		std::string url;
		std::string md5;
	};
	std::vector<DownloadInfo> update_urls;
	lock_type Lock{ 0 };
	std::atomic<bool> has_update{ false };
	std::atomic<bool> downloaded{ false };
	std::atomic<bool> downloading{ false };
#else
	bool HasUpdate() {
		return false;
	}
	bool UpdateDownloaded() {
		return false;
	}
#endif
};

extern ClientUpdater* gClientUpdater;

};

#endif //CLIENT_UPDATER_H
