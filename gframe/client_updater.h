#ifndef CLIENT_UPDATER_H
#define CLIENT_UPDATER_H

#include "text_types.h"

namespace ygo {
namespace updater {

void CheckUpdates();
bool HasUpdate();
bool StartUpdate(void(*callback)(int percentage, void* payload), void* payload, const path_string& dest = EPRO_TEXT("./updates"));
bool UpdateDownloaded();

};
};

#endif //CLIENT_UPDATER_H
