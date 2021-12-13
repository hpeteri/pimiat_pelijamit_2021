#include "PLATFORM_DirStack.h"
#include "PLATFORM_FileSystem.h"
namespace PLATFORM{  
  static char* GetExePath(void* (*Allocate)(size_t)){
    char* exePath = (char*)Allocate(MAX_PATH +  1);
    GetModuleFileName(NULL, exePath, MAX_PATH + 1);    
    return exePath;    
  }
};
