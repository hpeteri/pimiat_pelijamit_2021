#pragma once
#include "PLATFORM_Types.h"

#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)      
#include <X11/Xlib.h>
#endif

namespace PLATFORM{
  enum WINDOW_EVENT_TYPE{
    EVENT_CHAR,
    EVENT_KEY_DOWN,
    EVENT_KEY_UP,
    EVENT_SIZE,
    EVENT_MOUSE_BUTTON_DOWN,
    EVENT_MOUSE_BUTTON_UP,
    EVENT_MOUSE_WHEEL,
    EVENT_MOUSE_MOTION,
    EVENT_QUIT,
  };
  struct WindowEvent{
    WINDOW_EVENT_TYPE type;
    union {
      struct{
        unsigned short keycode;
      } key;
      struct{
        int x, y;
      } size;
      struct {
        short distance;
      } mouseWheel;
      struct {
        int x,y;
      } mouse;
    };
  };
  struct EventBuffer{
    unsigned int count;
    unsigned int size;
    WindowEvent* events;
  };

  struct PlatWindow{
#if defined(_WIN32)
    HWND handle;
    void* instance;
#elif defined(__linux__)      
    Window   handle;
    Display* display;
#endif
    struct {
      unsigned int x;
      unsigned int y;
    } size;

    EventBuffer eventBuffer;
    void UpdateEventBuffer();
  };
  
  bool Create_Window(const char* title, bool (*WindowCreationCallback)(PlatWindow*), PlatWindow* window);
  
  void Destroy_Window(PlatWindow* window);
  bool Create_GLContext(PlatWindow* window);
  void SwapWindowBuffers(PlatWindow* window);
};
