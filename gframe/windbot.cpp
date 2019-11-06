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


inline char* ptr(const std::string& str) {
	return const_cast<char*>(str.c_str());
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
	const std::vector<char*> args = {
		const_cast<char*>("mono"),
		const_cast<char*>("WindBot.exe"),
		ptr(fmt::format("Deck={}", BufferIO::EncodeUTF8s(deck))),
		ptr(fmt::format("Port={}", port)),
		ptr(fmt::format("Version={}", version)),
		ptr(fmt::format("name=[AI] {}", BufferIO::EncodeUTF8s(name))),
		ptr(fmt::format("Chat={}", chat)),
		hand ? ptr(fmt::format("Hand={}", hand)) : nullptr,
		nullptr
	};
	int pid = fork();
	if (pid == 0) {
		chdir("WindBot");
		execvp(args.front(), args.data());
		perror("Failed to start WindBot");
		exit(EXIT_FAILURE);
	}
	return pid > 0;
#endif
}

}
