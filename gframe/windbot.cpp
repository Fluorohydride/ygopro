#include "windbot.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <fmt/format.h>
#include "bufferio.h"

namespace ygo {

int WindBot::GetDifficulty() {
	if (flags & static_cast<int>(AI_LV1))
		return 1;
	if (flags & static_cast<int>(AI_LV2))
		return 2;
	if (flags & static_cast<int>(AI_LV3))
		return 3;
	if (flags & static_cast<int>(AI_ANTI_META))
		return 0;
	return -1;
}

bool WindBot::Launch(int port, bool chat, int hand) {
#ifdef _WIN32
	auto args = fmt::format(
		TEXT("./WindBot/WindBot.exe Deck=\"{}\" Port={} Version={} name=\"[AI] {}\" Chat={} {}"),
		deck.c_str(),
		port,
		version,
		name.c_str(),
		chat,
		hand ? fmt::format(TEXT("Hand={}"), hand) : TEXT(""));
	STARTUPINFO si = {};
	PROCESS_INFORMATION pi = {};
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	if (CreateProcess(NULL, (TCHAR*)args.c_str(), NULL, NULL, FALSE, 0, NULL, executablePath.c_str(), &si, &pi)) {
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return true;
	}
	return false;
#else
	int pid = fork();
	if (pid == 0) {
		std::string argDeck = fmt::format("Deck={}", BufferIO::EncodeUTF8s(deck).c_str());
		std::string argPort = fmt::format("Port={}", port);
		std::string argVersion = fmt::format("Version={}", version);
		std::string argName = fmt::format("name=[AI] {}", BufferIO::EncodeUTF8s(name).c_str());
		std::string argChat = fmt::format("Chat={}", chat);
		std::string argHand = fmt::format("Hand={}", hand);
		chdir("WindBot");
		execlp("mono", "WindBot.exe", "WindBot.exe", 
			argDeck.c_str(), argPort.c_str(), argVersion.c_str(), argName.c_str(), argChat.c_str(),
			hand ? argHand.c_str() : nullptr, nullptr);
		perror("Failed to start WindBot");
		exit(EXIT_FAILURE);
	}
	return pid > 0;
#endif
}

}
