#ifndef CLIENT_UPDATER_H
#define CLIENT_UPDATER_H

#include "text_types.h"

using update_callback = void(*)(int percentage, int cur, int tot, const char* filename, void* payload);

namespace ygo {
namespace updater {

void CheckUpdates();
bool HasUpdate();
bool StartUpdate(update_callback callback, void* payload, const path_string& dest = EPRO_TEXT("./updates"));
bool UpdateDownloaded();
void StartUnzipper();

};
};

#endif //CLIENT_UPDATER_H
