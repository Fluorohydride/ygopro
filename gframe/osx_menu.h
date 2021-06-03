#ifndef EDOPRO_OSX_MENU_H
#define EDOPRO_OSX_MENU_H
#include <string>

void EDOPRO_SetupMenuBar(void (*callback)(void));
void EDOPRO_ToggleFullScreen();
std::string EDOPRO_GetWindowRect(void* _window);
void EDOPRO_SetWindowRect(void* _window, const char* rect_string);

#endif /* EDOPRO_OSX_MENU_H */
