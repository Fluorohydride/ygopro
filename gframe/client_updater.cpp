#include "client_updater.h"
#ifndef UPDATE_URL
void ygo::updater::CheckUpdates() {}
bool ygo::updater::HasUpdate() { return false; }
bool ygo::updater::StartUpdate(void(*)(int, void*), void*, const path_string&) { return false; }
bool ygo::updater::UpdateDownloaded() { return false; }
#else
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <thread>
#include <fstream>
#include <atomic>
#include <sstream>
#include "utils.h"

std::atomic<bool> has_update{ false };
std::atomic<bool> downloaded{ false };
std::atomic<bool> downloading{ false };

std::string update_url = "";

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
	void(*callback)(int, void*) = nullptr;
	void* payload = nullptr;
};

int progress_callback(void* ptr, curl_off_t TotalToDownload, curl_off_t NowDownloaded, curl_off_t TotalToUpload, curl_off_t NowUploaded) {
	if(TotalToDownload <= 0.0) {
		return 0;
	}
	Payload* payload = reinterpret_cast<Payload*>(ptr);
	if(payload && payload->callback) {
		double fractiondownloaded = NowDownloaded / TotalToDownload;
		int percentage = std::round(fractiondownloaded * 100);
		payload->callback(percentage, payload->payload);
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
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "EDOPro-Autoupdater");
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
					auto id = asset["id"].get<int>();
					if(name == "include.zip") {
						update_url = asset["browser_download_url"].get<std::string>();
						has_update = true;
						break;
					}
				}
				catch(...) {}
			}
		}
	}
	catch(...) { return; }
}

void ygo::updater::CheckUpdates() {
	Lock = GetLock();
	if(!Lock)
		return;
	try {
		std::thread(CheckUpdate).detach();
	}
	catch(...) {}
}

bool ygo::updater::HasUpdate() {
	return has_update;
}

bool ygo::updater::UpdateDownloaded() {
	return downloaded;
}

void DownloadUpdate(path_string dest_path, void* payload, void(*callback)(int, void*)) {
	Payload cbpayload = { callback, payload };
	auto name = dest_path + EPRO_TEXT("/") + ygo::Utils::GetFileName(ygo::Utils::ToPathString(update_url), true);
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
	downloaded = true;
}

bool ygo::updater::StartUpdate(void(*callback)(int percentage, void* payload), void* payload, const path_string& dest) {
	if(!has_update || update_url.empty())
		return false;
	try {
		std::thread(DownloadUpdate, dest, payload, callback).detach();
	}
	catch(...) { return false; }
	return true;
}
#endif