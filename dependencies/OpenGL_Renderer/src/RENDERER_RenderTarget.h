#ifndef RENDERTARGET_H
#define RENDERTARGET_H

#include "RENDERER_Types.h"
namespace RENDERER{
  struct RenderTarget{
    u32 width;
    u32 height;
    u32 framebufferHandle;
    u32 colorAttachment;
  };
  enum RENDER_TARGET_BIT_MASK{
    COLOR_BUFFER_BIT = 1,
    DEPTH_BUFFER_BIT = 2,
  };
  enum FILTER{
    NEAREST = 1,
    LINEAR = 2,
  };
  bool InitRenderTarget(RenderTarget* renderTarget, u32 w, u32 h);
    
  void ResizeRenderTarget(RenderTarget* renderTarget, u32 w, u32 h);

  void SetClearColor(f32 r, f32 g, f32 b, f32 a);
  void SetRenderTarget(RenderTarget renderTarget);
  void ClearRenderTarget();
    
  void BlitRenderTarget(RenderTarget src, RenderTarget dst, RENDER_TARGET_BIT_MASK mask);
  void BlitRenderTarget(RenderTarget src,
                        f32 src_x0, f32 src_y0,
                        f32 src_x1, f32 src_y1,
                        RenderTarget dst,
                        f32 dst_x0, f32 dst_y0,
                        f32 dst_x1, f32 dst_y1,
                        RENDER_TARGET_BIT_MASK  mask);
}

#endif
