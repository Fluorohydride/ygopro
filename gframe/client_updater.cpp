#include "client_updater.h"
#define UPDATE_URL "test"
#ifndef UPDATE_URL
void ygo::updater::CheckUpdates() {}
bool ygo::updater::HasUpdate() { return false; }
bool ygo::updater::StartUpdate(update_callback, void*, const path_string&) { return false; }
bool ygo::updater::UpdateDownloaded() { return false; }
void ygo::updater::StartUnzipper(const path_string&) {}
#else
#if defined(_WIN32)
#define OSSTRING "Windows"
#elif defined(__APPLE__)
#define OSSTRING "MacOs"
#elif defined (__linux__) && !defined(__ANDROID__)
#define OSSTRING "Linux"
#endif
#ifdef _WIN32
#include <Windows.h>
#include <tchar.h>
#else
#include <sys/file.h>
#include <unistd.h>
#define _trename rename
#define _tremove remove
#endif
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <thread>
#include <fstream>
#include <atomic>
#include <sstream>
#include <openssl/md5.h>
#include "config.h"
#include "utils.h"
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define USERAGENT "Edopro-" OSSTRING "-" STR(EDOPRO_VERSION_MAJOR) "." STR(EDOPRO_VERSION_MINOR) "." STR(EDOPRO_VERSION_PATCH)

std::atomic<bool> has_update{ false };
std::atomic<bool> downloaded{ false };
std::atomic<bool> downloading{ false };

struct DownloadInfo {
	std::string name;
	std::string url;
	std::string md5;
};

std::vector<DownloadInfo> update_urls;

void* Lock = nullptr;

const path_string& GetExePath() {
	static path_string binarypath = EPRO_TEXT("");
	if(binarypath.empty()) {
#ifdef _WIN32
		TCHAR exepath[MAX_PATH];
		GetModuleFileName(NULL, exepath, MAX_PATH);
		binarypath = exepath;
#elif defined(__linux__) && !defined(__ANDROID__)
		char buff[PATH_MAX];
		ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff) - 1);
		if(len != -1) {
			buff[len] = '\0';
		}
		binarypath = buff;
#endif
	}
	return binarypath;
}

void DeleteOld() {
#if defined(_WIN32) || (defined(__linux__) && !defined(__ANDROID__))
	_tremove((GetExePath() + EPRO_TEXT(".old")).c_str());
#endif
}

void* GetLock() {
#ifdef _WIN32
	HANDLE hFile = CreateFile(EPRO_TEXT("./edopro_lock"), GENERIC_READ, 0, NULL, CREATE_ALWAYS, 0, NULL);
	if(!hFile || hFile == INVALID_HANDLE_VALUE)
		return nullptr;
	DeleteOld();
	return hFile;
#else
	size_t file = open("./edopro_lock", O_CREAT, S_IRWXU);
	if(flock(file, LOCK_EX | LOCK_NB)) {
		if(file)
			close(file);
		return nullptr;
	}
	DeleteOld();
	return (void*)file;
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
	bool is_new = true;
	int previous_percent = 0;
	void* payload = nullptr;
	const char* filename = nullptr;
};

int progress_callback(void* ptr, curl_off_t TotalToDownload, curl_off_t NowDownloaded, curl_off_t TotalToUpload, curl_off_t NowUploaded) {
	Payload* payload = reinterpret_cast<Payload*>(ptr);
	if(payload && payload->callback) {
		int percentage = 0;
		if(TotalToDownload > 0.0) {
			double fractiondownloaded = (double)NowDownloaded / (double)TotalToDownload;
			percentage = std::round(fractiondownloaded * 100);
		}
		if(percentage != payload->previous_percent) {
			payload->callback(percentage, payload->current, payload->total, payload->filename, payload->is_new, payload->payload);
			payload->is_new = false;
			payload->previous_percent = percentage;
		}
	}
	return 0;
}

size_t WriteCallback(char *contents, size_t size, size_t nmemb, void *userp) {
	size_t realsize = size * nmemb;
	auto buff = static_cast<std::vector<char>*>(userp);
	size_t prev_size = buff->size();
	buff->resize(prev_size + realsize);
	memcpy((char*)buff->data() + prev_size, contents, realsize);
	return realsize;
}

CURLcode curlPerform(const char* url, void* payload, void* payload2 = nullptr) {
	CURL* curl_handle = curl_easy_init();
	curl_easy_setopt(curl_handle, CURLOPT_URL, url);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, reinterpret_cast<void*>(WriteCallback));
	curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, 5L);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, payload);
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, USERAGENT);
	curl_easy_setopt(curl_handle, CURLOPT_NOPROXY, "*");
	curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl_handle, CURLOPT_DNS_CACHE_TIMEOUT, 0);
	curl_easy_setopt(curl_handle, CURLOPT_XFERINFOFUNCTION, progress_callback);
	curl_easy_setopt(curl_handle, CURLOPT_XFERINFODATA, payload2);
	curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 0L);
	CURLcode res = curl_easy_perform(curl_handle);
	curl_easy_cleanup(curl_handle);
	return res;
}

