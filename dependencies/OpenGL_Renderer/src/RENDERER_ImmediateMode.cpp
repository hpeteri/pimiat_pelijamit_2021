#include "RENDERER_ImmediateMode.h"
#include "Math_Matrix.h"
#include "Math_Vector.h"

#include <GL/glew.h>
#include <GL/gl.h>

namespace RENDERER{                                            
  namespace IM{
    const MATH::Vector2<f32> circlePrimitive[] = {
      MATH::Vector2<f32>{1, 0},
      MATH::Vector2<f32>{MATH::Cos(MATH::PI / 4), MATH::Sin(MATH::PI / 4)},
      MATH::Vector2<f32>{0, 1},
      MATH::Vector2<f32>{-MATH::Cos(MATH::PI / 4), MATH::Sin(MATH::PI / 4)},
      MATH::Vector2<f32>{-1, 0},
      MATH::Vector2<f32>{-MATH::Cos(MATH::PI / 4), -MATH::Sin(MATH::PI / 4)},
      MATH::Vector2<f32>{0, -1},
      MATH::Vector2<f32>{MATH::Cos(MATH::PI / 4), -MATH::Sin(MATH::PI / 4)}
    };
    
    void Init(IM_Renderer* renderer, Allocator allocator){
      memset(renderer, 0, sizeof(*renderer));
      renderer->allocator = allocator;
      renderer->batches.Init(allocator.Allocate, allocator.Free);
      renderer->vertexBuffer = CreateBuffer_Vertex(0);
      renderer->indexBuffer = CreateBuffer_Index(0);
      renderer->batchIndex = -1;
    }
    void Immediate_Begin(IM_Renderer* renderer, TOPOLOGY topology, RendererState state){
      if(renderer->batchIndex != -1){
        printf("Immediate end not called... Calling now\n");
        Immediate_End(renderer);
      }
      u32 topo = BACKEND::GetTopology(topology);

      if(!(state.stateBits & BLEND_BIT)){
        //If not blending, then take any matching batch
        for(u32 i = 0; i < renderer->batches.count; i++){
          auto it = renderer->batches[i];
   
          if(it->topology != topo)
            continue;
        
          if(!DoesStateMatch(it->state, state))
            continue;
          
          renderer->batchIndex = i;
          return;
        }
      }else{
        //Only take the last batch
        auto it = renderer->batches.End();
        
        if(it && it->topology == topo &&        
           DoesStateMatch(it->state, state)){
          
          renderer->batchIndex = renderer->batches.count - 1;
          return;
        }
      }
      
      //Create new batch
      renderer->batchIndex = renderer->batches.count;

      IM_Batch batch = {};
      batch.topology = topo;
      batch.state = state;
      
      batch.vertices.Init(renderer->allocator.Allocate,
                          renderer->allocator.Free);

      batch.indices.Init(renderer->allocator.Allocate,
                         renderer->allocator.Free);      

      renderer->batches.Add(batch);
      
            
      renderer->vertexState.color = 0xffffffff;
      renderer->vertexState.tc_x = 0;
      renderer->vertexState.tc_y = 0;
      renderer->vertexState.tc_z = 0;
      
      //renderer->startingIndex = 0;
    }
    void Immediate_End(IM_Renderer* renderer){
      renderer->batchIndex = -1;
    }
    u32 GetCurrentVertexCount(IM_Renderer* renderer){
      return renderer->batches[renderer->batchIndex]->vertices.count;
    }
    void Immediate_Vertex(IM_Renderer* renderer, f32 x, f32 y, f32 z){
      auto it = renderer->batches[renderer->batchIndex];

      renderer->vertexState.x = x;
      renderer->vertexState.y = y;
      renderer->vertexState.z = z;
            
      it->vertices.Add(renderer->vertexState);
    }
    void Immediate_Index(IM_Renderer* renderer, u32 i){
      auto it = renderer->batches[renderer->batchIndex];
      
      it->indices.Add(i);//renderer->startingIndex + i);
    }
    void Immediate_Color(IM_Renderer* renderer, u32 color){
      renderer->vertexState.color = color;
    }
    void Immediate_TextureCoord(IM_Renderer* renderer, f32 tc_x, f32 tc_y, f32 tc_z){
      renderer->vertexState.tc_x = tc_x;
      renderer->vertexState.tc_y = tc_y;
      renderer->vertexState.tc_z = tc_z;
    }
    void Immediate_Text(IM_Renderer* renderer, f32 x, f32 y, f32 scale, u32 color, const char* text, const Font* font){
      if(!text) return;

      f32 p0_x = x;
      f32 p0_y = y;
      
      const u8* at = (u8*)text;
      u32 index = GetCurrentVertexCount(renderer);

      Immediate_Color(renderer, color);
      for(; *at; at++){
        if(*at == '\n'){
          p0_y -= (font->lineGap + font->ascent - font->descent)* scale;
          p0_x = x;
          continue;
        }
        Glyph c = font->glyphs[*at];
        f32 width = c.width * scale;
        f32 height = c.height * scale;
        char glyphIndex = *at;

        f32 bearing_x = c.bearing_x * scale;
        f32 bearing_y = c.bearing_y * scale;
        
        Immediate_TextureCoord(renderer, c.bl_uv_x, c.bl_uv_y, glyphIndex);
        Immediate_Vertex(renderer, p0_x + bearing_x, p0_y + bearing_y, 1);
          
        Immediate_TextureCoord(renderer, c.bl_uv_x, c.tr_uv_y, glyphIndex);
        Immediate_Vertex(renderer, p0_x + bearing_x, p0_y - height + bearing_y, 1);
        
        Immediate_TextureCoord(renderer, c.tr_uv_x, c.tr_uv_y, glyphIndex);
        Immediate_Vertex(renderer, p0_x + bearing_x + width, p0_y - height + bearing_y, 1);

        
        Immediate_TextureCoord(renderer, c.tr_uv_x, c.bl_uv_y, glyphIndex);
        Immediate_Vertex(renderer, p0_x + bearing_x + width, p0_y + bearing_y , 1);

        Immediate_Index(renderer, index + 0);
        Immediate_Index(renderer, index + 1);
        Immediate_Index(renderer, index + 2);

        Immediate_Index(renderer, index + 0);
        Immediate_Index(renderer, index + 2);
        Immediate_Index(renderer, index + 3);
        
        index += 4;

        p0_x += c.advance * scale;
      }      
    }
    void Immediate_Line(IM_Renderer* renderer, f32 x0, f32 y0, f32 x1, f32 y1, f32 lineWidth, u32 color){
      Immediate_Color(renderer, color);
      MATH::Vector2<f32> dir = {x1 - x0, y1 - y0};
      auto it = renderer->batches[renderer->batchIndex];
      auto index =  GetCurrentVertexCount(renderer);
              
      auto normal = MATH::Normalize(MATH::Normal(dir));

      Immediate_Vertex(renderer,
                       x0 - normal.x * lineWidth * 0.5f, 
                       y0 - normal.y * lineWidth * 0.5f,
                       1);

      Immediate_Vertex(renderer,
                       x1 - normal.x * lineWidth * 0.5f, 
                       y1 - normal.y * lineWidth * 0.5f,
                       1);

      Immediate_Vertex(renderer,
                       x1 + normal.x * lineWidth * 0.5f, 
                       y1 + normal.y * lineWidth * 0.5f,
                       1);

      Immediate_Vertex(renderer,
                       x0 + normal.x * lineWidth * 0.5f, 
                       y0 + normal.y * lineWidth * 0.5f,
                       1);

      Immediate_Index(renderer, index + 0);
      Immediate_Index(renderer, index + 1);
      Immediate_Index(renderer, index + 2);
                                
      Immediate_Index(renderer, index + 0);
      Immediate_Index(renderer, index + 2);
      Immediate_Index(renderer, index + 3);
    }
    void Immediate_Circle_Outline(IM_Renderer* renderer, f32 x0, f32 y0, f32 r, f32 lineWidth, u32 color){
      u32 pointCount = sizeof(circlePrimitive) / sizeof(*circlePrimitive);
      for(u32 i = 0; i < pointCount; i++){
        Immediate_Line(renderer,
                       x0 + circlePrimitive[i].x * r,
                       y0 + circlePrimitive[i].y * r,
                       x0 + circlePrimitive[(i + 1) % pointCount].x * r,
                       y0 + circlePrimitive[(i + 1) % pointCount].y * r,
                       lineWidth, color);
      }
    }
    void Immediate_Outline(IM_Renderer* renderer, u32 pointCount, MATH::Vector2<f32>* points, f32 lineWidth, u32 color){
      for(u32 i = 0; i < pointCount; i++){
        Immediate_Line(renderer,
                       points[i].x,
                       points[i].y,
                       points[(i + 1) % pointCount].x,
                       points[(i + 1) % pointCount].y,
                       lineWidth, color);
      }
    }
    void Immediate_Flush(IM_Renderer* renderer){
      SetMaterialDescriptors_IM();
      
      BindBuffer(renderer->vertexBuffer);
      SetMaterialVertexDescriptors_IM();

      for(u32 i = 0; i < renderer->batches.count; i++){

        auto it = renderer->batches[i];
        SetRendererState(it->state);
        
        FillBuffer_Expand(&renderer->vertexBuffer,
                          it->vertices.count * sizeof(IM_Vertex),
                          it->vertices.data);

        renderer->drawCallCounter++;
        if(it->indices.count){
          BindBuffer(renderer->indexBuffer);
          FillBuffer_Expand(&renderer->indexBuffer,
                            it->indices.count * sizeof(Index),
                            it->indices.data);
          
          glDrawElements(it->topology, it->indices.count, GL_UNSIGNED_INT, nullptr);
        }else{
          glDrawArrays(it->topology, 0, it->vertices.count);
        }
                
        //////////////////////////////////////////////////////////////////////
        //Reset
        it->vertices.Destroy();
        it->indices.Destroy();
      }
      renderer->batches.count = 0;
      ResetMaterialDescriptors_IM(); 
    }
  }
}
