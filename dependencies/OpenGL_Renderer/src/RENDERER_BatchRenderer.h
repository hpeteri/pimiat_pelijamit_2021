#ifndef BATCHRENDERER_H
#define BATCHRENDERER_H

#include "RENDERER_Types.h"
#include "LinkedList.h"
#include "HashTable.h"
#include "Math_Matrix.h"
#include "RENDERER_Allocator.h"

#include "RENDERER_BufferObject.h"
#include "RENDERER_Mesh.h"
#include "RENDERER_RendererState.h"

#define MAX_INSTANCES_PER_BATCH  (1024 * 1024 / sizeof(Instance))
namespace RENDERER{
  namespace BATCH{
    struct Instance{
      MATH::Matrix4x4 model;
      u32 color;
      f32 diffuseLayer;
      f32 rotationToLight;
      u32 lightLayer;
    };
  
    struct Batch{
      u32 instanceCount;
      Instance* instances;
      
      BufferObject instanceBuffer;
      RendererState state;
    };
  
    struct DrawCall{
      BufferObject vertexBuffer;
      BufferObject indexBuffer;
      u32 indexCount;
      LinkedList<Batch> batches;
    };
    
    struct BatchRenderer{
      Allocator allocator;
      
      LinkedList<Batch> batchPool;
      HashTable<DrawCall> drawCalls;

      u32 drawCallCounter;
    };
  
    void Init(BatchRenderer* renderer, Allocator allocator);
    bool AddMesh(BatchRenderer* renderer, const char* name, const Mesh mesh);

    Batch* BeginBatch(BatchRenderer* renderer, const char* name, RendererState state);
    Batch* GetBatch(BatchRenderer* renderer, DrawCall* drawCall, RendererState state);

    //@Parameter batch must not be full, 
    //Returns true if new instance can be pushed after this push
    //Returns false if batch becomes full after pushing new instance to the buffer.
    bool PushInstance(Batch* batch, Instance instance);
    
    
    void Flush(BatchRenderer* renderer);
    inline void ResetCounter(BatchRenderer* renderer){renderer->drawCallCounter = 0;}
  }
}
#endif
