#include "windbot.h"
#include "utils.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#elif defined(__ANDROID__)
#include "Android/porting_android.h"
#include <nlohmann/json.hpp>
#else
#include <sys/wait.h>
#include <unistd.h>
#endif
#include <fmt/format.h>
#ifndef __ANDROID__
#include <fstream>
#include "Base64.h"
#endif
#include "config.h"
#include "bufferio.h"
#include "logging.h"

namespace ygo {

#if !defined(_WIN32) && !defined(__ANDROID__)
epro::path_string WindBot::executablePath{};
#endif
static constexpr uint32_t version{ CLIENT_VERSION };
#ifndef __ANDROID__
nlohmann::ordered_json WindBot::databases{};
bool WindBot::serialized{ false };
#ifdef _WIN32
std::wstring WindBot::serialized_databases{};
#else
std::string WindBot::serialized_databases{};
#endif
#endif

WindBot::launch_ret_t WindBot::Launch(int port, epro::wstringview pass, bool chat, int hand, const wchar_t* overridedeck) const {
#ifndef __ANDROID__
	if(!serialized) {
		serialized = true;
		serialized_databases = base64_encode<decltype(serialized_databases)>(databases.dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace));
	}
#endif
#ifdef _WIN32
	//Windows can modify this string
	auto args = Utils::ToPathString(fmt::format(
		L"WindBot.exe HostInfo=\"{}\" Deck=\"{}\" Port={} Version={} name=\"[AI] {}\" Chat={} Hand={} DbPaths={}{} AssetPath=./WindBot",
		pass, deck, port, version, name, chat, hand, serialized_databases, overridedeck ? fmt::format(L" DeckFile=\"{}\"", overridedeck) : L""));
	STARTUPINFO si{ sizeof(si) };
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	PROCESS_INFORMATION pi;
	if(!CreateProcess(EPRO_TEXT("./WindBot/WindBot.exe"), &args[0], nullptr, nullptr, false, 0, nullptr, nullptr, &si, &pi))
		return false;
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return true;
#elif defined(__ANDROID__)
	nlohmann::json param({
							{"HostInfo", BufferIO::EncodeUTF8(pass)},
							{"Deck", BufferIO::EncodeUTF8(deck)},
							{"Port", fmt::to_string(port)},
							{"Version", fmt::to_string(version)},
							{"Name", BufferIO::EncodeUTF8(name)},
							{"Chat", fmt::to_string(static_cast<int>(chat))},
							{"Hand", fmt::to_string(hand)}
						  });
	if(overridedeck)
		param["DeckFile"] = BufferIO::EncodeUTF8(overridedeck);
	porting::launchWindbot(param.dump());
	return true;
#else
	std::string argPass = fmt::format("HostInfo={}", BufferIO::EncodeUTF8(pass));
	std::string argDeck = fmt::format("Deck={}", BufferIO::EncodeUTF8(deck));
	std::string argPort = fmt::format("Port={}", port);
	std::string argVersion = fmt::format("Version={}", version);
	std::string argName = fmt::format("name=[AI] {}", BufferIO::EncodeUTF8(name));
	std::string argChat = fmt::format("Chat={}", chat);
	std::string argHand = fmt::format("Hand={}", hand);
	std::string argDbPaths = fmt::format("DbPaths={}", serialized_databases);
	std::string argDeckFile;
	if(overridedeck)
		argDeckFile = fmt::format("DeckFile={}", BufferIO::EncodeUTF8(overridedeck));
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
			   argDbPaths.data(), "AssetPath=./WindBot", argHand.data(), overridedeck ? argDeckFile.data() : nullptr, nullptr);
		_exit(EXIT_FAILURE);
	}
	if(executablePath.size())
		setenv("PATH", oldpath.data(), true);
	if(pid < 0 || waitpid(pid, nullptr, WNOHANG) != 0)
		pid = 0;
	return pid;
#endif
}

void WindBot::AddDatabase(epro::path_stringview database) {
#ifdef __ANDROID__
	porting::addWindbotDatabase(Utils::GetAbsolutePath(database));
#else
	serialized = false;
	databases.push_back(Utils::ToUTF8IfNeeded(Utils::GetAbsolutePath(database)));
#endif
}

}
