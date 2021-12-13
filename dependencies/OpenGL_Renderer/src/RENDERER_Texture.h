#ifndef TEXTURE_H
#define TEXTURE_H

#include "RENDERER_Types.h"

namespace RENDERER{
  enum TEXTURE_FORMAT{
    RGBA_8,
    R_8,
  };
  enum TEXTURE_FILTER{
    TEXTURE_FILTER_NEAREST,
    TEXTURE_FILTER_LINEAR,
  };
  enum TEXTURE_SWIZZLE{
    TEXTURE_SWIZZLE_RED,
    TEXTURE_SWIZZLE_GREEN,
    TEXTURE_SWIZZLE_BLUE,
    TEXTURE_SWIZZLE_ALPHA,
  };
  enum TEXTURE_COMPONENT{
    TEXTURE_COMPONENT_ZERO,
    TEXTURE_COMPONENT_ONE,
    TEXTURE_COMPONENT_RED,
    TEXTURE_COMPONENT_GREEN,
    TEXTURE_COMPONENT_BLUE,
    TEXTURE_COMPONENT_ALPHA,
  };
  struct TextureArray{
    u32 handle;
    u32 type;
    u32 format;
    u32 width, height, levels;
  };
  bool InitTextureArray(TextureArray* texture, u32 width, u32 height, u32 levels, u32 mipLevels, TEXTURE_FORMAT format);

  void UpdateTextureData(TextureArray texture, u32 width, u32 height, u32 level, void* data);
  void SetTextureFilter(TextureArray texture, TEXTURE_FILTER filter);
  void SetTextureSwizzle(TextureArray texture, TEXTURE_SWIZZLE swizzle, TEXTURE_COMPONENT component);
  
  void BindTexture(TextureArray texture);
  void UnbindTexture(TextureArray texture);
  
  
}
#endif
