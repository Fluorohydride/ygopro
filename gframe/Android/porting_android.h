#ifndef PORTING_ANDROID_H
#define PORTING_ANDROID_H

#include <vector>
#include <string>
#include "../text_types.h"
#include <IEventReceiver.h> //irr::SEvent

namespace irr {
class IrrlichtDevice;
}

struct android_app;

namespace porting {
/** java app **/
extern android_app* app_global;

extern std::string internal_storage;

bool transformEvent(const irr::SEvent& event, bool& stopPropagation);

void showComboBox(const std::vector<std::string>& list);

int getLocalIP();

void launchWindbot(path_stringview args);

void installUpdate(path_stringview args);

void openUrl(path_stringview url);

void openFile(path_stringview file);

void setTextToClipboard(epro_wstringview text);

const wchar_t* getTextFromClipboard();
}

#endif //PORTING_ANDROID_H