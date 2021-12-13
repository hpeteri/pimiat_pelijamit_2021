#include "RENDERER_Texture.h"
#include "RENDERER_glError.h"
#include "RENDERER_ASSERT.h"
#include "GL/glew.h"

namespace RENDERER{
  
  GLenum GetTextureFormat(TEXTURE_FORMAT format){
    switch(format){
    case RGBA_8: return GL_RGBA8;
    case R_8: return GL_R8;
    };

    ASSERT(false);

    return GL_INVALID_ENUM;
  }
  GLenum GetInternalFormat(GLenum format){
    switch(format){
    case GL_RGBA8: return GL_RGBA;
    case GL_R8: return GL_RED;
    }

    ASSERT(false);

    return GL_INVALID_ENUM;
  }
  bool InitTextureArray(TextureArray* texture, u32 width, u32 height, u32 levels, u32 mipLevels, TEXTURE_FORMAT format){
    glGenTextures(1, &texture->handle);
    CHECK_GL_ERROR("Create Texture");
    
    texture->type = GL_TEXTURE_2D_ARRAY;
    texture->format = GetTextureFormat(format);
    texture->width = width;
    texture->height = height;
    texture->levels = levels;

    BindTexture(*texture);
    
    glTexStorage3D(texture->type, mipLevels, texture->format, width, height, levels);
    CHECK_GL_ERROR("Texture Storage");

    
    //glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    CHECK_GL_ERROR("Texture params");
 
    return true;
  }
  void SetTextureFilter(TextureArray texture, TEXTURE_FILTER filter){
    int texFilter = GL_INVALID_ENUM;

    if(filter  == TEXTURE_FILTER_NEAREST)
      texFilter = GL_NEAREST;
    
    else if(filter == TEXTURE_FILTER_LINEAR)
      texFilter = GL_LINEAR;
    
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, texFilter);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, texFilter);
    CHECK_GL_ERROR("Set Texture Filter");
  }
  static GLenum GetTextureSwizzle(TEXTURE_SWIZZLE swizzle){
    if(swizzle == TEXTURE_SWIZZLE_RED)   return GL_TEXTURE_SWIZZLE_R;
    if(swizzle == TEXTURE_SWIZZLE_GREEN) return GL_TEXTURE_SWIZZLE_G;
    if(swizzle == TEXTURE_SWIZZLE_BLUE)  return GL_TEXTURE_SWIZZLE_B;
    if(swizzle == TEXTURE_SWIZZLE_ALPHA) return GL_TEXTURE_SWIZZLE_A;
    return GL_INVALID_ENUM;
  }
  static int GetTextureComponent(TEXTURE_COMPONENT component){
    if(component == TEXTURE_COMPONENT_RED)   return GL_RED;
    if(component == TEXTURE_COMPONENT_GREEN) return GL_GREEN;
    if(component == TEXTURE_COMPONENT_BLUE)  return GL_BLUE;
    if(component == TEXTURE_COMPONENT_ALPHA) return GL_ALPHA;
    if(component == TEXTURE_COMPONENT_ONE)   return GL_ONE;
    if(component == TEXTURE_COMPONENT_ZERO)  return GL_ZERO;
    return GL_INVALID_ENUM;  
  }
  void SetTextureSwizzle(TextureArray texture, TEXTURE_SWIZZLE swizzle, TEXTURE_COMPONENT component){
    glTexParameteri(texture.type, GetTextureSwizzle(swizzle), GetTextureComponent(component));
    CHECK_GL_ERROR("Texture Swizzle");  
  }
  void UpdateTextureData(TextureArray texture, u32 width, u32 height, u32 level, void* data){
    glTexSubImage3D(texture.type, 0, 0, 0, level, width, height, 1, GetInternalFormat(texture.format), GL_UNSIGNED_BYTE, data);
    CHECK_GL_ERROR("Update texture data");
  }
  void BindTexture(TextureArray texture){
    glBindTexture(texture.type, texture.handle);
    CHECK_GL_ERROR("Bind texture");
  }
  void UnbindTexture(TextureArray texture){
    glBindTexture(texture.type, 0);
    CHECK_GL_ERROR("Unbind texture");
  }
}
