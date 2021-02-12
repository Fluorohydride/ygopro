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
	std::vector<WindBot> bots;
#if !defined(_WIN32) && !defined(__ANDROID__)
	std::vector<pid_t> windbotsPids;
#endif

	irr::gui::IGUIWindow* window;
	irr::gui::IGUIComboBox* cbBotDeck;
	irr::gui::IGUICheckBox* chkThrowRock;
	irr::gui::IGUICheckBox* chkMute;
	irr::gui::IGUIStaticText* deckProperties;
	irr::gui::IGUIButton* btnAdd;

	int CurrentIndex();
	void Refresh(int filterMasterRule = 0, int lastIndex = 0);
	void UpdateDescription();
	bool LaunchSelected(int port, epro::wstringview pass);
};

}

#endif
