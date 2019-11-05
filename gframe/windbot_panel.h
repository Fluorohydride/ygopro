#include <vector>
#include <irrlicht.h>
#include "windbot.h"

namespace ygo {

struct WindBotPanel {
    std::vector<WindBot> bots;

    irr::gui::IGUIWindow *window;
    irr::gui::IGUIComboBox *deckBox;
    irr::gui::IGUIButton *btnConfirm;
    irr::gui::IGUIButton *btnCancel;
    irr::gui::IGUIStaticText *deckProperties;

    void Refresh();
    void UpdateDescription();
    bool LaunchSelected(int port);
};

}
