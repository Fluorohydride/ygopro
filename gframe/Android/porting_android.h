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

void launchWindbot(epro::path_stringview args);

void addWindbotDatabase(epro::path_stringview args);

void installUpdate(epro::path_stringview args);

void openUrl(epro::path_stringview url);

void openFile(epro::path_stringview file);

void setTextToClipboard(epro::wstringview text);

const wchar_t* getTextFromClipboard();

void dispatchQueuedMessages();
}

#endif //PORTING_ANDROID_H