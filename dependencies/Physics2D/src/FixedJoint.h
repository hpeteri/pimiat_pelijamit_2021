#ifndef FIXEDJOINT_H
#define FIXEDJOINT_H


#include "RigidBody.h"
#include "Math_Vector.h"

struct FixedJoint{
  RigidBody* A;
  RigidBody* B;

  
  f32 rotation;
  f32 directionToBody;
  f32 distance;
};

FixedJoint CreateFixedJoint(RigidBody* A, RigidBody* B){
  auto delta = B->position - A->position; 
  return FixedJoint{A, B, B->rotation - A->rotation, MATH::Atan2(delta.y, delta.x), MATH::Length(delta)};
}
void SolveFixedJoint(FixedJoint joint){
  printf("joint.direction :: %f\n", joint.directionToBody);
  {//Distance
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
    
    auto p0 = joint.A->position +
      MATH::Vector2<f32>{MATH::Cos(joint.A->rotation + joint.directionToBody),
      MATH::Sin(joint.A->rotation + joint.directionToBody)} * joint.distance;

    
    RENDERER::IM::Immediate_Line(&renderer.imRenderer,
                                 p0.x,
                                 p0.y,
                                 joint.A->position.x,
                                 joint.A->position.y,
                                 2.0f,
                                 (u32)0xff0000ff);
    
    
    RENDERER::IM::Immediate_Line(&renderer.imRenderer,
                                 p0.x,
                                 p0.y,
                                 p0.x + MATH::Cos(joint.A->rotation + joint.rotation) * 10,
                                 p0.y + MATH::Sin(joint.A->rotation + joint.rotation) * 10,
                                 1.0f,
                                 (u32)0xffffff00);
    
    auto p1 = joint.B->position + 
      MATH::Vector2<f32>{MATH::Cos(joint.B->rotation - joint.directionToBody),
      MATH::Sin(joint.B->rotation - joint.directionToBody)} * joint.distance;;
   
    RENDERER::IM::Immediate_Line(&renderer.imRenderer,
                                 joint.B->position.x,
                                 joint.B->position.y,
                                 p1.x,
                                 p1.y,
                                 2.0f,
                                 (u32)0xff00ffff);

    RENDERER::IM::Immediate_Line(&renderer.imRenderer,
                                 p1.x,
                                 p1.y,
                                 p1.x + MATH::Cos(joint.B->rotation - joint.rotation) * 10,
                                 p1.y + MATH::Sin(joint.B->rotation - joint.rotation) * 10,
                                 1.0f,
                                 (u32)0xffff00ff);
    
    
    

    
    joint.B->position = joint.A->position +
      MATH::Vector2<f32>{MATH::Cos(joint.A->rotation + joint.directionToBody),
      MATH::Sin(joint.A->rotation + joint.directionToBody)} * joint.distance;

    //joint.A->rotation = joint.B->rotation - joint.rotation;
    joint.B->rotation = joint.A->rotation + joint.rotation;


    return;
    joint.A->position = joint.B->position +
      MATH::Vector2<f32>{MATH::Cos(joint.B->rotation - joint.directionToBody),
      MATH::Sin(joint.B->rotation - joint.directionToBody)} * joint.distance;
    
    joint.A->rotation = joint.B->rotation - joint.rotation;
    //joint.B->rotation = joint.A->rotation + joint.rotation;

  }  
}

void SolveFixedJointForces(u32 jointCount, FixedJoint* joints){
  
  for(i32 i = 0; i < jointCount; i++){
    auto joint = joints[i];

    RigidBody* bodies[2] = {joint.A, joint.B};
    auto totalMass = joint.A->mass + joint.B->mass;
          
    for(int i = 0; i < 2; i++){
      auto A = bodies[i];
      auto B = bodies[(i + 1) % 2];
      if(A->totalForce.x != 0.0f || A->totalForce.y != 0.0f){
        
        ApplyTorqueThroughJoint(B,
                                MATH::Length(A->totalForce),
                                joint.distance,
                                MATH::Atan2(A->totalForce.y, A->totalForce.x) - (bodies[0]->rotation + joint.directionToBody));  

        ApplyForceThroughJoint(B,
                               A->totalForce.x,
                               A->totalForce.y,
                               B->mass / totalMass);
        
        ApplyForceThroughJoint(A, 
                               -A->totalForce.x, 
                               -A->totalForce.y, 
                               B->mass / totalMass);
                
       
      }
    }
  }
}

#endif
