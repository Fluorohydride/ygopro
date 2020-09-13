#ifndef PORTING_ANDROID_H
#define PORTING_ANDROID_H

#include <vector>
#include <string>
#include <IEventReceiver.h> //irr::SEvent

namespace irr {
class IrrlichtDevice;
}

struct android_app;

namespace porting {
/** java app **/
extern android_app* app_global;

extern std::string internal_storage;
extern std::string working_directory;

bool transformEvent(const irr::SEvent& event, bool& stopPropagation);

void showComboBox(const std::vector<std::string>& list);

int getLocalIP();

void launchWindbot(const std::string& args);

void installUpdate(const std::string& args);

void openUrl(const std::string& url);

void openFile(const std::string& file);

void setTextToClipboard(const wchar_t* text);

const wchar_t* getTextFromClipboard();
}

#endif //PORTING_ANDROID_H