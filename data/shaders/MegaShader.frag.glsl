#version 330
#define DIFFUSE_TEXTURE 0
#define LIGHT_TEXTURE 0
//////////////////////////////////////////////////////////////////////
#if DIFFUSE_TEXTURE
uniform sampler2DArray diffuse_texture;
#endif

#if LIGHT_TEXTURE
uniform sampler2DArray light_texture;
#endif

//////////////////////////////////////////////////////////////////////
//Fragment Data
in vec3 pass_frag_tc;

in vec4 pass_frag_tint_color;

//Light Data
in vec3 pass_light_tc;

//////////////////////////////////////////////////////////////////////
//Out
out vec4 frag_color;

void main(){ 
  frag_color = vec4(1,1,1,1);

#if DIFFUSE_TEXTURE
  frag_color = texture(diffuse_texture, pass_frag_tc);
#endif
  
#if LIGHT_TEXTURE
  frag_color *= texture(light_texture, pass_light_tc);
#endif
  
  frag_color *= pass_frag_tint_color;
  if(frag_color.a == 0.0f)
    discard;
}
