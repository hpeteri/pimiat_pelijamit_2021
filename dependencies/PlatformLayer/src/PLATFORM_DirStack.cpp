#if defined(_WIN32)
#include "PLATFORM_DirStack_win32.cpp"
#elif defined(__linux__)
#include "PLATFORM_DirStack_linux.cpp"
#endif

#include "PLATFORM_Assert.h"
namespace PLATFORM{
  void DirStack::Init(void* (*Alloc)(size_t), void (*Fr)(void*)){
    *this = DirStack{};
    this->Allocate = Alloc;
    this->Free = Fr;

    char* path = GetExePath(Allocate);
    if(!GetParentDir(path)){
      printf("Failed to get executable path\n");
    }
    this->stack.Add(path, this->Allocate, this->Free );
  }
  void DirStack::Destroy(){
    for(u32 i = 0; i < this->stack.count; i++){
      this->Free((void*)this->stack.data[i]);
      
    }
    this->Free(this->stack.data);
    this->stack.count = 0;
    this->stack.size  = 0;
    this->stack.data  = nullptr;
  }
  
  void DirStack::PushExeDir(){
    const char* exePath = *this->stack[0];
    size_t len = strlen(exePath);
    
    char* path = (char*)this->Allocate(len + 1);
    memcpy(path, exePath, len + 1);
    
    this->stack.Add(path, this->Allocate, this->Free);
  }
  bool DirStack::PushDir(const char* dir){
    const char* currentDir = this->GetCurrentDir();
    size_t len = strlen(currentDir);

    size_t dirLen = strlen(dir);

    char* newPath = (char*)malloc(len + dirLen + 2);
    
    memcpy(newPath, currentDir, len);
    newPath[len] = '/';
    memcpy(&newPath[len + 1], dir, dirLen + 1);
    
    if(IsDir(newPath)){
      this->stack.Add(newPath, this->Allocate, this->Free);
      return true;
    }
    printf("failed to push :: %s from %s\n", newPath, currentDir);
    this->Free(newPath);
    return false;
        
  }
  void DirStack::PopDir(u32 count){
    for(u32 i = 0; i < count; i++){
      if(this->stack.count == 1) return;

      this->Free((void*)*this->stack.End());
      this->stack.count --;
    }    
  }
  const char* DirStack::GetCurrentDir(){
    return *this->stack.End();
  }
  const char* DirStack::GetExeDir(){
    return *this->stack[0];
  }
};
