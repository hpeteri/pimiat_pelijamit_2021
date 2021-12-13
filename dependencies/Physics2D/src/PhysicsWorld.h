#ifndef PHYSICSWORLD_H
#define PHYSICSWORLD_H

#include "Types.h"
#include "Array.h"
#include "QuadTree.h"

#include "ForceGenerator.h"
#include "RigidBody.h"
#include "Collider.h"

#include "PhysicsWorldEvents.h"

struct CollisionPair{
  u64 b_id;
  Collider* b;
  CollisionManifold manifold;
};
struct Collisions{
  u64 a_id;
  Collider* a;
  Array<CollisionPair> collisions;
};
enum PHYSICS_WORLD_FLAGS{
  IS_RIGID_BODIES_SORTED = 1,
  IS_CIRCLE_COLLIDERS_SORTED = 2,
  IS_BOX_COLLIDERS_SORTED = 4,
};
struct PhysicsWorld{
  u64 runningRigidBody_id; //used to generate new id
  u64 runningCollider_id;
  
  void* (*Allocate)(size_t);
  void (*Free)(void*);

  u32 flags;
  
  //////////////////////////////////////////////////////////////////////
  Array<ForceGenerator> forceGenerators;
  //////////////////////////////////////////////////////////////////////
  //Bodies
  Array<RigidBody>      rigidBodies;  
  //////////////////////////////////////////////////////////////////////
  //Colliders
  Array<CircleCollider> circleColliders;
  Array<BoxCollider>    boxColliders;
  
  //////////////////////////////////////////////////////////////////////
  QuadTree<u64> quadTree;
  //////////////////////////////////////////////////////////////////////
  //Collision handling
  Array<Collider*>  collidersToSolve; //After syncing, handle collisions for these colliders
  Array<u64> movedRigidBodies; //Used to determine which colliders need to be synced
  Array<u64> removedRigidBodies; //Colliders use this to remove remove themselves
  Array<u64> collidersInPartition;
  Array<Collisions> collisionPairs;

  //Events
  Array<TriggerEvent>   triggerEvents;
  Array<CollisionEvent> collisionEvents;

  Array<TriggerEvent>   triggerEvents_prev;
  Array<CollisionEvent> collisionEvents_prev;
};

void InitPhysicsWorld(PhysicsWorld* world,
          f32 min_x, f32 min_y,
          f32 max_x, f32 max_y,
          void* (*Allocate)(size_t), void (*Free)(void*));

//////////////////////////////////////////////////////////////////////
//Bodies
u64 CreateRigidBody(PhysicsWorld* world, f32 mass, f32 rotation, f32 x, f32 y);
bool RemoveRigidBody(PhysicsWorld* world, u64 rigidBody_id);


//////////////////////////////////////////////////////////////////////
//Colliders
u64 CreateCollider(PhysicsWorld* world, u64 rigidBody, Circle shape, COLLIDER_FLAGS flags = (COLLIDER_FLAGS)0);
u64 CreateCollider(PhysicsWorld* world, u64 rigidBody, Rectangle shape, COLLIDER_FLAGS flags = (COLLIDER_FLAGS)0);
u64 CreateCollider(PhysicsWorld* world, u64 rigidBody, OrientedRectangle shape, COLLIDER_FLAGS flags = (COLLIDER_FLAGS)0);

void RemoveCollider(PhysicsWorld* world, u64 id);
//////////////////////////////////////////////////////////////////////
void CreateForceGenerator(PhysicsWorld*, f32 dir_x, f32 dir_y, f32 force);

//////////////////////////////////////////////////////////////////////
RigidBody* GetRigidBodyWithID(PhysicsWorld* world, u64 id);

void SetInertiaForRigidBody_Circle(PhysicsWorld* world, u64 id, f32 r); 
void SetInertiaForRigidBody_Rectangle(PhysicsWorld* world, u64 id, f32 width, f32 height);

CircleCollider* GetCircleColliderWithID(PhysicsWorld* world, u64 id);
BoxCollider* GetBoxColliderWithID(PhysicsWorld* world, u64 id);

//////////////////////////////////////////////////////////////////////
void PhysicsUpdate(PhysicsWorld* world, f32 dt);
void ResetPerFrame(PhysicsWorld* world);
//////////////////////////////////////////////////////////////////////
#endif
