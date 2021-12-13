#ifndef ENTITY_H
#define ENTITY_H

#include "Types.h"
#include "RigidBody.h"
#include "ParticleEmitter.h"
struct Entity{
  u64 rigidBody_id;
  MATH::Vector2<f32> position; //Got from rigid body
  f32 rotation;
};

struct PlayerPart{
  f32 distanceToBody;
  f32 rotationToBody;
  f32 rotation;

  u32 spriteIndex;
  f32 forceToApply;

  f32 timer;
  
  u8 activationKeyIndex; //WASD
  void (*OnActivate)(PlayerPart* part, RigidBody* rb);

  ParticleEmitter particleEmitter;
};
struct Player : public Entity{
  Array<PlayerPart> parts;
};
struct Bullet{
  u64 rigidBody_id;
  u64 collider_id;
  f32 lifeTime;
};
#endif
