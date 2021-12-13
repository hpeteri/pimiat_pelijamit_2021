#ifndef SHADER_H
#define SHADER_H

#include "RENDERER_Types.h"
namespace RENDERER{
  struct Shader{
    u32 handle;
    
    i32 projection_loc;
    
    i32 diffuseTexture_loc;
    i32 lightTexture_loc;
  };
  enum SHADER_TYPE{
    VERTEX_SHADER = 0,
    FRAGMENT_SHADER = 1
  };
  struct ShaderSource{
    SHADER_TYPE type;
    char* source;
  };
    
  struct ShaderSources{
    ShaderSource sources[2];        
  };
  
  
  bool InitShader(Shader* shader, const ShaderSources sources);
  void DeleteShader(const Shader shader);
  void BindShader(const Shader shader);
  void UnbindShader();
}
#endif
