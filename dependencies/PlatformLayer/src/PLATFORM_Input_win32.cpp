#include <windows.h>
#include "PLATFORM_Window.h"
namespace PLATFORM{
  void PollInput(InputLayer* input, PlatWindow* window){
    POINT pos;

    HWND windowHandle = window->handle;
    u32 windowHeight = window->size.y;
    GetCursorPos(&pos);
    ScreenToClient(windowHandle, &pos);
    
    input->mouse.delta.x = pos.x - input->mouse.position.x;
    input->mouse.delta.y = (windowHeight - pos.y) - input->mouse.position.y;
    
    input->mouse.position.x = pos.x;
    input->mouse.position.y = windowHeight - pos.y;
    
    char keyboard[256];

    for(int keycode = 0; keycode < 256; keycode++){
      keyboard[keycode] = (bool)(GetAsyncKeyState(keycode) & (1 << 15));
      
      VirtualButton* key = &input->keyboard[keycode];
      bool isDown = keyboard[keycode];
                  
      //if(key->wasDown != isDown)
        //key->transitionCount ++;
    }


    
  }

};
