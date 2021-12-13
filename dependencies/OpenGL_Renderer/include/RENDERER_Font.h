#ifndef RENDERER_FONT_H
#define RENDERER_FONT_H

#include "RENDERER_Types.h"
#include "RENDERER_Texture.h"
#include "Math_Vector.h"

namespace RENDERER{
  struct Glyph{
    i32 width, height;
    f32 advance, bearing_x, bearing_y;
    f32 bl_uv_x, bl_uv_y, tr_uv_x, tr_uv_y;    
  };
  struct Font{
    TextureArray texture;
    f32 ascent, descent;
    f32 lineGap;
    Glyph glyphs[256];
  };
  bool InitFont(Font* font, f32 fontSize, void* data, void* (*Allocate)(size_t), void (*Free)(void*));
  MATH::Vector2<f32> GetTextSize(const char* text, f32 scale, const Font* font);
}
#endif
