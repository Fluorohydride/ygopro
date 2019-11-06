#ifndef WINDBOT_H
#define WINDBOT_H

#include <string>

namespace ygo {

struct WindBot {
	std::wstring name;
	std::wstring deck;
	enum Parameters {
		AI_LV1 = 1,
		AI_LV2 = 2,
		AI_LV3 = 4,
		AI_ANTI_META = 8,
		SUPPORT_MASTER_RULE_3 = 16,
		SUPPORT_MASTER_RULE_4 = 32
	};
	int flags;

	int GetDifficulty();
	bool Launch(int port, bool chat = true, int hand = 0);

	// Can be made static
	unsigned short version;
	std::wstring executablePath;
};

}

#endif
