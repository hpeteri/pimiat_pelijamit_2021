#ifndef BUFFEROBJECT_H
#define BUFFEROBJECT_H

#include "RENDERER_Types.h"
namespace RENDERER{
  struct BufferObject{
    u32 handle;
    u32 type;
    u32 size;
    u32 usage;
  };
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  void BindBuffer(BufferObject buffer);
  void UnbindBuffer(BufferObject buffer);
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  void FillBuffer(BufferObject buffer, u32 offset, u32 size, void* data);
  void FillBuffer_Expand(BufferObject* buffer, u32 size, void* data);
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  BufferObject CreateBuffer_Index(u32 size, void* data = nullptr);
  BufferObject CreateBuffer_Vertex(u32 size, void* data = nullptr);
  BufferObject CreateBuffer_Uniform(u32 size, void* data = nullptr);
  BufferObject CreateBuffer_Instance(u32 size, void* data = nullptr);

  void DestroyBuffer(BufferObject buffer);
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif
