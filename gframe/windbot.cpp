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

#if !defined(_WIN32) && !defined(__ANDROID__)
epro::path_string WindBot::executablePath{};
#endif
uint32_t WindBot::version{ CLIENT_VERSION };

WindBot::launch_ret_t WindBot::Launch(int port, epro::wstringview pass, bool chat, int hand) const {
#ifdef _WIN32
	//Windows can modify this string
	auto args = Utils::ToPathString(fmt::format(
		L"WindBot.exe HostInfo=\"{}\" Deck=\"{}\" Port={} Version={} name=\"[AI] {}\" Chat={} Hand={} AssetPath=./WindBot",
		pass, deck, port, version, name, chat, hand));
	STARTUPINFO si{ sizeof(si) };
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	PROCESS_INFORMATION pi;
	if(!CreateProcess(L"./WindBot/WindBot.exe", &args[0], nullptr, nullptr, false, 0, nullptr, nullptr, &si, &pi))
		return false;
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return true;
#elif defined(__ANDROID__)
	auto param = BufferIO::EncodeUTF8(fmt::format(
		L"HostInfo='{}' Deck='{}' Port={} Version={} Name='[AI] {}' Chat={} Hand={}",
		pass, deck, port, version, name, static_cast<int>(chat), hand));
	porting::launchWindbot(param);
	return true;
#else
	std::string argPass = fmt::format("HostInfo={}", BufferIO::EncodeUTF8(pass));
	std::string argDeck = fmt::format("Deck={}", BufferIO::EncodeUTF8(deck));
	std::string argPort = fmt::format("Port={}", port);
	std::string argVersion = fmt::format("Version={}", version);
	std::string argName = fmt::format("name=[AI] {}", BufferIO::EncodeUTF8(name));
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
	if(executablePath.size())
		setenv("PATH", oldpath.data(), true);
	if(pid < 0 || waitpid(pid, nullptr, WNOHANG) != 0)
		pid = 0;
	return pid;
#endif
}

}
