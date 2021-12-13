#include "RENDERER_Material.h"
#include "RENDERER_BatchRenderer.h"
#include "RENDERER_ImmediateMode.h"
namespace RENDERER{  
  Material CreateMaterial(u32 flags, ShaderSources sources){
    char header[] = "#version 330\n#define DIFFUSE_TEXTURE 0 \n#define LIGHT_TEXTURE 0\n";
    auto len = strlen(header);
    
    sprintf(header,
              "#version 330\n#define DIFFUSE_TEXTURE %d \n#define LIGHT_TEXTURE %d\n",
            (flags & MATERIAL_FLAGS_DIFFUSE_TEXTURE) && 1,
             (flags & MATERIAL_FLAGS_LIGHT_TEXTURE) && 1);
    
    memcpy(sources.sources[0].source, header, len);
    memcpy(sources.sources[1].source, header, len);

    Material material = {0};
    material.flags = flags;
    
    InitShader(&material.shader, sources);
    
    return material;
  }
  void BindMaterial(Material material){
    BindShader(material.shader);
    
    if(material.flags & MATERIAL_FLAGS_DIFFUSE_TEXTURE){
      glActiveTexture(GL_TEXTURE0);
      BindTexture(material.diffuseTexture);
      glUniform1i(material.shader.diffuseTexture_loc, 0);
    }
    
    if(material.flags & MATERIAL_FLAGS_LIGHT_TEXTURE){
      glActiveTexture(GL_TEXTURE1);
      BindTexture(material.lightTexture);
      glUniform1i(material.shader.lightTexture_loc, 1);
    }
    CHECK_GL_ERROR("Bind Material");
  }
  void SetMaterialDescriptors_IM(){
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(8);
  }
  void SetMaterialVertexDescriptors_IM(){
    IM::IM_Vertex vertex;
    glVertexAttribPointer(0, 3,
                          GL_FLOAT, GL_FALSE, sizeof(IM::IM_Vertex),
                          (void*)((char*)&vertex.x -
                                  (char*)&vertex));

    glVertexAttribPointer(1, 2,
                          GL_FLOAT, GL_FALSE, sizeof(IM::IM_Vertex),
                          (void*)((char*)&vertex.tc_x -
                                  (char*)&vertex));

    glVertexAttribPointer(2, 4,
                          GL_UNSIGNED_BYTE, GL_TRUE, sizeof(IM::IM_Vertex),
                          (void*)((char*)&vertex.color -
                                  (char*)&vertex));

    //Layer data, which might not be used. Might be a problem if found in the shader's sources
    glVertexAttribPointer(8, 1,
                          GL_FLOAT, GL_FALSE, sizeof(IM::IM_Vertex),
                          (void*)((char*)&vertex.tc_z -
                                  (char*)&vertex));
    
    //Batch model
    auto model = MATH::Identity();
    glVertexAttrib4f(3, 1, 1, 1, 1);
    glVertexAttrib4fv(4, model[0].A);
    glVertexAttrib4fv(5, model[1].A);
    glVertexAttrib4fv(6, model[2].A);
    glVertexAttrib4fv(7, model[3].A);
  }
  void ResetMaterialDescriptors_IM(){
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(8);
  }
  
  void SetMaterialDescriptors_Batch(){
    for(u32 i = 0; i < 11; i++)
      glEnableVertexAttribArray(i);
  }
  void SetMaterialVertexDescriptors_Batch(){
    Vertex vertex;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tc_x));
    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, color));
  }
  void ResetMaterialDescriptors_Batch(){
    //@NOTE first 3 should always be active
    for(i32 i = 3; i <= 11; i++){
      glVertexAttribDivisor(i, 0);
      glDisableVertexAttribArray(i);
    }
  }
  void SetMaterialInstanceDescriptors_Batch(u32 flags){
    RENDERER::BATCH::Instance inst;
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE,
                          sizeof(inst),
                          (void*)offsetof(typeof(inst), model.col0));
    glVertexAttribDivisor(4, 1);
      
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE,
                          sizeof(inst),
                          (void*)offsetof(typeof(inst), model.col1));
    glVertexAttribDivisor(5, 1);
      
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE,
                          sizeof(inst),
                          (void*)offsetof(typeof(inst), model.col2));
    glVertexAttribDivisor(6, 1);
      
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE,
                          sizeof(inst),
                          (void*)offsetof(typeof(inst), model.col3));
    glVertexAttribDivisor(7, 1);
    
    glVertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_TRUE, 
                          sizeof(inst),
                          (void*)offsetof(typeof(inst), color));
    glVertexAttribDivisor(3, 1);

    if(flags & MATERIAL_FLAGS_DIFFUSE_TEXTURE){
      glVertexAttribPointer(8, 1, GL_FLOAT, GL_FALSE,  
                            sizeof(inst),
                            (void*)offsetof(typeof(inst), diffuseLayer));
      glVertexAttribDivisor(8, 1);
    }

    if(flags & MATERIAL_FLAGS_LIGHT_TEXTURE){
      glVertexAttribPointer(9, 1, GL_FLOAT, GL_FALSE,  
                            sizeof(inst),
                            (void*)offsetof(typeof(inst), lightLayer));
      
      glVertexAttribDivisor(9, 1);
      
      glVertexAttribPointer(10, 1, GL_FLOAT, GL_FALSE,  
                            sizeof(inst),
                            (void*)offsetof(typeof(inst), rotationToLight));
      
      glVertexAttribDivisor(10, 1);
    
    }

    
  }
  
}
