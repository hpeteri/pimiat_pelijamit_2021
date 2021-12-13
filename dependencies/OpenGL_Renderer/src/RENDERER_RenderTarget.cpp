#include "RENDERER_RenderTarget.h"
#include "RENDERER_glError.h"
#include <GL/glew.h>
#include <GL/gl.h>

namespace RENDERER{
  static GLbitfield GetGLMaskBits(RENDER_TARGET_BIT_MASK mask){
    GLbitfield bits = 0;
    if(mask | COLOR_BUFFER_BIT) bits |= GL_COLOR_BUFFER_BIT;
    if(mask | DEPTH_BUFFER_BIT) bits |= GL_DEPTH_BUFFER_BIT;
    return bits;
  }
  static GLenum GetGLFilter(FILTER filter){
    if(filter == NEAREST) return GL_NEAREST;
    if(filter == LINEAR) return GL_LINEAR;
    return GL_INVALID_ENUM;
  }
  bool InitRenderTarget(RenderTarget* renderTarget, u32 w, u32 h){
    renderTarget->width = w;
    renderTarget->height = h;
    
    glGenTextures(1, &renderTarget->colorAttachment);
    glBindTexture(GL_TEXTURE_2D, renderTarget->colorAttachment);

    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, w, h, 0,GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
      
    glGenFramebuffers(1, &renderTarget->framebufferHandle);
    glBindFramebuffer(GL_FRAMEBUFFER, renderTarget->framebufferHandle);
    
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderTarget->colorAttachment, 0);
    glBindFramebuffer(GL_FRAMEBUFFER,0);

    CHECK_GL_ERROR("Create RenderTarget");

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    
  
    return status == GL_FRAMEBUFFER_COMPLETE;    
  }
  void ResizeRenderTarget(RenderTarget* renderTarget, u32 w, u32 h){
    renderTarget->width = w;
    renderTarget->height = h;
    if(!renderTarget->framebufferHandle) return;
    
    glBindFramebuffer(GL_FRAMEBUFFER, renderTarget->framebufferHandle);
    glBindTexture(GL_TEXTURE_2D, renderTarget->colorAttachment);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindRenderbuffer(GL_RENDERBUFFER,0);
        
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, renderTarget->framebufferHandle);

    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);

    CHECK_GL_ERROR("Resize RenderTarget");
  }  
  void SetRenderTarget(RenderTarget renderTarget){
    CHECK_GL_ERROR("Start of SetRenderTarget");
    glBindFramebuffer(GL_FRAMEBUFFER, renderTarget.framebufferHandle);
    CHECK_GL_ERROR("Set RenderTarget glBindFrameBuffer");
    glViewport(0, 0, renderTarget.width, renderTarget.height);
    CHECK_GL_ERROR("SetRenderTarget glViewPort");
  }
  void SetClearColor(f32 r, f32 g, f32 b, f32 a){
    glClearColor(r, g, b, a);
  }
  void ClearRenderTarget(){
    glClear(GL_COLOR_BUFFER_BIT);
  }
  void BlitRenderTarget(RenderTarget src, RenderTarget dst,  RENDER_TARGET_BIT_MASK  mask){
    glBindFramebuffer(GL_READ_FRAMEBUFFER, src.framebufferHandle);
    CHECK_GL_ERROR("Bind read buffer");
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst.framebufferHandle);
    CHECK_GL_ERROR("bind write buffer");
    glBlitFramebuffer(0, 0, src.width, src.height,
                      0, 0, dst.width, dst.height,
                      GetGLMaskBits(mask), GL_NEAREST);
    CHECK_GL_ERROR("Blit RenderTarget");
  }
  void BlitRenderTarget(RenderTarget src,
                        f32 src_x0, f32 src_y0,
                        f32 src_x1, f32 src_y1,
                        RenderTarget dst,
                        f32 dst_x0, f32 dst_y0,
                        f32 dst_x1, f32 dst_y1,
                        RENDER_TARGET_BIT_MASK  mask){
    glBindFramebuffer(GL_READ_FRAMEBUFFER, src.framebufferHandle);
    CHECK_GL_ERROR("Bind read buffer");
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst.framebufferHandle);
    CHECK_GL_ERROR("bind write buffer");
    glBlitFramebuffer((i32)src_x0, (i32)src_y0, (i32)src_x1, (i32)src_y1,
                      (i32)dst_x0, (i32)dst_y0, (i32)dst_x1, (i32)dst_y1,
                      GetGLMaskBits(mask), GL_NEAREST);
    CHECK_GL_ERROR("Blit RenderTarget");
  }
}
