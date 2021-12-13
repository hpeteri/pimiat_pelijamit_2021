#version 330
#define DIFFUSE_TEXTURE 0
#define LIGHT_TEXTURE 0
//////////////////////////////////////////////////////////////////////

#define PI    3.1415926538
#define THETA 6.28318530718

uniform mat4 projection;
//////////////////////////////////////////////////////////////////////
//Vertex Data
layout(location = 0) in vec3 in_vertex_pos;
layout(location = 1) in vec2 in_vertex_tc;
layout(location = 2) in vec4 in_vertex_color;
layout(location = 3) in vec4 in_vertex_override_color;
//////////////////////////////////////////////////////////////////////
//Batch Data
layout(location = 4) in mat4  in_batch_model;
layout(location = 8) in float in_batch_layer;
//////////////////////////////////////////////////////////////////////
//Light Data
layout(location = 9) in float in_light_layer;
layout(location = 10) in float in_light_rotation;
//////////////////////////////////////////////////////////////////////
//Out
out vec3 pass_frag_tc;
out vec3 pass_light_tc;
out vec4 pass_frag_tint_color;
//////////////////////////////////////////////////////////////////////

void main(void){
  gl_Position = projection * in_batch_model * vec4(in_vertex_pos, 1);
  
#if LIGHT_TEXTURE
#if 1
  //Clamp rotation to -45 and 45 degrees
  vec2 tc = in_vertex_tc - 0.5f;
    
  float r = mod(in_light_rotation, THETA);
  r += THETA;

  float section_angle = PI / 2;
  float half_section = section_angle / 2;

  r += half_section;
  //which section we are in
  float section = floor(r /  section_angle);
  r -= half_section;
  r = section * section_angle;
#else
  //keep rotation the same;
  float r = in_light_rotation;
  vec2 tc = in_vertex_tc - 0.5f;
  
#endif


  
  float c = cos(r);
  float s = sin(r);

  pass_light_tc.x =  c * tc.x + s * tc.y + 0.5f;
  pass_light_tc.y = -s * tc.x + c * tc.y + 0.5f;
  pass_light_tc.z = in_light_layer;
#endif
  
#if DIFFUSE_TEXTURE
  pass_frag_tc = vec3(in_vertex_tc, in_batch_layer);
#endif
  
  pass_frag_tint_color = in_vertex_color * in_vertex_override_color;
}

