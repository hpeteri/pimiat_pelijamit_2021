#ifndef IMMEDIATEMODE_H
#define IMMEDIATEMODE_H

#include "RENDERER_Types.h"
#include "Array.h"
#include "RENDERER_Allocator.h"

#include "RENDERER_RendererState.h"
#include "RENDERER_BufferObject.h"
#include "RENDERER_Mesh.h"
#include "RENDERER_Font.h"

namespace RENDERER{
  namespace IM{
    struct IM_Vertex : public Vertex{
      f32 tc_z;
    };
    struct IM_Batch{
      u32 topology;
      RendererState state;
      u32 textureLayer;

      IM_Vertex vertexState;
      
      Array<IM_Vertex> vertices;
      Array<Index> indices;
    };

    struct IM_Renderer{
      Allocator allocator;
      i32 batchIndex;
      //u32 startingIndex;

      IM_Vertex vertexState;
      
      Array<IM_Batch> batches;

      BufferObject vertexBuffer;
      BufferObject indexBuffer;

      u32 drawCallCounter;
    };

    void Init(IM_Renderer* renderer, Allocator allocator);
    void Immediate_Begin(IM_Renderer* renderer, TOPOLOGY topology, RendererState state);
    void Immediate_End(IM_Renderer* renderer);

    u32 GetCurrentVertexCount(IM_Renderer* renderer);

    void Immediate_Vertex(IM_Renderer* renderer, f32 x, f32 y, f32 z);
    void Immediate_Index(IM_Renderer* renderer, u32 i);
    void Immediate_Color(IM_Renderer* renderer, u32 color);
    void Immediate_TextureCoord(IM_Renderer* renderer, f32 tc_x, f32 tc_y, f32 tc_z);

    void Immediate_Text(IM_Renderer* renderer,
                        f32 x, f32 y,
                        f32 scale, u32 color,
                        const char* text, const  Font* font);

    void Immediate_Line(IM_Renderer* renderer,
                        f32 x0, f32 y0,
                        f32 x1, f32 y1,
                        f32 lineWidth, u32 color);    

    void Immediate_Circle_Outline(IM_Renderer* renderer,
                                  f32 x0, f32 y0,
                                  f32 r, f32 lineWidth, u32 color);

    void Immediate_Outline(IM_Renderer* renderer,
                           u32 pointCount, MATH::Vector2<f32>* points,
                           f32 lineWidth, u32 color);
    
    void Immediate_Flush(IM_Renderer* renderer);
    inline void Immediate_ResetCounter(IM_Renderer* renderer){renderer->drawCallCounter = 0;}
      
  }
}

#endif
