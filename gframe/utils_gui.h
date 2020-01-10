#ifndef UTILS_GUI_H
#define UTILS_GUI_H

#include <IrrlichtDevice.h>
#include <ICursorControl.h>

namespace ygo {

namespace GUIUtils {

void ChangeCursor(irr::IrrlichtDevice* device, irr::gui::ECURSOR_ICON icon);
void TakeScreenshot(irr::IrrlichtDevice* device);
void ToggleFullscreen(irr::IrrlichtDevice* device, bool& fullscreen);

}

}

#endif
