#include <sys/sysinfo.h>

#include "PLATFORM_Thread.h"
#include "PLATFORM_Assert.h"
namespace PLATFORM{
#define CHECK_ERROR() /*perror(__func__)*/; 


  u32 GetProcessorCount(){return get_nprocs();}
  
  ///////////////////////////////////////////////////////////////////////////////
  //Thread
  Thread Create_Thread(ThreadProc_RType (*ThreadProc)(void*), void* args){
    Thread thread;
    pthread_create(&thread.handle, nullptr, ThreadProc, args);

    return thread;
  }
  bool Destroy_Thread(Thread* thread){
    if(!thread->handle) return false;;
    
    int error = pthread_cancel(thread->handle);
    
    pthread_join(thread->handle, nullptr);
    thread->handle = 0;
    return true;
  }
  bool WaitFor_Thread(Thread* thread){
    if(!thread->handle) return false;
    
    int error = pthread_tryjoin_np(thread->handle, nullptr);
    if(error){
      error = pthread_join(thread->handle, nullptr);
    }    
    return !error;
  }
  ThreadID Thread_GetID(){
    return  pthread_self();
  }
  ///////////////////////////////////////////////////////////////////////////////
  //Mutex
  Mutex Create_Mutex(){
    Mutex mutex = {};
    i32 err = pthread_mutex_init(&mutex.handle, nullptr);
    if(err) CHECK_ERROR();
    return mutex;
  }
  void Destroy_Mutex(Mutex* mutex){
    i32 err = pthread_mutex_destroy(&mutex->handle);
    if(err) CHECK_ERROR();
  }
  void Lock_Mutex(Mutex* mutex){  
    i32 err = pthread_mutex_lock(&mutex->handle);
    if(err) CHECK_ERROR();
  }
  void Unlock_Mutex(Mutex* mutex){
    i32 err = pthread_mutex_unlock(&mutex->handle);
    if(err) CHECK_ERROR();
  }
  ///////////////////////////////////////////////////////////////////////////////
  //Semaphore
  Semaphore Create_Semaphore(u32 initialCount, u32 maxCount){
    sem_t handle;
    sem_init(&handle, 0, initialCount);
    
    Semaphore semaphore;
    semaphore.handle = handle;
    return semaphore;
  }
  void Destroy_Semaphore(Semaphore* sem){
    if(sem_close(&sem->handle))
      CHECK_ERROR();
  }
  void Lock_Semaphore(Semaphore* sem){
    if(sem_wait(&sem->handle))
      CHECK_ERROR();
  }
  void Unlock_Semaphore(Semaphore* sem){
    if(sem_post(&sem->handle))
      CHECK_ERROR();
  }
  ///////////////////////////////////////////////////////////////////////////////
  //Fence
  Fence Create_Fence(){
    Fence fence = {};
    fence.mutex = Create_Mutex();

    if(pthread_cond_init(&fence.cond, NULL))
      CHECK_ERROR();
    
    return fence;
  }
  void Destroy_Fence(Fence* fence){
    Destroy_Mutex(&fence->mutex);
  }
  void Signal_Fence(Fence* fence){
    auto result = pthread_cond_broadcast(&fence->cond);
    if(result)
      CHECK_ERROR();
  }
  void WaitOn_Fence(Fence* fence){
    Lock_Mutex(&fence->mutex);
    pthread_cond_wait(&fence->cond, &fence->mutex.handle);
  }
  ///////////////////////////////////////////////////////////////////////////////
#undef CHECK_ERROR
};
      
