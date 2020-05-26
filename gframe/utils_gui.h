#ifndef UTILS_GUI_H
#define UTILS_GUI_H

namespace irr {
class IrrlichtDevice;
}

namespace ygo {

class GameConfig;

namespace GUIUtils {

irr::IrrlichtDevice* CreateDevice(GameConfig* configs);
void ChangeCursor(irr::IrrlichtDevice* device, /*irr::gui::ECURSOR_ICON*/ int icon);
void TakeScreenshot(irr::IrrlichtDevice* device);
void ToggleFullscreen(irr::IrrlichtDevice* device, bool& fullscreen);

}

}

#endif
