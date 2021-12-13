#ifndef RENDERER_BACKEND_OPENGL_H
#define RENDERER_BACKEND_OPENGL_H

namespace RENDERER{
  namespace BACKEND{
    struct GL_Context{
      u32 vao;
    };

    bool Init_OpenGL_Context();

    // extern GL_Context glContext;
  }
}
#endif
