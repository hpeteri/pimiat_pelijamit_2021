#ifndef PARTICLEEMITTER_H
#define PARTICLEEMITTER_H

#include "Math_Vector.h"
#include "RENDERER_Texture.h"
struct Particle{
  MATH::Vector2<f32> position;
  f32 rotation;
  u32 color; 
  f32 size;
  u32 spriteIndex;
  //////////////////////////////////
  MATH::Vector2<f32> linearVelocity;
  f32 lifeTime;
  f32 lifeTime_max;
  
  f32 fadeInTime;
  f32 fadeOutTime;
  
  
  f32 fadeInSize;
  f32 fadeOutSize;
  f32 targetSize;
  
  MATH::Vector4<f32> fadeInColor;
  MATH::Vector4<f32> targetColor;
  MATH::Vector4<f32> fadeOutColor;
  
};
struct RandomVariable{
  f32 min = 0;
  f32 max = 1;
};
struct RandomColor{
  MATH::Vector4<f32> min;
  MATH::Vector4<f32> max;
};
struct ParticleProperties{
  bool fadeSize;
  bool fadeColor;

  RandomVariable spriteIndex;
  
  RandomVariable lifeTime;
  RandomVariable fadeIn;
  RandomVariable fadeOut;
  RandomVariable angle;
  RandomVariable speed;

  RandomVariable pos_x;
  RandomVariable pos_y;

  RandomVariable size;  
  RandomVariable fadeInSize;
  RandomVariable fadeOutSize;

  RandomColor color;
  RandomColor fadeInColor;
  RandomColor fadeOutColor;
};
struct ParticleEmitter{
  MATH::Vector2<f32> position;
  f32 rotation;
 
  RENDERER::TextureArray texture;
  ParticleProperties perParticle;
  
  f32 linearDrag;
  f32 angularDrag;
  
  f32 particlesPerSecond;
  bool paused;

  

  f32 spawnCounter;
  Array<Particle> particles;
  f32 timer;
};

ParticleEmitter CreateParticleEmitter(void* (*Allocate)(size_t), void (*Free)(void*));
void UpdateParticleEmitter(ParticleEmitter* emitter, f32 dt);
void DrawParticleEmitter(const ParticleEmitter* emitter);
#endif
