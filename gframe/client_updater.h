#ifndef CLIENT_UPDATER_H
#define CLIENT_UPDATER_H

#ifdef UPDATE_URL
#include <vector>
#include <atomic>
#endif
#include "utils.h"

struct UnzipperPayload {
	int cur;
	int tot;
	const epro::path_char* filename;
	void* payload;
};

using update_callback = void(*)(int percentage, int cur, int tot, const char* filename, bool is_new, void* payload);

namespace ygo {
#ifdef UPDATE_URL
class ClientUpdater {
public:
	ClientUpdater(epro::path_stringview override_url);
	~ClientUpdater() = default;
	bool StartUpdate(update_callback callback, void* payload);
	void StartUnzipper(unzip_callback callback, void* payload);
	void CheckUpdates();
	bool HasUpdate() {
		return has_update;
	}
	bool UpdateDownloaded() {
		return downloaded;
	}
	bool UpdateFailed() {
		return failed;
	}
private:
	class FileLock {
#ifdef __ANDROID__
	public:
		constexpr bool acquired() { return true; }
#else
#ifdef _WIN32
		using lock_type = void*;
		static constexpr lock_type null_lock = nullptr;
#else
		using lock_type = size_t;
		static constexpr lock_type null_lock = 0;
#endif
		lock_type m_lock{ null_lock };
	public:
		bool acquired() { return m_lock != null_lock; };
		FileLock();
		~FileLock();
#endif
	};
	void CheckUpdate();
	void DownloadUpdate(void* payload, update_callback callback);
	void Unzip(void* payload, unzip_callback callback);
	struct DownloadInfo {
		std::string name;
		std::string url;
		std::string md5;
	};
	std::vector<DownloadInfo> update_urls;
	FileLock Lock{};
	std::atomic<bool> has_update{ false };
	std::atomic<bool> downloaded{ false };
	std::atomic<bool> failed{ false };
	std::atomic<bool> downloading{ false };
	std::string update_url{ UPDATE_URL };
};
#else
class ClientUpdater {
public:
	ClientUpdater(epro::path_stringview) {}
	~ClientUpdater() = default;
	static constexpr bool StartUpdate(update_callback, void*) { return false; }
	static constexpr void StartUnzipper(unzip_callback, void*) {}
	static constexpr void CheckUpdates() {}
	static constexpr bool HasUpdate() { return false; }
	static constexpr bool UpdateDownloaded() { return false; }
	static constexpr bool UpdateFailed() { return true; }
};
#endif

extern ClientUpdater* gClientUpdater;

};

#endif //CLIENT_UPDATER_H
