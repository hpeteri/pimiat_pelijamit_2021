#include "Math_Vector.h"

#include "RigidBody.h"
#include "Spring.h"
#include "DistanceJoint.h"
#include "FixedJoint.h"

struct Transform{
  MATH::Vector2<f32> position;
  MATH::Vector2<f32> scale;
  f32 rotation;
};

struct Entity{
  Transform transform;
  RigidBody rigidBody;
  u32 color;
};
struct PhysicsWorld{
  Array<RigidBody>     rigidBodies;
  Array<Spring>        springs;
  Array<FixedJoint>    fixedJoints;
  Array<DistanceJoint> distanceJoints;
};
void PhysicsUpdate(Entity* e, f32 dt){ 

  e->rigidBody.totalForce += e->rigidBody.forceAccum;
  e->rigidBody.forceAccum = {0, 0};
  e->rigidBody.totalForce -= e->rigidBody.linearVelocity * linearDrag_;

  e->rigidBody.totalTorque += e->rigidBody.torqueAccum;
  e->rigidBody.torqueAccum = 0;
  e->rigidBody.totalTorque -= e->rigidBody.angularVelocity * angularDrag_;
  
    
  { //linear
    
    //drag force
    
    //v = v * a * dt
    e->rigidBody.linearVelocity += GetAcceleration(e->rigidBody.totalForce, e->rigidBody.mass) * dt;
    
    e->rigidBody.position += e->rigidBody.linearVelocity * dt;


    //Set force to 0

  }
  { //Angular

    e->rigidBody.angularVelocity += e->rigidBody.totalTorque * GetInverseMass(e->rigidBody.mass) * dt;
    
    e->rigidBody.rotation += e->rigidBody.angularVelocity * dt;
    
  }
}
void UpdateEntityPosition(Entity* e){
  //Update entity transform
  e->rigidBody.totalTorque = 0;
  e->rigidBody.totalForce = {0,0};
  
  e->transform.position = e->rigidBody.position;
  e->transform.rotation = e->rigidBody.rotation;
}
