#pragma once
#include "PLATFORM_Types.h"

#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include <pthread.h>
#include <semaphore.h>
#endif

namespace PLATFORM{
#if defined(_WIN32)
  typedef DWORD ThreadProc_RType;
  typedef u64   ThreadID;
#elif defined(__linux__)
  typedef void* ThreadProc_RType;
  typedef pthread_t ThreadID;
#endif

  u32 GetProcessorCount();

  struct Thread{
#if defined(_WIN32)
    HANDLE handle;
#elif defined(__linux__)
    pthread_t handle;
#endif
  };
  
  Thread Create_Thread(ThreadProc_RType (*ThreadProc)(void*), void* args);
  
  bool   Destroy_Thread(Thread* thread);
  bool   WaitFor_Thread(Thread* thread);
  ThreadID Thread_GetID();
  
  struct Mutex{    
#if defined(_WIN32)
    HANDLE handle;
#elif defined(__linux__)
    pthread_mutex_t handle;
#endif
  };
  Mutex Create_Mutex(); //mutex should be unlocked
  void Destroy_Mutex(Mutex* mutex);
  void Lock_Mutex(Mutex*);
  void Unlock_Mutex(Mutex*);

  struct Semaphore{    
#if defined(_WIN32)
    HANDLE handle;
#elif defined(__linux__)
    sem_t handle;
#endif
    
  };
  Semaphore Create_Semaphore(u32 initialCount, u32 maxCount);
  void Destroy_Semaphore(Semaphore* sem);
  void Lock_Semaphore(Semaphore* sem);
  void Unlock_Semaphore(Semaphore* sem);

  struct Fence{
#if defined(_WIN32)
    HANDLE handle;
#elif defined(__linux__)
    Mutex mutex;
    pthread_cond_t cond;
#endif
  };
  Fence Create_Fence();
  void Destroy_Fence(Fence* fence);
  void Signal_Fence(Fence* fence);
  void WaitOn_Fence(Fence* fence);

};
