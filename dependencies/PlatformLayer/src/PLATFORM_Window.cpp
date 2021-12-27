#include "PLATFORM_Assert.h"
#include "PLATFORM_Window.h"
#include "PLATFORM_Thread.h"


namespace PLATFORM{
  #define MAX_EVENTS 300
  EventBuffer swapEventBuffer;
  Mutex       eventBufferMutex;
  
  struct WindowCreationArgs{
    const char* title;
    PLATFORM::PlatWindow* window;
    Fence*                fence;
    bool (*WindowCreationCallback)(PlatWindow* windowHandle);
  };
  
  static void PushWindowEvent(WindowEvent event){
    Lock_Mutex(&eventBufferMutex);
    ASSERT(!(swapEventBuffer.count == swapEventBuffer.size));
    
    swapEventBuffer.events[swapEventBuffer.count] = event;
    swapEventBuffer.count++;
    
    Unlock_Mutex(&eventBufferMutex);
  }
};

#if defined(_WIN32)
#include "PLATFORM_Window_win32.cpp"
#elif defined(__linux__)
#include "PLATFORM_Window_linux.cpp"
#endif

namespace PLATFORM{
  
  void PlatWindow::UpdateEventBuffer(){
    Lock_Mutex(&eventBufferMutex);

    this->eventBuffer.count = swapEventBuffer.count;
    memcpy(this->eventBuffer.events, swapEventBuffer.events, sizeof(WindowEvent) * this->eventBuffer.count);
    swapEventBuffer.count = 0;

    Unlock_Mutex(&eventBufferMutex);
  }
  bool Create_Window(const char* title,
                     bool (*WindowCreationCallback)(PlatWindow*),
                     PlatWindow* window){
    
    eventBufferMutex = Create_Mutex();
    window->eventBuffer.size = 300;
    swapEventBuffer.size = 300;
    
    window->eventBuffer.events = (WindowEvent*)malloc(sizeof(WindowEvent) * 300);
    swapEventBuffer.events = (WindowEvent*)malloc(sizeof(WindowEvent) * 300);
    
    Fence fence = Create_Fence();
    WindowCreationArgs args = {title,
                               window,
                               &fence,
                               WindowCreationCallback};

    Thread thread = Create_Thread(CreateWindowProc, &args);
    if(!thread.handle) return false;
    
    WaitOn_Fence(&fence);
    Destroy_Fence(&fence);
    
    return ((u64)window->handle != 0);
  }


};
