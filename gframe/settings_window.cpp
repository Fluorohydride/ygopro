#include "settings_window.h"
#include <IGUICheckBox.h>
#include <IGUIScrollBar.h>
#include <IGUIStaticText.h>

namespace ygo {

void SettingsPane::DisableAudio() {
	chkEnableSound->setVisible(false);
	stSoundVolume->setVisible(false);
	scrSoundVolume->setVisible(false);
	chkEnableMusic->setVisible(false);
	stMusicVolume->setVisible(false);
	scrMusicVolume->setVisible(false);
	stNoAudioBackend->setVisible(true);
}

void SettingsWindow::DisableAudio() {
	chkEnableSound->setVisible(false);
	stSoundVolume->setVisible(false);
	scrSoundVolume->setVisible(false);
	chkEnableMusic->setVisible(false);
	stMusicVolume->setVisible(false);
	scrMusicVolume->setVisible(false);
	chkLoopMusic->setVisible(false);
	stNoAudioBackend->setVisible(true);
}

}
