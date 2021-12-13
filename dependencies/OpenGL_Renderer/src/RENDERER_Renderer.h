#ifndef RENDERER_H
#define RENDERER_H

#include "RENDERER_Types.h"

#include "RENDERER_Allocator.h"

#include "RENDERER_RendererState.h"
#include "RENDERER_BatchRenderer.h"
#include "RENDERER_ImmediateMode.h"

#include "RENDERER_Shader.h"
namespace RENDERER{
  struct Renderer{
    RendererState state;
    
    BATCH::BatchRenderer batchRenderer;
    IM::IM_Renderer imRenderer;    
  };
  namespace BACKEND{
  bool Init(Renderer* renderer, Allocator allocator);  
  }
}

#endif
