#ifndef RENDERERSTATE_H
#define RENDERERSTATE_H

#include "RENDERER_Types.h"
#include "RENDERER_RenderTarget.h"
#include "RENDERER_Texture.h"
#include "RENDERER_Material.h"

#include "Math_Matrix.h"

namespace RENDERER{
  
  enum TOPOLOGY{
    TOPOLOGY_TRIANGLES = 1,
  };
  
  namespace BACKEND{
    u32 GetTopology(RENDERER::TOPOLOGY topology);
  };
  enum STATE_BITS{
    BLEND_BIT = 1,
    DEPTH_BIT = 2,
  };
  const u32 DEFAULT_STATE = 0;
  
  struct RendererState{
    MATH::Matrix4x4 projection;
    RenderTarget    renderTarget;
    i32     stateBits;

    Material material;
    //TextureArray texture;
  };
  bool DoesStateMatch(RendererState state0, RendererState state1);
  void SetRendererState(RendererState state);
}

#endif
