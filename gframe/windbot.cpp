#include "windbot.h"
#include "utils.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <unistd.h>
#endif
#ifdef __ANDROID__
#include "Android/porting_android.h"
#endif
#include <fmt/format.h>
#include "config.h"
#include "bufferio.h"
#include "logging.h"

namespace ygo {

epro::path_string WindBot::executablePath{};
uint32_t WindBot::version{ CLIENT_VERSION };

#if defined(_WIN32) || defined(__ANDROID__)
bool WindBot::Launch(int port, const std::wstring& pass, bool chat, int hand) const {
#else
pid_t WindBot::Launch(int port, const std::wstring& pass, bool chat, int hand) const {
#endif
#ifdef _WIN32
	auto args = fmt::format(
		L"./WindBot/WindBot.exe HostInfo=\"{}\" Deck=\"{}\" Port={} Version={} name=\"[AI] {}\" Chat={} {}",
		pass,
		deck,
		port,
		version,
		name,
		chat,
		hand ? fmt::format(L"Hand={}", hand) : L"");
	STARTUPINFO si = {};
	PROCESS_INFORMATION pi = {};
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	if (CreateProcess(NULL, (TCHAR*)Utils::ToPathString(args).data(), NULL, NULL, FALSE, 0, NULL, executablePath.data(), &si, &pi)) {
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return true;
	}
	return false;
#elif defined(__ANDROID__)
	std::string param = fmt::format(
		"HostInfo='{}' Deck='{}' Port={} Version={} Name='[AI] {}' Chat={} Hand={}",
		BufferIO::EncodeUTF8s(pass),
		BufferIO::EncodeUTF8s(deck),
		port,
		version,
		BufferIO::EncodeUTF8s(name),
		static_cast<int>(chat),
		hand);
	porting::launchWindbot(param);
	return true;
#else
	std::string argPass = fmt::format("HostInfo={}", BufferIO::EncodeUTF8s(pass));
	std::string argDeck = fmt::format("Deck={}", BufferIO::EncodeUTF8s(deck));
	std::string argPort = fmt::format("Port={}", port);
	std::string argVersion = fmt::format("Version={}", version);
	std::string argName = fmt::format("name=[AI] {}", BufferIO::EncodeUTF8s(name));
	std::string argChat = fmt::format("Chat={}", chat);
	std::string argHand = fmt::format("Hand={}", hand);
	std::string oldpath = getenv("PATH");
	if(executablePath.length()) {
		std::string envPath = fmt::format("{}:{}", oldpath, executablePath);
		setenv("PATH", envPath.data(), true);
	}
	pid_t pid = vfork();
	if (pid == 0) {
		execlp("mono", "WindBot.exe", "./WindBot/WindBot.exe",
			   argPass.data(), argDeck.data(), argPort.data(), argVersion.data(), argName.data(), argChat.data(),
			   "AssetPath=./WindBot", hand ? argHand.data() : nullptr, nullptr);
		_exit(EXIT_FAILURE);
	}
	setenv("PATH", oldpath.data(), true);
	return pid;
#endif
}

}
