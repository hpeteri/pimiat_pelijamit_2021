#include <X11/Xlib.h>
#include <cstring>
#include <pthread.h>
#include <cstdio>
#include <time.h>

#include "glew.h"
#include <GL/glx.h>
#include <GL/gl.h>


#include "PLATFORM_Window.h"
#include "PLATFORM_Input_Keycodes.h"


namespace PLATFORM{
  typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*,
                                                       GLXFBConfig,
                                                       GLXContext,
                                                       Bool,
                                                       const int*);

  ////////////////////////////////////////////////////////////////
  static unsigned short MaybeConvertToUpperCase(unsigned short c){
    if(c >= 'a' && c <= 'z'){
      c -= 'a' - 'A';
    }
    return c;
  }
  void HandleEvent(XEvent e){
    switch(e.type){
    case KeyPress:
      {
        KeySym baseKey = XLookupKeysym(&e.xkey, 0);
        KeySym charKey = XLookupKeysym(&e.xkey, e.xkey.state);

        WindowEvent keyEvent = {};
        keyEvent.type = EVENT_KEY_DOWN;
        keyEvent.key.keycode = MaybeConvertToUpperCase(baseKey);
        PushWindowEvent(keyEvent);
        
        
        if(charKey != NoSymbol){
          //We should check if key has a symbol
          WindowEvent charEvent = {};
          charEvent.type = EVENT_CHAR;
          charEvent.key.keycode = charKey;
        
          PushWindowEvent(charEvent);
        }
        break;
      }
    case KeyRelease:
      {
        KeySym baseKey = XLookupKeysym(&e.xkey, 0);
       
        WindowEvent keyEvent = {};
        keyEvent.type = EVENT_KEY_UP;
        keyEvent.key.keycode = MaybeConvertToUpperCase(baseKey);
        PushWindowEvent(keyEvent);
        break;
      }
    case ButtonPress:
      {
        WindowEvent buttonEvent = {};
        buttonEvent.type = (e.xbutton.button < 4) ? EVENT_MOUSE_BUTTON_DOWN : EVENT_MOUSE_WHEEL;

        if(e.xbutton.button <= 3){
          buttonEvent.key.keycode = e.xbutton.button;
        }else{
          buttonEvent.mouseWheel.distance = (e.xbutton.button == 4) * 1 - (e.xbutton.button == 5) * 1;
        }
        
        PushWindowEvent(buttonEvent);
                
        break;
      }
    case ButtonRelease:
      {
        WindowEvent buttonEvent = {};
        buttonEvent.type = EVENT_MOUSE_BUTTON_UP;
        
        if(e.xbutton.button <= 3){
          buttonEvent.key.keycode = e.xbutton.button;
          PushWindowEvent(buttonEvent);
        }        
                
        break;
      }
    case MotionNotify:
      {
        //We poll mouse position
        //printf("Motion\n");
        WindowEvent event;
        event.type = EVENT_MOUSE_MOTION;
        event.mouse.x = e.xmotion.x;
        event.mouse.y = e.xmotion.y;
      
        PushWindowEvent(event);      
        break;
      }
    case EnterNotify:
      //printf("focus on\n");
      break;
    case LeaveNotify:
      //printf("focus off\n");
      break;
    case FocusIn:
      //printf("Input focus on\n");
      break;
    case FocusOut:
      //printf("Input focus off\n");
    case Expose:
      {
        break;
      }
    case GraphicsExpose:
      //printf("GraphicsExposure\n");
      break;
    case NoExpose:
      //printf("No Exposure\n");
      break;
    case CirculateRequest:
      //printf("CirculateRequest\n");
      break;
    case ConfigureRequest:
      //printf("ConfigureRequest\n");
      break;
    case MapRequest:
      //printf("MapRequest\n");
      break;
    case ResizeRequest:
      //printf("ResizeRequest\n");
      break;
    case CirculateNotify:
      //printf("CirculateNotify\n");
      break;
    case ConfigureNotify:
      WindowEvent ev;
      ev.type = EVENT_SIZE;
      ev.size.x = e.xconfigure.width;
      ev.size.y = e.xconfigure.height;
      PushWindowEvent(ev);
      break;
    case CreateNotify:
      //printf("CreateNotify\n");
      break;
    case DestroyNotify:
      printf("Destroy\n");
      break;
    case GravityNotify:
      //printf("Gravity");
      break;
    case MapNotify:
      //printf("Map");
      break;
    case MappingNotify:
      //printf("Mapping");
      break;
    case ReparentNotify:
      //printf("Reparent");
      break;
    case UnmapNotify:
      //printf("Unmap");
      break;
    case VisibilityNotify:
      //printf("Visibility");
      break;
    case ColormapNotify:
      //printf("Colormap\n");
      break;
    case ClientMessage:
      PushWindowEvent({EVENT_QUIT});
      break;
    case PropertyNotify:
      //printf("PropertyNotify\n");
      break;
    case SelectionClear:
      //printf("SelectionClear\n");
      break;
    case SelectionNotify:
      //printf("SelectionNotify\n");
      break;
    case SelectionRequest:
      //printf("Selection Request\n");
      break;      
    default:
      //printf("Default :: %d\n", e.type);
      break;
    }    
  }
  void* CreateWindowProc(void* voidArgs){
    PLATFORM::WindowCreationArgs* args = (PLATFORM::WindowCreationArgs*)voidArgs;
    PlatWindow* window = args->window;
    const char* title = args->title;
    Fence* fence = args->fence;
    /////////////////////////////////////////////////////////
    XInitThreads();

    Display* display = XOpenDisplay(nullptr);
    if(!display){
      Signal_Fence(fence);
      return 0;
    }
    
    Screen* screen = DefaultScreenOfDisplay(display);
    int screenId = DefaultScreen(display);
    
    XSetWindowAttributes windowAttribs;
    windowAttribs.border_pixel = BlackPixel(display, screenId);
    windowAttribs.background_pixel = BlackPixel(display, screenId);
    windowAttribs.override_redirect = true;
    windowAttribs.colormap = XCreateColormap(display,
                                             RootWindow(display, screenId),
                                             XDefaultVisual(display, screenId),
                                             AllocNone);
    windowAttribs.event_mask = ExposureMask;
    Window w = XCreateWindow(display,
                             RootWindow(display, screenId),
                             0, 0,
                             400, 400,
                             0,
                             XDefaultDepth(display, screenId),
                             InputOutput,
                             XDefaultVisual(display, screenId),
                             CWBackPixel | CWColormap | CWBorderPixel | CWEventMask,
                             &windowAttribs);
    
    XStoreName(display, w, title);
    window->handle = w;
    window->display = display;
    if(!args->WindowCreationCallback(window)){
      window->handle = 0;
      Signal_Fence(fence);
      return 0;
    }

    /////////////////////////////////////////////////////////
    
    XSelectInput(display, w,
                 KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | EnterWindowMask |
                 LeaveWindowMask | PointerMotionMask | ButtonMotionMask | KeymapStateMask |
                 StructureNotifyMask | FocusChangeMask | ExposureMask);
    
    //Prevent window from closing
    //Atom wm_pc = XInternAtom(display, "WM_PROTOCOLS", false);
    Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", false);
    if(!XSetWMProtocols(display, w, &wm_delete_window, 1))
      perror("Protocol fail");
    
    XMapWindow(display, w);
    XFlush(display);
    
    Signal_Fence(fence);
      
    for(;;){
      if(XPending(display) > 0){
        XEvent e;
        
        XNextEvent(display, &e);
        
        HandleEvent(e);
        if(e.type == ClientMessage) break;
      }
    }
    return 0;
  }
  void Destroy_Window(PlatWindow* window){
    XDestroyWindow(window->display, window->handle);
    XCloseDisplay(window->display);
  }
  void SwapWindowBuffers(PlatWindow* window){
    glXSwapBuffers(window->display, window->handle);
  }
  void SetVSync(bool isOn){
    //glXQueryExtensionsString();
    //glXSwapIntervalEXT(isOn * 1);
  }
  bool Create_GLContext(PlatWindow* window){
    Window windowHandle = window->handle;
    Display* display = window->display;
    Screen* screen = DefaultScreenOfDisplay(display);
    int screenId = DefaultScreen(display);
    
    GLint glxAttribs[] = {
      GLX_X_RENDERABLE    , True,
      GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
      GLX_RENDER_TYPE     , GLX_RGBA_BIT,
      GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
      GLX_RED_SIZE        , 8,
      GLX_GREEN_SIZE      , 8,
      GLX_BLUE_SIZE       , 8,
      GLX_ALPHA_SIZE      , 8,
      GLX_DEPTH_SIZE      , 24,
      GLX_STENCIL_SIZE    , 8,
      GLX_DOUBLEBUFFER    , True,
      None
    };

    //////////////////////////////////////////////////////////////////////
    //FrameBuffer creation
    int confCount;
    int selectedConf = 0;
    GLXFBConfig* configs = glXChooseFBConfig(display,
                                             DefaultScreen(display),
                                             glxAttribs,
                                             &confCount);
    
    //@TODO, select the best config, somehow. For now we use the first one    
    XVisualInfo* visual = glXGetVisualFromFBConfig(display, configs[selectedConf]);
    
    
    
    GLXContext temp_glContext = glXCreateNewContext(display, configs[selectedConf], GLX_RGBA_TYPE, 0, true);

    /*
    if(!context){
      printf("Failed to create newer context. Using legacy\n");
      context = temp_glContext;
    }
    if(!glXIsDirect(display, context)){
      printf("Indirect context?\n");
      }*/
    //////////////////////////////////////////////////////////////////////    
    if(!visual){
      return false;
    }
    printf("Created context\n");
    glXMakeCurrent(display, windowHandle, temp_glContext);

    if(glewInit() != GLEW_OK){
      printf("Failed To init glew\n");
      return false;
    }
    /////////////////////////////////////////////////////////////////////
    //Create 3.2 context
    
    glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
    glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((const GLubyte*)"glXCreateContextAttribsARB");
    if(glXCreateContextAttribsARB){
      int contextAttributes[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
        GLX_CONTEXT_MINOR_VERSION_ARB, 2,
        
        None
      };
      GLXContext context = 0;
      context = glXCreateContextAttribsARB(
                                      display,
                                      configs[selectedConf],
                                      0,
                                      True,
                                      contextAttributes
                                      );
      if(context){
        printf("Got 3.2 context\n");
        glXMakeCurrent(display, windowHandle, context);
        glXDestroyContext(display, temp_glContext);
      }else{
        printf("Using legacy context\n");
        
      }
    }


    XFree(configs);
    return true;
  }
};
