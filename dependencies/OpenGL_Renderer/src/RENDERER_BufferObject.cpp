#include "RENDERER_BufferObject.h"
#include "RENDERER_glError.h"
#include "RENDERER_ASSERT.h"
#include <GL/glew.h>
#include <GL/gl.h>

namespace RENDERER{
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  static BufferObject CreateBuffer(GLenum type, u32 size, void* data, GLenum usage){
    BufferObject buffer;
    buffer.type = type;
    buffer.size = size;
    buffer.usage = usage;
    
    glGenBuffers(1, &buffer.handle);

    CHECK_GL_ERROR("Create Buffer");
    if(!buffer.handle){
      printf("Failed to create buffer\n");
      return BufferObject{0};
    }
    if(size){
      glBindBuffer(type, buffer.handle);
      glBufferData(type, size, data, usage);
      glBindBuffer(type, 0);
    }
    return buffer;
  }
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  void BindBuffer(BufferObject buffer){
    glBindBuffer(buffer.type, buffer.handle);
    CHECK_GL_ERROR("Bind Buffer");
  }
  void UnbindBuffer(BufferObject buffer){
    glBindBuffer(buffer.type, 0);
    CHECK_GL_ERROR("Unbind Buffer");
  }
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  void FillBuffer(BufferObject buffer, u32 offset, u32 size, void* data){
    ASSERT(buffer.size >= offset + size);
    glBufferSubData(buffer.type, offset, size, data);
    CHECK_GL_ERROR("Fill Buffer");
  }
  void FillBuffer_Expand(BufferObject* buffer, u32 size, void* data){
    if(buffer->size >= size){
      FillBuffer(*buffer, 0, size, data);
    }else{
      buffer->size = size;
      glBufferData(buffer->type, size, data, buffer->usage);
      CHECK_GL_ERROR("Resize Buffer");
    }
  }
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  BufferObject CreateBuffer_Index(u32 size, void* data){
    return CreateBuffer(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_READ);
  }
  BufferObject CreateBuffer_Vertex(u32 size, void* data){
    return CreateBuffer(GL_ARRAY_BUFFER, size, data, GL_STATIC_READ);
  }
  BufferObject CreateBuffer_Uniform(u32 size, void* data){
    return CreateBuffer(GL_UNIFORM_BUFFER, size, data, GL_DYNAMIC_READ);
  }
  BufferObject CreateBuffer_Instance(u32 size, void* data){
    return CreateBuffer(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_READ);
  }
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  void DestroyBuffer(BufferObject buffer){
    glDeleteBuffers(1, &buffer.handle);
  }
}
