#include "RENDERER_RendererState.h"

namespace RENDERER{
  bool DoesStateMatch(RendererState state0, RendererState state1){
    if(state0.projection != state1.projection) return false;
    if(state0.stateBits != state1.stateBits) return false;
    
    if(state0.material.flags != state1.material.flags) return false;
    if(state0.material.diffuseTexture.handle != state1.material.diffuseTexture.handle) return false;
    
    return true;
  }
  void SetRendererState(RendererState state){
    BindMaterial(state.material);
    glUniformMatrix4fv(state.material.shader.projection_loc,
                       1,
                       GL_FALSE,
                       state.projection.A2);

    if(state.stateBits & BLEND_BIT){
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }else{
      glDisable(GL_BLEND);
    }

    SetRenderTarget(state.renderTarget);
    CHECK_GL_ERROR("Set Renderer State\n");
  }
}
