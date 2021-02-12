#include "windbot.h"
#include "utils.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#elif defined(__ANDROID__)
#include "Android/porting_android.h"
#else
#include <sys/wait.h>
#include <unistd.h>
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
#ifdef _WIN32
	//Windows can modify this string
	auto args = Utils::ToPathString(fmt::format(
		L"WindBot.exe HostInfo=\"{}\" Deck=\"{}\" Port={} Version={} name=\"[AI] {}\" Chat={} Hand={} AssetPath=./WindBot",
		pass,
		deck,
		port,
		version,
		name,
		chat,
		hand));
	STARTUPINFO si{ sizeof(si) };
	PROCESS_INFORMATION pi{};
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	if(CreateProcess(L"./WindBot/WindBot.exe", &args[0], nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return true;
	}
	return false;
#elif defined(__ANDROID__)
	std::string param = BufferIO::EncodeUTF8s(fmt::format(
		L"HostInfo='{}' Deck='{}' Port={} Version={} Name='[AI] {}' Chat={} Hand={}",
		pass,
		deck,
		port,
		version,
		name,
		static_cast<int>(chat),
		hand));
	porting::launchWindbot(param);
	return true;
#endif
}
#else
pid_t WindBot::Launch(int port, const std::wstring& pass, bool chat, int hand) const {
	std::string argPass = fmt::format("HostInfo={}", BufferIO::EncodeUTF8s(pass));
	std::string argDeck = fmt::format("Deck={}", BufferIO::EncodeUTF8s(deck));
	std::string argPort = fmt::format("Port={}", port);
	std::string argVersion = fmt::format("Version={}", version);
	std::string argName = fmt::format("name=[AI] {}", BufferIO::EncodeUTF8s(name));
	std::string argChat = fmt::format("Chat={}", chat);
	std::string argHand = fmt::format("Hand={}", hand);
	std::string oldpath;
	if(executablePath.size()) {
		oldpath = getenv("PATH");
		std::string envPath = fmt::format("{}:{}", oldpath, executablePath);
		setenv("PATH", envPath.data(), true);
	}
	auto pid = vfork();
	if(pid == 0) {
		execlp("mono", "WindBot.exe", "./WindBot/WindBot.exe",
			   argPass.data(), argDeck.data(), argPort.data(), argVersion.data(), argName.data(), argChat.data(),
			   "AssetPath=./WindBot", hand ? argHand.data() : nullptr, nullptr);
		_exit(EXIT_FAILURE);
	}
	if(waitpid(pid, nullptr, WNOHANG) != 0)
		pid = 0;
	if(executablePath.size())
		setenv("PATH", oldpath.data(), true);
	return pid;
}
#endif

}
