#ifndef DISTANCEJOINT_H
#define DISTANCEJOINT_H

#include "RigidBody.h"
#include "Math_Vector.h"
struct DistanceJoint{
  RigidBody* A;
  RigidBody* B;

  f32 distance;
};

DistanceJoint CreateDistanceJoint(RigidBody* A, RigidBody* B, f32 distance){
  return DistanceJoint{A, B, distance};
}

//This should be constraint
void SolveDistanceJoint(DistanceJoint joint){
  auto delta = joint.B->position - joint.A->position;
  f32 d = MATH::Length(delta);
  
  f32 correction = 0.0f;
  if(d != 0.0f){
    correction = (d - joint.distance ) / d;
  }else{
    delta = MATH::Vector2<f32>{0.0f, 1.0f};
    *(i32*)nullptr = 0;
      correction = joint.distance;
  }
    
  joint.A->position += delta * correction * 0.5f;
  
  joint.B->position -= delta * correction * 0.5f;
}


#endif
