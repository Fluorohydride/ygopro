#include "windbot_panel.h"
#include <sstream>
#include <fmt/format.h>
#include "data_manager.h"

namespace ygo {

void WindBotPanel::Refresh(int filterMasterRule) {
	deckBox->clear();
	for (int i = 0; i < bots.size(); i++) {
		const auto& bot = bots[i];
		switch (filterMasterRule) {
		case 0:
			deckBox->addItem(bot.name.c_str(), i);
			break;
		case 3:
			if (bot.SupportsMasterRule3())
				deckBox->addItem(bot.name.c_str(), i);
			break;
		case 4:
			if (bot.SupportsMasterRule4())
				deckBox->addItem(bot.name.c_str(), i);
			break;
		default:
			break;
		}
	}
	UpdateDescription();
}

void WindBotPanel::UpdateDescription() {
	int selected = deckBox->getSelected();
	if (selected < 0) {
		deckProperties->setText(L"");
		return;
	}
	auto& bot = bots[deckBox->getItemData(selected)];
	int level = bot.GetDifficulty();
	std::wstringstream params;
	if (level > 0)
		params << fmt::format(dataManager.GetSysString(2055), level);
	else if (level == 0)
		params << dataManager.GetSysString(2056);
	params << L"\n";
	std::wstring mr; // Due to short string optimization, a stream is not needed
	if (bot.SupportsMasterRule3())
		mr.append(L"3");
	if (bot.SupportsMasterRule4()) {
		if (mr.size())
			mr.append(L",");
		mr.append(L"4");
	}
	if (mr.size()) {
		params << fmt::format(dataManager.GetSysString(2057), mr.c_str());
		params << L"\n";
	}
	deckProperties->setText(params.str().c_str());
}

bool WindBotPanel::LaunchSelected(int port) {
	int sel = deckBox->getSelected();
	if (sel < 0 || sel >= bots.size()) return false;
	// 1 = scissors, 2 = rock, 3 = paper
	return bots[sel].Launch(port, !chkMute->isChecked(), chkThrowRock->isChecked() * 2);
}

}