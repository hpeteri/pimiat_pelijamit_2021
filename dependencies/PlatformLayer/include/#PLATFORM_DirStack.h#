#pragma once
#include "PLATFORM_Types.h"
#include "PLATFORM_Array.h"
namespace PLATFORM{
  
  struct DirStack{
    PLATFORM::Array<const char*> stack;
    
    void* (*Allocate)(size_t);
    void (*Free)(void*);
    //////////////////////////////////////////////////////////////////
    void Init(void* (*Allocate)(size_t), void (*Free)(void*));
    void Destroy();
    
    void PushExeDir();
    bool PushDir(const char* dir);
    void PopDir(u32 count = 1);
    
    const char* GetCurrentDir();
    const char* GetExeDir();
  };  
};
