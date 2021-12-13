#if defined(_WIN32)
#include <windows.h>

#define KEYCODE_R_BUTTON VK_RBUTTON
#define KEYCODE_L_BUTTON VK_LBUTTON
#define KEYCODE_M_BUTTON VK_MBUTTON

#elif defined(__linux__)
#include <X11/keysymdef.h>

#define KEYCODE_R_BUTTON 3
#define KEYCODE_L_BUTTON 1
#define KEYCODE_M_BUTTON 2

#endif




