#include "ParticleEmitter.h"
#include "Math_Random.h"

static f32 GetRandom(RandomVariable random){
  return MATH::RandomInRange_f32(random.min, random.max);
}
static MATH::Vector4<f32> GetRandom(RandomColor random){
  MATH::Vector4<f32> color;
  for(u32 i = 0; i < 4; i++)
    color[i] = MATH::RandomInRange_f32(random.min[i], random.max[i]);
  return color;
}
static f32 Lerp(f32 p0, f32 p1, f32 ratio){
  return p0 + (p1 - p0) * ratio;
}

static u32 LerpColor(MATH::Vector4<f32> c0, MATH::Vector4<f32> c1, f32 ratio){
  u32 color = 0;
  for(u32 i = 0; i < 4; i++){
    f32 c = Lerp(c0[i], c1[i], ratio);
    u8 it = (u8)(0xff * c);
    color |= it << (i *  8);
  }
  return color;
}
ParticleEmitter CreateParticleEmitter(void* (*Allocate)(size_t), void (*Free)(void*)){
  ParticleEmitter emitter = {0};
  emitter.particles.Init(Allocate, Free);


  //////////////////////////////////////////////////////////////////////
  return emitter;
}
inline void SimParticle(Particle* particle, f32 dt){
  particle->lifeTime += dt;
  particle->position += particle->linearVelocity * dt;    

  if(particle->fadeInTime){
    f32 ratio = MATH::Clamp((particle->lifeTime / particle->fadeInTime), 0.0f, 1.0f);
    particle->color = LerpColor(particle->fadeInColor, particle->targetColor, ratio);
    particle->size = Lerp(particle->fadeInSize, particle->targetSize, ratio);
  }
  
  f32 delta = (particle->fadeInTime + particle->lifeTime_max);
  if(particle->lifeTime >= delta){
    f32 ratio = MATH::Clamp(((particle->lifeTime - delta) / particle->fadeOutTime), 0.0f, 1.0f);
          
    particle->color = LerpColor(particle->targetColor, particle->fadeOutColor, ratio);
    particle->size = Lerp(particle->targetSize, particle->fadeOutSize, ratio);
  }
}
void DrawParticleEmitter(const ParticleEmitter* emitter){
  offscreenState.material = diffuseMaterial;
  offscreenState.material.diffuseTexture = particleTexture;
  offscreenState.stateBits = RENDERER::BLEND_BIT;
  
  auto b = RENDERER::BATCH::BeginBatch(&renderer.batchRenderer,
                                       "quad",
                                       offscreenState);

  RENDERER::BATCH::Instance instance = {0};
  for(u32 i = 0; i < emitter->particles.count; i++){
    auto particle = emitter->particles.data[i];

    instance.model = 
      MATH::Translate(particle.position.x, particle.position.y, 1) *
      MATH::RotateZ(particle.rotation) *
      MATH::Scale(particle.size, particle.size, 1);
    
    instance.color = particle.color;
    instance.diffuseLayer = (f32)particle.spriteIndex;
    instance.rotationToLight = particle.rotation;

    if(!RENDERER::BATCH::PushInstance(b, instance))
      b = RENDERER::BATCH::BeginBatch(&renderer.batchRenderer, "quad", offscreenState);
    
  }
}
void UpdateParticleEmitter(ParticleEmitter* emitter, f32 dt){
  
  if(!emitter->paused){
    emitter->spawnCounter += dt * emitter->particlesPerSecond;
    emitter->timer += dt;
  }
  
  u32 freeSlot = 0;
  for(u32 i = 0; i < emitter->particles.count; i++){
    auto particle = emitter->particles[i];
    if(particle->lifeTime + dt >=
       particle->lifeTime_max + particle->fadeOutTime + particle->fadeInTime){
      continue;
    }
   
    SimParticle(particle, dt);
    emitter->particles.data[freeSlot] = *particle;
    freeSlot++;    
  }
  emitter->particles.count = freeSlot;
  
  u32 particlesToSpawn = MATH::Floor(emitter->spawnCounter);
  if(!particlesToSpawn) return;
  
  emitter->spawnCounter -= particlesToSpawn;
  
  Particle p = {0};  
  const f32 dtPerParticle = 1 / (f32)emitter->particlesPerSecond;

  
  for(u32 i = 0; i < particlesToSpawn; i++){
    p.spriteIndex = MATH::Round(GetRandom(emitter->perParticle.spriteIndex));
    
    p.lifeTime_max = GetRandom(emitter->perParticle.lifeTime);
    p.fadeInTime   = GetRandom(emitter->perParticle.fadeIn);
    p.fadeOutTime  = GetRandom(emitter->perParticle.fadeOut);
    
    f32 simTime = (particlesToSpawn - (i+1)) * dtPerParticle;
    if(simTime >= p.lifeTime_max + p.fadeInTime + p.fadeOutTime){  
      continue;
    }
    p.targetColor = GetRandom(emitter->perParticle.color);
    p.fadeInColor = GetRandom(emitter->perParticle.fadeInColor);
    p.fadeOutColor = GetRandom(emitter->perParticle.fadeOutColor);
    p.color = LerpColor(p.targetColor, p.targetColor, 1);
  
    
    p.rotation = emitter->rotation;
    f32 r = GetRandom(emitter->perParticle.angle);
    r += emitter->rotation;

    f32 speed = GetRandom(emitter->perParticle.speed);
    
    p.linearVelocity.x = MATH::Cos(r) * speed;
    p.linearVelocity.y = MATH::Sin(r) * speed;

    p.position.x = GetRandom(emitter->perParticle.pos_x);
    p.position.y = GetRandom(emitter->perParticle.pos_y);
    p.position = MATH::RotateZ(p.position, emitter->rotation);
    p.position += emitter->position;

    p.targetSize = GetRandom(emitter->perParticle.size);
    p.fadeInSize = GetRandom(emitter->perParticle.fadeInSize);
    p.fadeOutSize = GetRandom(emitter->perParticle.fadeOutSize);
    
    p.size = p.targetSize;
    
    
    SimParticle(&p, simTime);
    emitter->particles.Add(p);    
  }
}
