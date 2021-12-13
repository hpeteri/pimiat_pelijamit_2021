#include <windows.h>
#include <iostream>
#include <intrin.h>

#include "glew.h"
#include "wglew.h"
#include <GL/GL.h>

//#include "wglext.h"

namespace PLATFORM{
  static void  HandleWindowCreationError();      //Error handling function
  static DWORD CreateWindowProc(void* voidArgs); //Thread proc for creating window
  
  ////////////////////////////////////////////////////////////////
  LRESULT CALLBACK WindowCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam){
    LRESULT result = 0;
    switch(message){
    case WM_PAINT:
      {
        PAINTSTRUCT paint = {0};
        BeginPaint(window,&paint);
        EndPaint(window,&paint);
        break;
      }
    case WM_ERASEBKGND:
      return TRUE;
    case WM_DESTROY:
    case WM_CLOSE:
      {
        PushWindowEvent({EVENT_QUIT, 0});
        
        PostQuitMessage(0);
        break;
      }
    case WM_CHAR:
      {
        WindowEvent event;
        event.type = EVENT_CHAR;
        event.key.keycode = (char)wParam;
        PushWindowEvent(event);
        break;
      }
    case WM_MOUSEMOVE:
      {
        int y = (int)(lParam >> 16);
        int x = (int)(lParam & 0x0000ffff);
        
        WindowEvent event;
        event.type = EVENT_MOUSE_MOTION;
        event.mouse.x = x;
        event.mouse.y = y;
        PushWindowEvent(event);
        break;
      }
    case WM_MOUSEWHEEL:
      {
        WindowEvent event;
        event.type = EVENT_MOUSE_WHEEL;
        event.mouseWheel.distance = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
        PushWindowEvent(event);
        
        break;
      }
    case WM_SIZE:
      {
        RECT client;
        GetClientRect(window,&client);
        
        WindowEvent event;
        event.type = EVENT_SIZE;
        event.size.x = client.right;
        event.size.y = client.bottom;
        PushWindowEvent(event);

        break;
      }
  case WM_KEYDOWN:
  case WM_KEYUP:
    {
      WindowEvent event;
      
      bool wasDown = lParam & (1 << 30);
      if(wasDown && message == WM_KEYDOWN) break;
      
      event.type = (message == WM_KEYUP)? EVENT_KEY_UP : EVENT_KEY_DOWN;
      event.key.keycode = (unsigned short)wParam;
      PushWindowEvent(event);      
      break;
    }
    case WM_LBUTTONUP:
    case WM_LBUTTONDOWN:
      {
        WindowEvent event;
       
        bool wasDown = lParam & (1 << 30);
        if(wasDown && message == WM_KEYDOWN) break;
      
        event.type = (message == WM_LBUTTONUP)? EVENT_MOUSE_BUTTON_UP : EVENT_MOUSE_BUTTON_DOWN;
        event.key.keycode = 1;
        PushWindowEvent(event);      
        break;
      }
    case WM_RBUTTONUP:
    case WM_RBUTTONDOWN:
      {
       
        WindowEvent event;
        bool wasDown = lParam & (1 << 30);
        if(wasDown && message == WM_KEYDOWN) break;
      
        event.type = (message == WM_RBUTTONUP)? EVENT_MOUSE_BUTTON_UP : EVENT_MOUSE_BUTTON_DOWN;
        event.key.keycode = 3;
        PushWindowEvent(event);      
        break;
      }
    default:
      {
        result = DefWindowProc(window, message, wParam, lParam);  
        break;
      }
      
    };
    
    return result;
  }
  DWORD WINAPI CreateWindowProc(void* voidArgs){
    WindowCreationArgs* args = (WindowCreationArgs*)voidArgs;
    PlatWindow* window = args->window;
    const char* title = args->title;
    Fence* fence = args->fence;
    
    /////////////////////////////////////////////////////////
    //Create window
    HINSTANCE instance = GetModuleHandle(NULL);
    WNDCLASSEX windowClass        = {};
    windowClass.cbSize           = sizeof(WNDCLASSEX);
    windowClass.style            = CS_HREDRAW|CS_VREDRAW;
    windowClass.lpfnWndProc      = WindowCallback;
    windowClass.cbClsExtra       = NULL;
    windowClass.cbWndExtra       = NULL;
    windowClass.hInstance        = instance;
    windowClass.hIcon            = NULL;
    windowClass.hCursor          = NULL;//LoadCursor(NULL,IDC_ARROW);
    windowClass.hbrBackground    =(HBRUSH)(COLOR_WINDOW+1);
    windowClass.lpszClassName    = "WINDOW";

    RegisterClassEx(&windowClass);
    window->handle = CreateWindow(windowClass.lpszClassName,
                                     title,
                                     WS_OVERLAPPEDWINDOW,
                                     CW_USEDEFAULT,
                                     CW_USEDEFAULT,
                                     CW_USEDEFAULT,
                                     CW_USEDEFAULT,
                                     NULL,
                                     NULL,
                                     instance,
                                     NULL);

    /////////////////////////////////////////////////////////
    //Signal that window is created
  
    ////////////////////////////////////////////////////////////////
    window->instance = GetModuleHandle(0);
    if(!args->WindowCreationCallback(window)){
      if(window->handle){
        PostMessage(window->handle, WM_DESTROY, 0, 0);
      }
      Signal_Fence(fence);
      window->handle = 0;
      return 0;
    }
    Signal_Fence(fence);
        
    RECT client;
    GetClientRect(window->handle, &client);
    window->size = {(unsigned int)client.right, (unsigned int)client.bottom};
    ValidateRect(window->handle, &client);

        
    ShowWindow(window->handle, SW_SHOW);
    UpdateWindow(window->handle);
  
    /////////////////////////////////////////////////////////
    MSG msg;    
    for(;;){
      if(PeekMessage(&msg,0,0,0,PM_REMOVE)){
        if(msg.message == WM_QUIT){
          WindowEvent event;
          event.type = EVENT_QUIT;
          PushWindowEvent(event);
          break;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
      WaitMessage();
    }
    return 0;
  }
  void SwapWindowBuffers(PlatWindow* window){
    HDC dc = GetDC(window->handle);
    SwapBuffers(dc);
    ReleaseDC(window->handle, dc);
  }
  bool Create_GLContext(PlatWindow* window){
    HDC dc = GetDC(window->handle);
    
    //////////////////////////////////////////////////////////////////////
    //Create temporary dc
    PIXELFORMATDESCRIPTOR pfd = {
      sizeof(PIXELFORMATDESCRIPTOR),
      1,
      PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    // Flags
      PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
      32,                   // Colordepth of the framebuffer.
      0, 0, 0, 0, 0, 0,
      0,
      0,
      0,
      0, 0, 0, 0,
      24,                   // Number of bits for the depthbuffer
      8,                    // Number of bits for the stencilbuffer
      0,                    // Number of Aux buffers in the framebuffer.
      PFD_MAIN_PLANE,
      0,
      0, 0, 0
    };
    PIXELFORMATDESCRIPTOR givenPfd;
    i32 givenPfdIndex = ChoosePixelFormat(dc, &pfd);

    if(!SetPixelFormat(dc, givenPfdIndex, &givenPfd)){
      ReleaseDC(window->handle, dc);
      return false;
    }
    auto temp_glContext = wglCreateContext(dc);

    if(!wglMakeCurrent(dc, temp_glContext)){
      MessageBox(NULL,"Couldn't initialize opengl","ERROR", MB_ICONERROR);
      return false;
    }
    
    //////////////////////////////////////////////////////////////////////
    //Create 3.2 context
    if(glewInit() != GLEW_OK){
      wglDeleteContext(temp_glContext);
      return false;
    }
    const int pixelFormatAttribs[] = {
      WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
      WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
      WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
      WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
      WGL_COLOR_BITS_ARB, 32,
      WGL_DEPTH_BITS_ARB, 24,
      WGL_STENCIL_BITS_ARB, 8,
      0 // End of attributes list
    };
    int contextAttribs[] = {
      WGL_CONTEXT_MAJOR_VERSION_ARB, 3
      , WGL_CONTEXT_MINOR_VERSION_ARB, 2
      , WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
      , 0
    };
    
    int pixelFormat = 0;
    UINT numFormats = 0;
    
    wglChoosePixelFormatARB(dc, pixelFormatAttribs, NULL, 1, &pixelFormat, &numFormats);
    
    SetPixelFormat(dc, pixelFormat, &pfd);
    
    auto glContext = wglCreateContextAttribsARB(dc, 0, contextAttribs);
 
    if(!wglMakeCurrent(dc, glContext)){
      MessageBox(NULL,"Couldn't initialize opengl","ERROR", MB_ICONERROR);
    }
    ReleaseDC(window->handle, dc);
    wglDeleteContext(temp_glContext);
    return true;
  }
  void Destroy_Window(PlatWindow* window){
    
  }
};
    
