#ifndef SETTINGS_WINDOW_H
#define SETTINGS_WINDOW_H

namespace irr {
namespace gui {
class IGUIWindow;
class IGUICheckBox;
class IGUIStaticText;
class IGUIScrollBar;
class IGUIComboBox;
class IGUIButton;
}
}

namespace ygo {

struct SettingsWindow {
	irr::gui::IGUIWindow* window;
	irr::gui::IGUICheckBox* chkShowFPS;
	irr::gui::IGUICheckBox* chkEnableSound;
	irr::gui::IGUIStaticText* stSoundVolume;
	irr::gui::IGUIScrollBar* scrSoundVolume;
	irr::gui::IGUICheckBox* chkEnableMusic;
	irr::gui::IGUIStaticText* stMusicVolume;
	irr::gui::IGUIScrollBar* scrMusicVolume;
	irr::gui::IGUIStaticText* stNoAudioBackend;
	irr::gui::IGUIStaticText* stCurrentSkin;
	irr::gui::IGUIComboBox* cbCurrentSkin;
	irr::gui::IGUIButton* btnReloadSkin;
};

}

#endif
