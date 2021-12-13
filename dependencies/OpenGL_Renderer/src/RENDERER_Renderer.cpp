#include "RENDERER_Renderer.h"

#include "RENDERER_Renderer_Backend.opengl.h"

#include <cstdio>
namespace RENDERER{
  namespace BACKEND{
    bool Init(Renderer* renderer, Allocator allocator){
      if(!BACKEND::Init_OpenGL_Context())
        return false;
            
      Init(&renderer->batchRenderer, allocator);
      Init(&renderer->imRenderer, allocator);
      
      return true;
    }
  }
}

