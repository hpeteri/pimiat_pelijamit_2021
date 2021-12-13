#ifndef MESH_H
#define MESH_H

#include "RENDERER_Types.h"
namespace RENDERER{
  struct Vertex{
    f32 x,y,z;
    f32 tc_x, tc_y;
    u32 color;    
  };
  typedef u32 Index;
  
  struct Mesh{
    u32 vertexCount;
    Vertex* vertices;

    u32 indexCount;
    Index* indices;
  };

  
};

#endif
