#include "client_updater.h"
#ifndef UPDATE_URL
void ygo::updater::CheckUpdates() {}
bool ygo::updater::HasUpdate() { return false; }
bool ygo::updater::StartUpdate(void(*)(int, void*), void*, const path_string&) { return false; }
bool ygo::updater::UpdateDownloaded() { return false; }
#else
#if defined(_WIN32)
#define OSSTRING "Windows"
#elif defined(__APPLE__)
#define OSSTRING "MacOs"
#elif defined (__linux__) && !defined(__ANDROID__)
#define OSSTRING "Linux"
#endif
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <thread>
#include <fstream>
#include <atomic>
#include <sstream>
#include <fmt/format.h>
#include "config.h"
#include "utils.h"
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define USERAGENT "Edopro-" OSSTRING "-" STR(EDOPRO_VERSION_MAJOR) "." STR(EDOPRO_VERSION_MINOR) "." STR(EDOPRO_VERSION_PATCH)

std::atomic<bool> has_update{ false };
std::atomic<bool> downloaded{ false };
std::atomic<bool> downloading{ false };

std::vector<std::pair<std::string, std::string>> update_urls;

void* Lock = nullptr;

void* GetLock() {
#ifdef _WIN32
	HANDLE hFile = CreateFile(EPRO_TEXT("./edopro_lock"), GENERIC_READ, 0, NULL, CREATE_ALWAYS, 0, NULL);
	return hFile == INVALID_HANDLE_VALUE ? nullptr : hFile;
#else
	size_t file = open("edopro_lock", O_CREAT, S_IRWXU);
	return flock(file, LOCK_EX | LOCK_NB) ? nullptr : (void*)file;
#endif
	return nullptr;
}

void FreeLock() {
#ifdef _WIN32
	CloseHandle(Lock);
	DeleteFile(EPRO_TEXT("./edopro_lock"));
#else
	flock(reinterpret_cast<uintptr_t>(Lock), LOCK_UN);
	close(reinterpret_cast<uintptr_t>(Lock));
	remove("./edopro_lock");
#endif
}
struct Payload {
	update_callback callback = nullptr;
	int current = 1;
	int total = 1;
	void* payload = nullptr;
	const char* filename = nullptr;
};

int progress_callback(void* ptr, curl_off_t TotalToDownload, curl_off_t NowDownloaded, curl_off_t TotalToUpload, curl_off_t NowUploaded) {
	if(TotalToDownload <= 0.0) {
		return 0;
	}
	Payload* payload = reinterpret_cast<Payload*>(ptr);
	if(payload && payload->callback) {
		double fractiondownloaded = NowDownloaded / TotalToDownload;
		int percentage = std::round(fractiondownloaded * 100);
		payload->callback(percentage, payload->current, payload->total, payload->filename, payload->payload);
	}
	return 0;
}

size_t WriteCallback(char *contents, size_t size, size_t nmemb, void *userp) {
	size_t realsize = size * nmemb;
	auto buff = static_cast<std::ostream*>(userp);
	buff->write(static_cast<const char*>(contents), realsize);
	return realsize;
}

CURL* setupHandle(const std::string& url, void* func, void* payload, void* payload2 = nullptr) {
	CURL* curl_handle = curl_easy_init();
	curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, func);
	curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, 5L);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, payload);
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, USERAGENT);
	curl_easy_setopt(curl_handle, CURLOPT_NOPROXY, "*");
	curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl_handle, CURLOPT_DNS_CACHE_TIMEOUT, 0);
	curl_easy_setopt(curl_handle, CURLOPT_XFERINFOFUNCTION, progress_callback);
	curl_easy_setopt(curl_handle, CURLOPT_XFERINFODATA, payload2);
	curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 0L);
	return curl_handle;
}

void CheckUpdate() {
	std::stringstream retrieved_data;
	auto curl_handle = setupHandle(UPDATE_URL, reinterpret_cast<void*>(WriteCallback), &retrieved_data);
	try {
		CURLcode res = curl_easy_perform(curl_handle);
		if(res != CURLE_OK)
			return;
		curl_easy_cleanup(curl_handle);
	}
	catch(...) {
		return;
	}
	try {
		nlohmann::json j = nlohmann::json::parse(retrieved_data);
		if(j["assets"].is_array()) {
			for(auto& asset : j["assets"]) {
				try {
					auto name = asset["name"].get<std::string>();
					update_urls.emplace_back(name, asset["browser_download_url"].get<std::string>());
				}
				catch(...) {}
			}
			if(update_urls.size())
				has_update = true;
		}
	}
	catch(...) { update_urls.clear(); }
}

void ygo::updater::CheckUpdates() {
	Lock = GetLock();
	if(!Lock)
		return;
	try {
		update_urls.clear();
		std::thread(CheckUpdate).detach();
	}
	catch(...) { update_urls.clear(); }
}

bool ygo::updater::HasUpdate() {
	return has_update;
}

bool ygo::updater::UpdateDownloaded() {
	return downloaded;
}

void DownloadUpdate(path_string dest_path, void* payload, update_callback callback) {
	downloading = true;
	Payload cbpayload = { callback, 1, static_cast<int>(update_urls.size()), payload, nullptr };
	int i = 1;
	for(auto& file : update_urls) {
		auto name = dest_path + EPRO_TEXT("/") + ygo::Utils::ToPathString(file.first);
		cbpayload.current = i;
		cbpayload.filename = file.first.data();
		auto& update_url = file.second;
		std::ofstream stream(name, std::ofstream::binary);
		auto curl_handle = setupHandle(update_url, reinterpret_cast<void*>(WriteCallback), &stream, &cbpayload);
		try {
			CURLcode res = curl_easy_perform(curl_handle);
			if(res != CURLE_OK)
				throw 1;
			stream.close();
			curl_easy_cleanup(curl_handle);
		}
		catch(...) {
			stream.close();
			ygo::Utils::FileDelete(name);
		}
		i++;
	}
	downloaded = true;
}

bool ygo::updater::StartUpdate(update_callback callback, void* payload, const path_string& dest) {
	if(!has_update || downloading)
		return false;
	try {
		std::thread(DownloadUpdate, dest, payload, callback).detach();
	}
	catch(...) { return false; }
	return true;
}
#endif