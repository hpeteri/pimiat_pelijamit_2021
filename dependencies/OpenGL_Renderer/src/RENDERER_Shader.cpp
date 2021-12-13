#include "RENDERER_Shader.h"

#include <stdio.h>
#include "RENDERER_glError.h"
namespace RENDERER{
  static int GetShaderType(SHADER_TYPE type){
    if(type == VERTEX_SHADER) return GL_VERTEX_SHADER;
    if(type == FRAGMENT_SHADER) return GL_FRAGMENT_SHADER;
    return 0;
  }
  static bool CheckModuleCompileStatus(const GLuint module){
    i32 success = 0;
    glGetShaderiv(module, GL_COMPILE_STATUS, &success);
    if(success) return true;

    
    char buffer[1024];
    glGetShaderInfoLog(module, 1024, NULL, buffer);
    printf(buffer);
    return false;    
  }
  
  static bool CheckShaderLinkStatus(const GLuint shader){
    i32 success = 0;
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if(success) return true;

    
    char buffer[1024];
    glGetProgramInfoLog(shader, 1024, NULL, buffer);
    printf(buffer);
    return false;    
  }

  bool InitShader(Shader* shader, const ShaderSources sources){
    shader->handle = glCreateProgram();
    CHECK_GL_ERROR("Create Shader Program");
    for(u32 i = 0; i < sizeof(sources.sources) / sizeof(sources.sources[0]); i++){
      ShaderSource source = sources.sources[i];
      if(!source.source) continue;
      GLuint module = glCreateShader(GetShaderType(source.type));
      CHECK_GL_ERROR("Create Shader");
      glShaderSource(module, 1, &source.source, NULL);
      CHECK_GL_ERROR("Shader source");
      glCompileShader(module);
      CHECK_GL_ERROR("Compile Shader");
      if(!CheckModuleCompileStatus(module)){
        printf("Error in source :: \n%s\n", source.source);
        //@TODO Cleanup
        glDeleteShader(module);
        glDeleteProgram(shader->handle);
        CHECK_GL_ERROR("Failed to create shader");
        return false;
      }
      CHECK_GL_ERROR("Check compilation status");
      glAttachShader(shader->handle, module);
      CHECK_GL_ERROR("Attach shader");
    }
    
    glLinkProgram(shader->handle);
    
    if(!CheckShaderLinkStatus(shader->handle))
      ASSERT(false);

    CHECK_GL_ERROR("Init Shader");
    
    BindShader(*shader);
    shader->projection_loc = glGetUniformLocation(shader->handle, "projection");

    shader->diffuseTexture_loc = glGetUniformLocation(shader->handle, "diffuse_texture");
    shader->lightTexture_loc = glGetUniformLocation(shader->handle, "light_texture");

    CHECK_GL_ERROR("Init Shader");
    return true;
  }
  void DeleteShader(const Shader shader){
    glDeleteProgram(shader.handle);
  }
  
  void BindShader(const Shader shader){
    glUseProgram(shader.handle);
    CHECK_GL_ERROR("Bind Shader");
  }
  void UnbindShader(){
    glUseProgram(0);
    CHECK_GL_ERROR("Unbind Shader");
  }    
}
