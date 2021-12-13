#include "RENDERER_Font.h"
#include "RENDERER_glError.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include "Math_Basic.h"


namespace RENDERER{
  bool InitFont(Font* font, f32 fontSize, void* data, void* (*Allocate)(size_t), void (*Free)(void*)){
    if(!data){
      printf("Data is null\n");
      return false;
    }
    bool inited = false;
    stbtt_fontinfo info; 

    if(!stbtt_InitFont(&info, (const unsigned char*)data, 0)){
      printf("stbtt_InitFont failed\n");
      return false;
    }
    const u32 glyphCount = info.numGlyphs;
    const f32 scale = stbtt_ScaleForPixelHeight(&info, fontSize);
    
    i32 advance, lsb, x0, y0, x1, y1;
        
    stbtt_GetFontBoundingBox(&info, &x0, &y0, &x1, &y1);
    const u32 font_width =  MATH::Next2Pow(MATH::Round((x1 - x0)* scale));
    const u32 font_height =  MATH::Next2Pow(MATH::Round((y1 - y0) * scale));
    
    RENDERER::InitTextureArray(&font->texture, font_width, font_height, 256, 1, RENDERER::TEXTURE_FORMAT::R_8);
            
    u8* buffer = (u8*)Allocate(font_width * font_height);

    const u32 pad = 5;
    for(i32 i = 0; i < 256; i++){
      memset(buffer, 0, font_width * font_height);
      stbtt_GetCodepointHMetrics(&info, i, &advance, &lsb);
      stbtt_GetCodepointBitmapBox(&info, i, scale, scale, &x0, &y0, &x1, &y1); 
      i32 x =  x1 - x0;
      i32 y  = y1 - y0;
      
      stbtt_MakeCodepointBitmap(&info,
                                &buffer[pad * font_width + pad],
                                font_width - pad,
                                font_height - pad,
                                font_width,
                                scale,
                                scale,
                                i
                                );
      
      RENDERER::UpdateTextureData(font->texture, font_width, font_height, i, buffer);

      Glyph* g = &font->glyphs[i];
      g->width = x;
      g->height = y;
      g->advance = advance * scale;
      g->bearing_x = (f32)lsb * scale;
      g->bearing_y = -(f32)y0;
      g->bl_uv_x = pad / (f32)font_width;
      g->bl_uv_y = pad / (f32)font_width;
      g->tr_uv_x = (x + pad) / (f32)font_width;
      g->tr_uv_y = (y + pad)/ (f32)font_height;
    }
    i32 descent, ascent, lineGap;
    stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);
    font->ascent = ascent * scale;
    font->descent = descent * scale;
    font->lineGap = lineGap * scale;
    Free(buffer);
    
    SetTextureFilter(font->texture, TEXTURE_FILTER_NEAREST);
    SetTextureSwizzle(font->texture, TEXTURE_SWIZZLE_ALPHA, TEXTURE_COMPONENT_RED);
    SetTextureSwizzle(font->texture, TEXTURE_SWIZZLE_RED,   TEXTURE_COMPONENT_ONE);
    SetTextureSwizzle(font->texture, TEXTURE_SWIZZLE_GREEN, TEXTURE_COMPONENT_ONE);
    SetTextureSwizzle(font->texture, TEXTURE_SWIZZLE_BLUE,  TEXTURE_COMPONENT_ONE);
    return true;
  }
  MATH::Vector2<f32> GetTextSize(const char* text, f32 scale, const Font* font){
    MATH::Vector2<f32> size = {0};

    if(!text) return size;
    
    f32 p0_x = 0;
    f32 p0_y = 0;
    
    const u8* at = (u8*)text;
    for(; *at; at++){
        if(*at == '\n'){
          p0_y -= (font->lineGap + font->ascent - font->descent) * scale;
          size.x = MAX(size.x, p0_x);
          p0_x = 0;
          continue;
        }
        
        Glyph c = font->glyphs[*at];
        p0_x += c.advance * scale;
        
    }
    size.x = MAX(size.x, p0_x);
    size.y += -p0_y + (font->lineGap + font->ascent - font->descent) * scale;

    return size;
  }
};
