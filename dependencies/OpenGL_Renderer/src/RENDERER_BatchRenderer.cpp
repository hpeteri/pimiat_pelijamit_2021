#include "RENDERER_BatchRenderer.h"
#include "RENDERER_Shader.h"
#include "RENDERER_glError.h"
#include "RENDERER_ASSERT.h"
#include <GL/glew.h>
#include <GL/gl.h>

namespace RENDERER{
  namespace BATCH{
    unsigned long long StringHash(const char* string){
      unsigned long long hash = 0;
      const char* at = string;
      while(*at){
        hash += *at++;
        hash += (hash << 10);
        hash ^= (hash >> 6);
      }
      hash += (hash << 3);
      hash ^= (hash >> 11);
      hash += (hash << 15);
      hash &= ~((u64)1 << 63);
      return hash;      
    }
    void Init(BatchRenderer* renderer, Allocator allocator){
      renderer->allocator = allocator;
      renderer->drawCalls.Init(allocator.Allocate, allocator.Free, StringHash);
      renderer->batchPool.Init(allocator.Allocate, allocator.Free);
    }
    DrawCall* FindDrawCallByName(BatchRenderer* renderer, const char* name){
      return renderer->drawCalls.Find(name);
    }
    bool AddMesh(BatchRenderer* renderer, const char* name, const Mesh mesh){
      if(FindDrawCallByName(renderer, name)) //Already registered
        return false;

      DrawCall drawCall;

      drawCall.vertexBuffer = CreateBuffer_Vertex(mesh.vertexCount * sizeof(*mesh.vertices),
                                                  mesh.vertices);
      drawCall.indexBuffer = CreateBuffer_Index(mesh.indexCount * sizeof(*mesh.indices),
                                                mesh.indices);
      drawCall.indexCount = mesh.indexCount;
      drawCall.batches.Init(renderer->allocator.Allocate, renderer->allocator.Free);
      renderer->drawCalls.Add(name, drawCall);

      
      return true;
    }
    Batch* GetBatch(BatchRenderer* renderer, DrawCall* drawCall, RendererState state){

      
      for(ListNode<Batch>* node = drawCall->batches.end; node; node = node->prev){
        auto it = &node->data;          
        if(!DoesStateMatch(it->state, state)){
          //if blending, then we are forces to make a new batch if the last pushed batch doesn't
          //match our current batch
          if(state.stateBits & BLEND_BIT)
            break;
          
          continue;
        }      

        if(!(it->instanceCount < MAX_INSTANCES_PER_BATCH)){
          continue;
        }
        
        
        return it;      
      }
      if(!renderer->batchPool.end){
        Batch batch = {0};
        batch.instances = (Instance*)renderer->allocator.Allocate(sizeof(Instance) * MAX_INSTANCES_PER_BATCH);
        batch.instanceBuffer = CreateBuffer_Instance(sizeof(Instance) * MAX_INSTANCES_PER_BATCH, nullptr);
        renderer->batchPool.Add(batch);
      }
    
      auto node = renderer->batchPool.end;
      renderer->batchPool.RemoveLink(node);
      drawCall->batches.AddLink(node);    

    
      Batch* it = &node->data;
      it->instanceCount = 0;
      it->state = state;
    
      return &drawCall->batches.end->data;
    }
    Batch* BeginBatch(BatchRenderer* renderer, const char* name, RendererState state){
      DrawCall* drawCall = FindDrawCallByName(renderer, name);
      if(!drawCall){
        return nullptr;
      }
      Batch* batch = GetBatch(renderer, drawCall, state);
      
      return batch;      
    }
    bool PushInstance(Batch* batch, Instance instance){
      ASSERT(batch->instanceCount < MAX_INSTANCES_PER_BATCH);
      batch->instances[batch->instanceCount++] = instance;
      return batch->instanceCount < MAX_INSTANCES_PER_BATCH;
    }
    
    void Flush(BatchRenderer* renderer){
      SetMaterialDescriptors_Batch();

      for(unsigned int i = 0; i < renderer->drawCalls.count; i++){
        DrawCall* drawCall = renderer->drawCalls[i];
        
        BindBuffer(drawCall->vertexBuffer);
        SetMaterialVertexDescriptors_Batch();
      
        BindBuffer(drawCall->indexBuffer);
        
        for(ListNode<Batch>* batchNode = drawCall->batches.start; batchNode; batchNode = batchNode = batchNode->next){
          
          Batch* it = &batchNode->data;
          SetRendererState(it->state);
          
          BindBuffer(it->instanceBuffer);
          SetMaterialInstanceDescriptors_Batch(it->state.material.flags);
          FillBuffer(it->instanceBuffer, 0, it->instanceCount * sizeof(Instance), it->instances);
          
          renderer->drawCallCounter++;
          
          glDrawElementsInstanced(GL_TRIANGLES, drawCall->indexCount, GL_UNSIGNED_INT, nullptr, it->instanceCount);
          CHECK_GL_ERROR("Draw instanced\n");
          
          //Add batch back to pool        
          drawCall->batches.RemoveLink(batchNode);
          renderer->batchPool.AddLink(batchNode);
        }
      }
      ResetMaterialDescriptors_Batch();      
    }  
  }
}
