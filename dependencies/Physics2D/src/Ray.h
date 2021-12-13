#ifndef RAY_H
#define RAY_H

#include "Math_Vector.h"
#include "PhysicsWorld.h"

struct RayCastHit{
  bool isHit;
  MATH::Vector2<f32> p0;
  u64 rigidBody_id;
};
struct Ray{
  MATH::Vector2<f32> p0;
  MATH::Vector2<f32> dir;
};

RayCastHit CastRay(PhysicsWorld* world, MATH::Vector2<f32> p0, f32 rotation, u64 ignore_rigidBody_id , bool ignoreTriggers, Array<MATH::Vector2<f32>>* dst);
 
#endif
