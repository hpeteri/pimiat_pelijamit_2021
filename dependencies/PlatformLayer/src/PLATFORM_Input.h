#pragma once

#include "PLATFORM_Types.h"
#include "PLATFORM_Window.h"
#include "PLATFORM_Input_Keycodes.h"
namespace PLATFORM{
  
  struct Mouse{
    struct{
      i32 x,y;
    } position;
    struct {
      i32 x, y;
    } delta;
    i16 wheelDistance;
  };
  struct VirtualButton{
    bool wasDown; //State of previous frame
    bool isDown;
  };
  struct InputLayer{
    Mouse mouse;
    VirtualButton keyboard[(u16)-1]; 
  };

  void Init_InputLayer(InputLayer* input);
  void PollInput(InputLayer* input, PlatWindow* window);
  void UpdateVirtualKeyboard(InputLayer* input);
  void UpdateVirtualKey(InputLayer* input, u16 keycode, bool down);
  
  bool IsKeyDown(InputLayer* input, u16 keycode);
  bool IsKeyUp(InputLayer* input, u16 keycode);
  bool IsKeyHeld(InputLayer* input, u16 keycode);
};
