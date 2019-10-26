#ifndef EDOPRO_OSX_MENU_H
#define EDOPRO_OSX_MENU_H

#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif

void EDOPRO_SetupMenuBar(bool *fullScreenState);
void EDOPRO_ToggleFullScreen();

#ifdef __cplusplus
}
#endif

#endif /* EDOPRO_OSX_MENU_H */
