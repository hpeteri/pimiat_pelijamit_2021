#include "RENDERER_Renderer_Backend.opengl.h"

#include <GL/glew.h>
#include <GL/gl.h>

#include "RENDERER_glError.h"
namespace RENDERER{
  namespace BACKEND{
    GL_Context glContext;

    bool Init_OpenGL_Context(){
      printf("Init OpenGL Backend\n");
      if(glewInit() != GLEW_OK){
        printf("Failed to init glew\n");
        return false;
      }      
      CHECK_GL_ERROR("Init GL Context");
      
      glGenVertexArrays(1, &glContext.vao);
      CHECK_GL_ERROR("GenVertexArrays");
            
      glBindVertexArray(glContext.vao);
      CHECK_GL_ERROR("BindVertexArray");
      return true;
    }

    
    u32 GetTopology(TOPOLOGY topology){
      switch(topology){
      case(TOPOLOGY_TRIANGLES):
        return GL_TRIANGLES;
      default:
        *(int*)nullptr = 0;
        return 0;
      }
    }
  }
}
