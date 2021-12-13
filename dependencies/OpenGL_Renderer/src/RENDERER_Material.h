#ifndef RENDERER_MATERIAL_H
#define RENDERER_MATERIAL_H

#include "RENDERER_Types.h"
#include "RENDERER_Shader.h"

namespace RENDERER{
  enum MATERIAL_FLAGS{
    MATERIAL_FLAGS_DIFFUSE_TEXTURE = 0x1,
    MATERIAL_FLAGS_LIGHT_TEXTURE   = 0x2,
  };
  struct Material{
    u32 flags;
    Shader shader;
    
    TextureArray diffuseTexture;
    TextureArray lightTexture;
  };

  Material CreateMaterial(u32 flags, ShaderSources sources);
  void BindMaterial(Material material);
  
  void SetMaterialDescriptors_IM();
  void SetMaterialVertexDescriptors_IM();
  void ResetMaterialDescriptors_IM();

  void SetMaterialDescriptors_Batch();
  void SetMaterialVertexDescriptors_Batch();
  void SetMaterialInstanceDescriptors_Batch(u32 flags);
  void ResetMaterialDescriptors_Batch();
}
#endif
