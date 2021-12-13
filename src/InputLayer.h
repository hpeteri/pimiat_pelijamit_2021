#ifndef INPUTLAYER_H
#define INPUTLAYER_H

#include "Array.h"

enum VIRTUAL_KEY_FLAGS{
  VIRTUAL_KEY_FLAG_IS_DOWN = 1,
  VIRTUAL_KEY_FLAG_WAS_DOWN = 2,
};
struct VirtualKey{
  int keycode;
  int flags;
};
inline u64 GetVirtualKeyKeycode(VirtualKey key){return key.keycode;}
struct Mouse{
  int x, y;
  int x_delta, y_delta;
  int wheelDistance;
};  
struct InputState{
  Mouse      mouse;
  Array<VirtualKey> keyboardState;  
};
struct InputLayer{
  InputState    input;
};

bool InitInputLayer(InputLayer* input, void* (*Allocate0)(size_t), void (*Free0)(void*)){
  input->input.keyboardState.Init(Allocate0, Free0);
  return true;
};
static VirtualKey* FindKey(InputLayer* input, int keycode){
  return input->input.keyboardState.Search_Binary(VirtualKey{keycode, 0},
                                                  GetVirtualKeyKeycode);
}
static VirtualKey* InsertNewKey(InputLayer* input, int keycode){
  if((int)input->input.keyboardState.count >= (int)input->input.keyboardState.size){
    input->input.keyboardState.Expand();
  }
  auto index = input->input.keyboardState.GetIndexToInsertAt(VirtualKey{keycode, 0},
                                                             GetVirtualKeyKeycode);
  VirtualKey it = {0};
  it.keycode = keycode;
  auto ret = input->input.keyboardState.InsertAndKeepOrder(index, it);  
  
  return ret;
}

void HandleVirtualKey(InputLayer* input, int keycode, bool isDown){
  auto key = FindKey(input, keycode);
  if(!key){
    key = InsertNewKey(input, keycode);
  }
  if(isDown)
    key->flags |= VIRTUAL_KEY_FLAG_IS_DOWN;
  else
    key->flags &= VIRTUAL_KEY_FLAG_WAS_DOWN;
  
}
void UpdateInput_EndOfFrame(InputLayer* input){
  for(unsigned int i = 0; i < input->input.keyboardState.count; i++){
    auto it = input->input.keyboardState[i];
    int down = it->flags & VIRTUAL_KEY_FLAG_IS_DOWN;

    it->flags = ((it->flags << 1) & 0b11) | (it->flags & VIRTUAL_KEY_FLAG_IS_DOWN);
    it->flags |= down;
  }
}
bool IsKeyUp(InputLayer* input, int keycode){
  auto key = FindKey(input, keycode);
  if(!key) return false;
 
  return key->flags == VIRTUAL_KEY_FLAG_WAS_DOWN;
}
bool IsKeyDown(InputLayer* input, int keycode){
  auto key = FindKey(input, keycode);
  if(!key) return false;
  
  return key->flags == VIRTUAL_KEY_FLAG_IS_DOWN;

}
bool IsKeyHeld(InputLayer* input, int keycode){
  auto key = FindKey(input, keycode);
  if(!key) return false;
 
  return key->flags == (VIRTUAL_KEY_FLAG_WAS_DOWN | VIRTUAL_KEY_FLAG_IS_DOWN);
}

#endif
