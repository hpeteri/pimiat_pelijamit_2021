#include <windows.h>
#include <intrin.h>

#include "PLATFORM_Thread.h"

namespace PLATFORM{
  u32 GetProcessorCount(){return GetMaximumProcessorCount(ALL_PROCESSOR_GROUPS);}

  ThreadID Thread_GetID(){return GetCurrentThreadId();}
  Thread Create_Thread(ThreadProc_RType (*ThreadProc)(void*), void* args){
    DWORD id;
    Thread thread = {};
    thread.handle = CreateThread(NULL,
                                 0,
                                 ThreadProc,
                                 args,
                                 0,
                                 &id);
    
    return thread;
  }
  bool Destroy_Thread(Thread* thread){
    if(!thread->handle) return false;
    BOOL success = TerminateThread(thread->handle, 0);
    if(!success){
      return false;
    }
    
    CloseHandle(thread->handle);
    thread->handle = nullptr;
    return true;
  }
  bool WaitFor_Thread(Thread* thread){
    if(!thread->handle) return false;
    auto error = WaitForSingleObject(thread->handle, INFINITE);
    return true;
  }
  ////////////////////////////////////////////////////////////////////////////////////  
#define CHECK_ERROR()
  ///////////////////////////////////////////////////////////////////////////////
  //Mutex
  Mutex Create_Mutex(){
    Mutex mutex = {};
    mutex.handle = CreateMutex(NULL,
                               false,
                               NULL);
    return mutex;
  }
  void Destroy_Mutex(Mutex* mutex){
    CloseHandle(mutex->handle);
  }
  void Lock_Mutex(Mutex* mutex){
    WaitForSingleObject(mutex->handle, INFINITE);
  }
  void Unlock_Mutex(Mutex* mutex){
    ReleaseMutex(mutex->handle);
  }
  Semaphore Create_Semaphore(u32 initialCount, u32 maxCount){
    auto handle = CreateSemaphore(NULL, initialCount, maxCount, NULL);
    
    Semaphore semaphore;
    semaphore.handle = handle;
    return semaphore;
  }
  void Destroy_Semaphore(Semaphore* sem){
    CloseHandle(sem->handle);
  }
  void Lock_Semaphore(Semaphore* sem){
    WaitForSingleObject(sem->handle, INFINITE);
  }
  void Unlock_Semaphore(Semaphore* sem){
    ReleaseSemaphore(sem->handle, 1, NULL);
  }
  
  Fence Create_Fence(){
    Fence fence = {};
    fence.handle = CreateEvent(NULL, FALSE, FALSE, NULL);
    
    return fence;
  }
  void Destroy_Fence(Fence* fence){
    CloseHandle(fence->handle);
  }
  void Signal_Fence(Fence* fence){
    SetEvent(fence->handle);
  }
  void WaitOn_Fence(Fence* fence){
    WaitForSingleObject(fence->handle, INFINITE);
  }  

#undef CHECK_ERROR

  
};
