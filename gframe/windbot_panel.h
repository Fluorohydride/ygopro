#ifndef WINDBOT_PANEL_H
#define WINDBOT_PANEL_H

#include <vector>
#include "windbot.h"

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

	irr::gui::IGUIWindow* window;
	irr::gui::IGUIComboBox* deckBox;
	irr::gui::IGUICheckBox* chkThrowRock;
	irr::gui::IGUICheckBox* chkMute;
	irr::gui::IGUIStaticText* deckProperties;
	irr::gui::IGUIButton* btnAdd;

	void Refresh(int filterMasterRule = 0);
	void UpdateDescription();
	bool LaunchSelected(int port);
};

}

#endif
