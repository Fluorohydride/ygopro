#include "windbot_panel.h"
#include <sstream>
#include <fmt/format.h>
#include <IGUIComboBox.h>
#include <IGUIStaticText.h>
#include <IGUICheckBox.h>
#include "data_manager.h"

namespace ygo {

int WindBotPanel::CurrentIndex() {
	int selected = cbBotDeck->getSelected();
	return selected >= 0 ? cbBotDeck->getItemData(selected) : selected;
}

void WindBotPanel::Refresh(int filterMasterRule, int lastIndex) {
	int oldIndex = CurrentIndex();
	int lastBot = oldIndex >= 0 ? oldIndex : lastIndex;
	cbBotDeck->clear();
	for (size_t i = 0; i < bots.size(); i++) {
		const auto& bot = bots[i];
		if (filterMasterRule == 0 || bot.masterRules.find(filterMasterRule) != bot.masterRules.end()) {
			int newIndex = cbBotDeck->addItem(bot.name.data(), i);
			if (i == lastBot)
				cbBotDeck->setSelected(newIndex);
		}
	}
	UpdateDescription();
}

void WindBotPanel::UpdateDescription() {
	int index = CurrentIndex();
	if (index < 0) {
		deckProperties->setText(L"");
		return;
	}
	auto& bot = bots[index];
	std::wstringstream params;
	if (bot.difficulty != 0)
		params << fmt::format(gDataManager->GetSysString(2055), bot.difficulty);
	else
		params << gDataManager->GetSysString(2056);
	params << L"\n";
	if (bot.masterRules.size()) {
		std::wstring mr; // Due to short string optimization, a stream is not needed
		for (auto rule : bot.masterRules) {
			if (mr.size())
				mr.append(L",");
			mr.append(fmt::to_wstring(rule));
		}
		params << fmt::format(gDataManager->GetSysString(2057), mr);
		params << L"\n";
	}
	deckProperties->setText(params.str().data());
}

bool WindBotPanel::LaunchSelected(int port, const std::wstring& pass) {
	int index = CurrentIndex();
	if (index < 0) return false;
	// 1 = scissors, 2 = rock, 3 = paper
#if defined(_WIN32) || defined(__ANDROID__)
	return bots[index].Launch(port, pass, !chkMute->isChecked(), chkThrowRock->isChecked() * 2);
#else
	auto pid = bots[index].Launch(port, pass, !chkMute->isChecked(), chkThrowRock->isChecked() * 2);
	if(pid > 0) {
		windbotsPids.push_back(pid);
	}
	return pid > 0;
#endif
}

}
