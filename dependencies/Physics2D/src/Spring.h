#ifndef SPRING_H
#define SPRING_H

#include "RigidBody.h"

struct Spring{
  RigidBody* A;
  RigidBody* B;
  f32 length;
  f32 k; //spring constant [-n, 0]
  f32 b; //damplening [0, 1]
};
Spring CreateSpring(RigidBody* A, RigidBody* B, f32 length, f32 k, f32 b){
  Spring spring;
  spring.A = A;
  spring.B = B;
  spring.length = length;
  spring.k = MIN(0, k);
  spring.b = MATH::Clamp(b, 0, 1);
  return spring;
}

void ApplySpringForce(Spring spring){
  auto relPos = spring.B->position - spring.A->position;
  auto relVel = spring.B->linearVelocity - spring.A->linearVelocity;

  // Prevent underflow
  for (int i = 0; i < 2; ++i) {
    relPos[i] = (ABS(relPos[i]) < 0.0000001f) ? 0.0f : relPos[i];
    relVel[i] = (ABS(relVel[i]) < 0.0000001f) ? 0.0f : relVel[i];
  }

  float x = MATH::Length(relPos) - spring.length;
  float v = MATH::Length(relVel);
  float F = (-spring.k * x) + (-spring.b * v);

  auto impulse = MATH::Normalize(relPos) * F;

  spring.A->linearVelocity += impulse * GetInverseMass(spring.A->mass);
  spring.B->linearVelocity -= impulse * GetInverseMass(spring.B->mass);
}
#endif
