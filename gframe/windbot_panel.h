#ifndef WINDBOT_PANEL_H
#define WINDBOT_PANEL_H

#include <vector>
#include "windbot.h"
#if !defined(_WIN32) && !defined(__ANDROID__)
#include <sys/types.h>
#endif

namespace irr {
namespace gui {
class IGUIWindow;
class IGUIComboBox;
class IGUICheckBox;
class IGUIStaticText;
class IGUIButton;
}
}

namespace ygo {

struct WindBotPanel {
	static std::wstring absolute_deck_path;
	std::vector<WindBot> bots;
#if !defined(_WIN32) && !defined(__ANDROID__)
	std::vector<pid_t> windbotsPids;
#endif

	WindBot* genericEngine;

	irr::gui::IGUIWindow* window;
	irr::gui::IGUIComboBox* cbBotDeck;
	irr::gui::IGUIComboBox* cbBotEngine;
	irr::gui::IGUICheckBox* chkThrowRock;
	irr::gui::IGUICheckBox* chkMute;
	irr::gui::IGUIStaticText* stBotEngine;
	irr::gui::IGUIStaticText* deckProperties;
	irr::gui::IGUIButton* btnAdd;

	int CurrentIndex();
	int CurrentEngine();
	void Refresh(int filterMasterRule = 0, int lastIndex = 0);
	void UpdateDescription();
	void UpdateEngine();
	bool LaunchSelected(int port, epro::wstringview pass);
private:
	int genericEngineIdx;
};

}

#endif
