#include "RigidBody.h"
#include <cstdio>

#include "Epsilon.h"

const f32 linearDrag_ = .5f;
const f32 angularDrag_ = 20.5f;

f32 GetInverseMass(f32 mass){
  if(mass == 0.0f) return 0.0f;
  return 1.0f / mass;
}

MATH::Vector2<f32> GetAcceleration(MATH::Vector2<f32> force, f32 mass){
  return force * GetInverseMass(mass);
}
//////////////////////////////////////////////////////////////////////
bool IsMoved(RigidBody* rb){
  if(rb->rotation != rb->oldRotation ||
     rb->position != rb->oldPosition){
    return true;
  }
  return false;
}
//////////////////////////////////////////////////////////////////////
void ApplyForce(RigidBody* rb, f32 dir_x, f32 dir_y, f32 force){
  rb->totalForce.x += dir_x * force;
  rb->totalForce.y += dir_y * force;
}
void ApplyForce(RigidBody* rb, f32 dir_x, f32 dir_y){
  rb->totalForce.x += dir_x;
  rb->totalForce.y += dir_y;
}
void ApplyForce(RigidBody* rb, MATH::Vector2<f32> force){
  rb->totalForce += force;
}
void ApplyTorque(RigidBody* rb, f32 force, f32 d, f32 rad){
  rb->totalTorque += force * d  *  MATH::Sin(rad);
}
void SetOldTransform(RigidBody* rb){
  rb->oldPosition = rb->position;
  rb->oldRotation = rb->rotation;
}
void SetInertia_Circle(RigidBody* rb, f32 r){
  rb->system.inertia = rb->mass * r  * r * 0.25f;
}
void SetInertia_Rectangle(RigidBody* rb, f32 width, f32 height){
  rb->system.inertia = rb->mass * (width  * width  +  height * height) / 12;
}
void PhysicsUpdate_RigidBody(RigidBody* rb, f32 dt){
  SetOldTransform(rb);
  //Drag
  rb->totalForce -= rb->linearVelocity * linearDrag_ * dt;
  rb->totalTorque -= rb->angularVelocity * angularDrag_ * dt;
  
  rb->linearVelocity += GetAcceleration(rb->totalForce, rb->mass) * dt;
  rb->angularVelocity += rb->totalTorque * GetInverseMass(rb->mass) * dt;
  
  auto moveVec = rb->linearVelocity * dt;
  if(ABS(moveVec.x) > epsilon_ || ABS(moveVec.y) > epsilon_)
    rb->position += moveVec;
  
  float rot = rb->angularVelocity * dt;
  if(ABS(rot) > epsilon_)
    rb->rotation += rot;

  //////////////////////////////////////////////////////////////////////
  rb->system.linearVelocity = rb->linearVelocity;
  rb->system.angularVelocity = rb->angularVelocity;

  //Calculated in PhysicsUpdate() PhysicsWorld.cpp
  //rb->system.centerOfMass = rb->position;
  //rb->system.mass = rb->mass;

}
void ResetForces(RigidBody* rb){
  rb->totalForce = {0,0};
  rb->totalTorque = 0;
}
