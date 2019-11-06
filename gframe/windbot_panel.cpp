#include "windbot_panel.h"
#include <fmt/format.h>

namespace ygo {

void WindBotPanel::Refresh() {
	deckBox->clear();
	for (auto& bot : bots)
		deckBox->addItem(bot.name.c_str());
	UpdateDescription();
}

void WindBotPanel::UpdateDescription() {
	int sel = deckBox->getSelected();
	if (sel < 0 || sel >= bots.size())
		return;
	auto& bot = bots[sel];
	int level = bot.GetDifficulty();
	// TODO: Consider using a string buffer here instead and account for i18n
	std::wstring params;
	if (level > 0)
		params.append(fmt::format(L"AI Level: {}\n", level));
	else if (level == 0)
		params.append(L"Anti Meta AI\n");
	std::wstring mr;
	if (bot.flags & static_cast<int>(WindBot::Parameters::SUPPORT_MASTER_RULE_3))
		mr.append(L"3");
	if (bot.flags & static_cast<int>(WindBot::Parameters::SUPPORT_MASTER_RULE_4)) {
		if (mr.size())
			mr.append(L",");
		mr.append(L"4");
	}
	if (mr.size())
		params.append(fmt::format(L"Master Rule supported: {}\n", mr.c_str()));
	deckProperties->setText(params.c_str());
}

bool WindBotPanel::LaunchSelected(int port) {
	int sel = deckBox->getSelected();
	if (sel < 0 || sel >= bots.size()) return false;
	// 1 = scissors, 2 = rock, 3 = paper
	return bots[sel].Launch(port, !chkMute->isChecked(), chkThrowRock->isChecked() * 2);
}

}