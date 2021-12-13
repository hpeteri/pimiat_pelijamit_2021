#include <limits.h>
#include <libgen.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "PLATFORM_DirStack.h"
#include "PLATFORM_FileSystem.h" //IsDir()

#include "PLATFORM_Assert.h"


namespace PLATFORM{  
  static char* GetExePath(void* (*Allocate)(size_t)){ 
    char arg1[20];
    char* exePath = (char*)Allocate(PATH_MAX +  1);
    
    sprintf(arg1, "/proc/%d/exe", getpid());
    readlink(arg1, exePath, PATH_MAX + 1);
    return exePath;
  }
};
