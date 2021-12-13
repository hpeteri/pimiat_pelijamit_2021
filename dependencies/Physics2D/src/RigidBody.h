#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include "Math_Vector.h"



enum RIGID_BODY_TYPE: u8{
  RIGID_BODY_STATIC,
  RIGID_BODY_KINEMATIC,
  RIGID_BODY_DYNAMIC,
};
enum RIGID_BODY_FLAGS: u8{
  
};
struct RigidBody{
  u64 id;
  //////////////////////////////////////////////////////////////////////
  RIGID_BODY_TYPE type;
  RIGID_BODY_FLAGS flags;
  
  f32 mass;    
  f32 rotation;
  MATH::Vector2<f32> position;
  
  //////////////////////////////////////////////////////////////////////
  f32 angularVelocity;
  MATH::Vector2<f32> linearVelocity;
  MATH::Vector2<f32> totalForce;
  f32 totalTorque;

  //////////////////////////////////////////////////////////////////////
  //Used to track if body has moved
  MATH::Vector2<f32> oldPosition;
  f32 oldRotation;
  //////////////////////////////////////////////////////////////////////
  struct{
    f32 mass;
    MATH::Vector2<f32> linearVelocity;
    f32 angularVelocity;
    MATH::Vector2<f32> centerOfMass;
    f32 inertia;
  } system;
};
inline u64 GetRigidBodyID(RigidBody rb){return rb.id;}
//////////////////////////////////////////////////////////////////////
f32 GetInverseMass(f32 mass);
MATH::Vector2<f32> GetAcceleration(MATH::Vector2<f32> force, f32 mass);
//////////////////////////////////////////////////////////////////////
void SetOldTransform(RigidBody* rb);
bool IsMoved(RigidBody* rb);
//////////////////////////////////////////////////////////////////////
void SetInertia_Circle(RigidBody* rb, f32 r);
void SetInertia_Rectangle(RigidBody* rb, f32 width, f32 height);

void ApplyForce(RigidBody* rb, f32 dir_x, f32 dir_y, f32 force);
void ApplyForce(RigidBody* rb, f32 dir_x, f32 dir_y);
void ApplyForce(RigidBody* rb, MATH::Vector2<f32> force);
void ApplyTorque(RigidBody* rb, f32 force, f32 d, f32 rad);

void PhysicsUpdate_RigidBody(RigidBody* rb, f32 dt);
void ResetForces(RigidBody* rb);
#endif
