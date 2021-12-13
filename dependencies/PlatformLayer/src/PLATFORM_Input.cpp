#include "PLATFORM_Input.h"

#if defined(_WIN32)
#include "PLATFORM_Input_win32.cpp"
#elif defined(__linux__)
#include "PLATFORM_Input_linux.cpp"
#endif

namespace PLATFORM{
  void Init_InputLayer(InputLayer* input){
    *input = {};
    printf("%c :: %c :: delta %d\n", 'A', 'a', 'A' - 'a');
  }
  void UpdateVirtualKeyboard(InputLayer* input){
    for(u32 i = 0; i < sizeof(input->keyboard) / sizeof(*input->keyboard); i++){
      VirtualButton* button = &input->keyboard[i];
      button->wasDown = button->isDown;
    }
    input->mouse.wheelDistance = 0;
  }
  void UpdateVirtualKey(InputLayer* input, u16 keycode, bool down){
    printf("Key update %u %c\n", keycode, (char)keycode);
    VirtualButton* button = &input->keyboard[keycode];
    button->isDown = down;    
  }
  bool IsKeyDown(InputLayer* input, u16 keycode){
    
#if defined(__linux__)
    if(keycode >= 'A' && keycode <= 'Z')
      keycode += 32;
#endif
    
    VirtualButton* button = &input->keyboard[keycode];
    bool result = (!button->wasDown && button->isDown);
    return result;
  }
  bool IsKeyUp(InputLayer* input, u16 keycode){

#if defined(__linux__)
    if(keycode >= 'A' && keycode <= 'Z')
      keycode += 32;

#endif
    VirtualButton* button = &input->keyboard[keycode];
    return (button->wasDown && !button->isDown);
  }
  bool IsKeyHeld(InputLayer* input, u16 keycode){

#if defined(__linux__)
    if(keycode >= 'A' && keycode <= 'Z'){
      keycode += 32;
    }
#endif
    VirtualButton* button = &input->keyboard[keycode];
    bool result = (button->wasDown && button->isDown);
    return result;
  }
};