void CheckUpdate() {
	std::vector<char> retrieved_data;
	if(curlPerform(UPDATE_URL, &retrieved_data) != CURLE_OK)
		return;
	try {
		nlohmann::json j = nlohmann::json::parse(retrieved_data);
		if(!j.is_array())
			return;
		for(auto& asset : j) {
			try {
				auto url = asset["url"].get<std::string>();
				auto name = asset["name"].get<std::string>();
				auto md5 = asset["md5"].get<std::string>();
				update_urls.push_back({ name, url, md5 });
			}
			catch(...) {}
		}
		has_update = !!update_urls.size();
	}
	catch(...) { update_urls.clear(); }
}

void ygo::updater::CheckUpdates() {
	if(!(Lock = GetLock()))
		return;
	update_urls.clear();
	std::thread(CheckUpdate).detach();
}

bool ygo::updater::HasUpdate() {
	return has_update;
}

bool ygo::updater::UpdateDownloaded() {
	return downloaded;
}

void Reboot(){
#ifdef _WIN32
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	auto pathstring = GetExePath() + EPRO_TEXT(" show_changelog");
	CreateProcess(nullptr, (LPWSTR)pathstring.c_str(), nullptr, nullptr, false, 0, nullptr, EPRO_TEXT("./"), &si, &pi);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
#elif defined(__APPLE__)
	system("open -b io.github.edo9300.ygoprodll --args show_changelog");
#else
	pid_t pid = fork();
	if(pid == 0) {
		execl(GetExePath().c_str(), "show_changelog", nullptr);
		exit(EXIT_FAILURE);
	}
#endif
	exit(0);
}

void ygo::updater::StartUnzipper(const path_string& src) {
#if defined(_WIN32) || (defined(__LINUX__) && !defined(__ANDROID__))
	auto pathstring = GetExePath() + EPRO_TEXT(".old");
	_trename(GetExePath().c_str(), pathstring.c_str());
#endif
	for(auto& file : update_urls) {
		auto name = src + ygo::Utils::ToPathString(file.name);
		ygo::Utils::UnzipArchive(src + ygo::Utils::ToPathString(file.name));
	}
	Reboot();
}

bool CheckMd5(const std::vector<char>& buffer, const std::vector<uint8_t>& md5) {
	return true;
	if(md5.size() < MD5_DIGEST_LENGTH)
		return false;
	uint8_t result[MD5_DIGEST_LENGTH];
	MD5((uint8_t*)buffer.data(), buffer.size(), result);
	return memcmp(result, md5.data(), MD5_DIGEST_LENGTH) == 0;
}

void DownloadUpdate(path_string dest_path, void* payload, update_callback callback) {
	downloading = true;
	Payload cbpayload{};
	cbpayload.callback = callback;
	cbpayload.total = static_cast<int>(update_urls.size());
	cbpayload.payload = payload;
	int i = 1;
	for(auto& file : update_urls) {
		auto name = dest_path + EPRO_TEXT("/") + ygo::Utils::ToPathString(file.name);
		cbpayload.current = i++;
		cbpayload.filename = file.name.data();
		cbpayload.is_new = true;
		cbpayload.previous_percent = -1;
		std::vector<uint8_t> binmd5;
		for(std::string::size_type i = 0; i < file.md5.length(); i += 2) {
			uint8_t b = static_cast<uint8_t>(strtoul(file.md5.substr(i, 2).c_str(), nullptr, 16));
			binmd5.push_back(b);
		}
		{
			std::ifstream file(name, std::ifstream::binary);
			if(file.good() && CheckMd5({ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() }, binmd5))
				continue;
		}
		std::vector<char> buffer;
		if((curlPerform(file.url.c_str(), &buffer, &cbpayload) != CURLE_OK)
		   || !CheckMd5(buffer, binmd5)
		   || !ygo::Utils::CreatePath(name))
			continue;
		std::ofstream stream(name, std::ofstream::binary);
		if(stream.good())
			stream.write(buffer.data(), buffer.size());
	}
	downloaded = true;
}

bool ygo::updater::StartUpdate(update_callback callback, void* payload, const path_string& dest) {
	if(!has_update || downloading)
		return false;
	std::thread(DownloadUpdate, dest, payload, callback).detach();
	return true;
}
#endif