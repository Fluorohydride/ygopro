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
class IGUIEditBox;
}
}

namespace ygo {

struct SettingsPane {
	irr::gui::IGUICheckBox* chkIgnoreOpponents;
	irr::gui::IGUICheckBox* chkIgnoreSpectators;
	irr::gui::IGUICheckBox* chkQuickAnimation;
	irr::gui::IGUICheckBox* chkHideChainButtons;
	irr::gui::IGUICheckBox* chkAutoChainOrder;
	irr::gui::IGUICheckBox* chkNoChainDelay;
	irr::gui::IGUICheckBox* chkEnableSound;
	irr::gui::IGUIStaticText* stSoundVolume;
	irr::gui::IGUIScrollBar* scrSoundVolume;
	irr::gui::IGUICheckBox* chkEnableMusic;
	irr::gui::IGUIStaticText* stMusicVolume;
	irr::gui::IGUIScrollBar* scrMusicVolume;
	irr::gui::IGUIStaticText* stNoAudioBackend;
	irr::gui::IGUICheckBox* chkMAutoPos;
	irr::gui::IGUICheckBox* chkSTAutoPos;
	irr::gui::IGUICheckBox* chkRandomPos;
};

struct SettingsWindow {
	irr::gui::IGUIWindow* window;
	irr::gui::IGUICheckBox* chkShowFPS;
	irr::gui::IGUICheckBox* chkFullscreen;
	irr::gui::IGUICheckBox* chkScaleBackground;
	irr::gui::IGUICheckBox* chkAccurateBackgroundResize;
	irr::gui::IGUICheckBox* chkHideSetname;
	irr::gui::IGUICheckBox* chkHidePasscodeScope;
	irr::gui::IGUICheckBox* chkDrawFieldSpells;
	irr::gui::IGUICheckBox* chkFilterBot;
	irr::gui::IGUIStaticText* stCurrentSkin;
	irr::gui::IGUIComboBox* cbCurrentSkin;
	irr::gui::IGUIButton* btnReloadSkin;
	irr::gui::IGUIStaticText* stCurrentLocale;
	irr::gui::IGUIComboBox* cbCurrentLocale;
	irr::gui::IGUIStaticText* stDpiScale;
	irr::gui::IGUIEditBox* ebDpiScale;
	irr::gui::IGUIButton* btnRestart;
	
};

}

#endif
