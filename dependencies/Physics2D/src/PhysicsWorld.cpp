#include "PhysicsWorld.h"
//////////////////////////////////////////////////////////////////////
void InitPhysicsWorld(PhysicsWorld* world, f32 min_x, f32 min_y, f32 max_x, f32 max_y, void* (*Allocate)(size_t), void (*Free)(void*)){
  memset(world, 0, sizeof(*world));

  world->Allocate = Allocate;
  world->Free = Free; 

  world->forceGenerators.Init(Allocate, Free);
  world->rigidBodies.Init(Allocate, Free);
  world->circleColliders.Init(Allocate, Free);
  world->boxColliders.Init(Allocate, Free);
  
  world->quadTree.Init({min_x, min_y}, {max_x, max_y}, 4, Allocate, Free);
  world->movedRigidBodies.Init(Allocate, Free);
  world->removedRigidBodies.Init(Allocate, Free);
  world->collidersToSolve.Init(Allocate, Free);
  world->collidersInPartition.Init(Allocate, Free);
  world->collisionPairs.Init(Allocate, Free);

  world->collisionEvents.Init(Allocate, Free);
  world->triggerEvents.Init(Allocate, Free);
  
  world->collisionEvents_prev.Init(Allocate, Free);
  world->triggerEvents_prev.Init(Allocate, Free);
}
inline u64 GetItself(u64 it){return it;}

static void ApplyForceGenerators(PhysicsWorld* world, RigidBody* rb){
  for(u32 i = 0; i < world->forceGenerators.count; i++){
    auto gen = world->forceGenerators[i];
    ApplyForce(rb, gen->dir_x, gen->dir_y, gen->force);
  }
}

static RigidBody* GetMovedRigidBodyWithID(PhysicsWorld* world, u64 id){
  if(!id) return nullptr;
  
  if(!world->movedRigidBodies.Search_Binary(id, GetItself)){

    return nullptr;
  }

  auto it = GetRigidBodyWithID(world, id);
  if(!it){
    printf("Failed to find rigid body, which was moved\n");
  }
  return it; 
}

static void SetRigidBodyToBeSynced(PhysicsWorld* world, u64 rigidBody_id){
  world->movedRigidBodies.Add(rigidBody_id);
}

static void SortMovedRigidBodies(PhysicsWorld* world){
  world->movedRigidBodies.Sort_Radix256(GetItself);
}

static void SortRigidBodies(PhysicsWorld* world){
  if(!(world->flags & IS_RIGID_BODIES_SORTED)){
    world->rigidBodies.Sort_Radix256(GetRigidBodyID);
    world->flags |= IS_RIGID_BODIES_SORTED;
  }
}

static void SortCircleColliders(PhysicsWorld* world){
  if(!(world->flags & IS_CIRCLE_COLLIDERS_SORTED)){
    world->circleColliders.Sort_Radix256(GetColliderID);
    world->flags |= IS_CIRCLE_COLLIDERS_SORTED;
  }
}

static void SortBoxColliders(PhysicsWorld* world){
  if(!(world->flags & IS_BOX_COLLIDERS_SORTED)){
    world->boxColliders.Sort_Radix256(GetColliderID);
    world->flags |= IS_BOX_COLLIDERS_SORTED;
  }
}

template <typename T>
static void SyncCollider(PhysicsWorld* world, T* collider, RigidBody* rb){
  if(rb){
    collider->position = rb->position;
    collider->rotation = rb->rotation;
  }
  world->quadTree.Remove(collider->id, collider->aabb.min, collider->aabb.max);

  GetGlobalMinMaxPoints(collider->collider, collider->aabb.minMax, collider->position, collider->rotation);
  world->quadTree.Insert(collider->id, collider->aabb.min, collider->aabb.max);
}

template <typename T>
static bool SyncCollider_IfMoved(PhysicsWorld* world, T* collider){
  RigidBody* rb = GetMovedRigidBodyWithID(world, collider->rigidBody_id);
  if(!rb)
    return false;
  
  SyncCollider(world, collider, rb);
  return true;  
}
#include "PhysicsWorld_ObjectCreation.cpp"
//////////////////////////////////////////////////////////////////////
static bool IsAttachmentPointRemoved(PhysicsWorld* world, u64 rigidBody_id){
  if(!rigidBody_id) return false;

  return world->removedRigidBodies.Search_Binary(rigidBody_id, GetItself);
}
static void SyncColliders(PhysicsWorld* world){
  SortBoxColliders(world);
  SortCircleColliders(world);
  SortMovedRigidBodies(world);
  
  world->removedRigidBodies.Sort_Radix256(GetItself);
  
  for(u32 i = 0; i < world->circleColliders.count; i++){
    auto it = world->circleColliders[i];
    if(IsAttachmentPointRemoved(world, it->rigidBody_id)){
      world->quadTree.Remove(it->id, it->aabb.min, it->aabb.max);
      world->circleColliders.RemoveAndKeepOrder(i);      
      i --;
      continue;
    }
    
    it->perFrame.uniqueCollisions = 0;
    if(SyncCollider_IfMoved(world, it)){
      it->perFrame.hasMoved = true;
      world->collidersToSolve.Add(it);
    }else{
      it->perFrame.hasMoved = false;
    } 
  }
  for(u32 i = 0; i < world->boxColliders.count; i++){
    auto it = world->boxColliders[i];

    if(IsAttachmentPointRemoved(world, it->rigidBody_id)){
      world->quadTree.Remove(it->id, it->aabb.min, it->aabb.max);
      world->boxColliders.RemoveAndKeepOrder(i);
      i --;
      continue;
    }
    it->perFrame.uniqueCollisions = 0;
    if(SyncCollider_IfMoved(world, it)){
      it->perFrame.hasMoved = true;
      world->collidersToSolve.Add(it);
    }else{
      it->perFrame.hasMoved = false;
    }
  }
}
static Collider* GetColliderWithID(PhysicsWorld* world, u64 id){
  Collider* collider = GetBoxColliderWithID(world, id);
  if(collider)
    return collider;
  
  collider = GetCircleColliderWithID(world, id);
  if(collider)
    return collider;

  return nullptr;
}
static void MaybeAddTriggerEvent(PhysicsWorld* world,
                                 RigidBody* a, COLLIDER_FLAGS a_flags,
                                 RigidBody* b, COLLIDER_FLAGS b_flags){
  if(a_flags & IS_TRIGGER && b_flags & IS_TRIGGER) return; //Both are triggers
  
  if(a_flags & IS_TRIGGER){
    TriggerEvent event;
    event.type = PHYSICS_WORLD_EVENT_ENTER;
    event.rigidBody_id_a = a->id;
    event.rigidBody_id_b = b ? b->id : 0;
    world->triggerEvents.Add(event);
  }
  if(!b) return;
  
  if(b_flags & IS_TRIGGER){
    TriggerEvent event;
    event.type = PHYSICS_WORLD_EVENT_ENTER;
    event.rigidBody_id_a = b->id;
    event.rigidBody_id_b = a->id;
    world->triggerEvents.Add(event);
  }
  
}
static void MaybeAddCollisionEvent(PhysicsWorld* world,
                                   RigidBody* a, COLLIDER_FLAGS a_flags,
                                   RigidBody* b, COLLIDER_FLAGS b_flags){
  if(a_flags & IS_COLLISION_CALLBACK && !((a_flags | b_flags) & IS_TRIGGER)){
    CollisionEvent event;
    event.type = PHYSICS_WORLD_EVENT_ENTER;
    event.rigidBody_id_a = a->id;
    event.rigidBody_id_b = (b) ? b->id : 0;
    world->collisionEvents.Add(event);
  }
  if(!b) return;
  if(b_flags & IS_COLLISION_CALLBACK&& !((a_flags | b_flags) & IS_TRIGGER)){
    CollisionEvent event;
    event.type = PHYSICS_WORLD_EVENT_ENTER;
    event.rigidBody_id_a = b->id;
    event.rigidBody_id_b = a->id;
    world->collisionEvents.Add(event);
  }  
}
static bool IgnoreCollisionImpulse(RigidBody* rb, COLLIDER_FLAGS flags_a, COLLIDER_FLAGS flags_b){
  if(!rb) return true;
  if(flags_a & IS_TRIGGER || flags_b & IS_TRIGGER) return true;

  return false;
}
static void SolveCollision(PhysicsWorld* world, f32 dt, Collisions* collisionsForA){
  Collider* collider_a = collisionsForA->a;
  RigidBody* rb_a = GetMovedRigidBodyWithID(world, collider_a->rigidBody_id);
  if(!rb_a){
    printf("Entity %d already got removed, basically created and removed during the same frame\n", collider_a->rigidBody_id);
    //Rigid body got removed, but we don't remove it from the moved rigid bodies array
    return;
  }
  const f32 mass_a = rb_a->system.mass;
  
  
  for(u32 ii = 0; ii < collisionsForA->collisions.count; ii++){
    CollisionPair* collision = collisionsForA->collisions[ii];
    Collider* collider_b = collision->b;
    
    RigidBody* rb_b = collider_b->perFrame.hasMoved ?
      GetMovedRigidBodyWithID(world, collider_b->rigidBody_id) : //This is faster
      GetRigidBodyWithID(world, collider_b->rigidBody_id);
    //////////////////////////////////////////////////////////////////////
    MaybeAddCollisionEvent(world, rb_a, collider_a->flags,
                           rb_b, collider_b->flags);
    
    MaybeAddTriggerEvent(world, rb_a, collider_a->flags,
                         rb_b, collider_b->flags);
    
    
    //////////////////////////////////////////////////////////////////////  
    auto pointOfContact = collision->manifold.pointOfContact;
    auto normal = collision->manifold.normal;
    auto depth = collision->manifold.depth;
        
    const f32 mass_b = rb_b ? rb_b->system.mass : mass_a;
    const f32 mass_total = mass_a + mass_b;
    if(!mass_total)
      continue;

    const f32 inv_mass_total = GetInverseMass(mass_a) + GetInverseMass(mass_b);
   
    //Calculate final velocity
    auto arm_a = pointOfContact - rb_a->system.centerOfMass;
    auto rotV_a = MATH::Normal(normal * MATH::Length(arm_a) * rb_a->system.angularVelocity);
    auto Vf_a = rb_a->system.linearVelocity + rotV_a;
                
    auto arm_b = pointOfContact - (rb_b ? rb_b->system.centerOfMass : collider_b->position);
    auto rotV_b = rb_b ? MATH::Normal(arm_b * rb_b->system.angularVelocity) : typeof(rotV_a){0, 0};
    auto Vf_b = rotV_b + (rb_b ? rb_b->system.linearVelocity : typeof(rotV_a){0, 0});

    auto V_rel = Vf_a - Vf_b;

    f32 inertia_a = rb_a->system.inertia;
        
    f32 inertia_b = rb_b ? rb_b->system.inertia : mass_b * (MATH::Pow2((collider_b->aabb.max.x  - collider_b->aabb.min.x)) + 
                                                            MATH::Pow2((collider_b->aabb.max.y  - collider_b->aabb.min.y))) / 12;
    
        
    auto Iaug_a = MATH::CrossProduct2D(arm_a, normal);
    if(inertia_a != 0.0f)
      Iaug_a = (Iaug_a / inertia_a) * Iaug_a;
    else
      Iaug_a = 0;
    
    auto Iaug_b = MATH::CrossProduct2D(arm_b, normal);        
    if(inertia_b != 0.0f)
      Iaug_b = (Iaug_b / inertia_b) * Iaug_b;
    else
      Iaug_b = 0;
    
    f32 e = 0.95f;
    auto numerator = -(1 + e) * MATH::DotProduct(V_rel, normal);
    auto j = numerator / (inv_mass_total + Iaug_a + Iaug_b);

    
    auto impulse = normal * j;
    //////////////////////////////////////////////////////////////////////
    // RigidBody A
    if(!IgnoreCollisionImpulse(rb_a, collider_a->flags, collider_b->flags)){
      rb_a->position += collision->manifold.normal * collision->manifold.depth;
      auto angularImpulse_a = (inertia_a != 0.0f)? (MATH::CrossProduct2D(arm_a, impulse) / inertia_a) : 0;
    
      rb_a->linearVelocity += impulse * GetInverseMass(mass_a) / collider_a->perFrame.uniqueCollisions;  
      rb_a->angularVelocity += angularImpulse_a / collider_a->perFrame.uniqueCollisions;
    }
    //////////////////////////////////////////////////////////////////////  
    // RigidBody B
    if(!IgnoreCollisionImpulse(rb_b, collider_b->flags, collider_a->flags)){
      
      auto angularImpulse_b = (inertia_b != 0.0f) ? (MATH::CrossProduct2D(arm_b, impulse) / inertia_b) : 0;
      
      
      rb_b->linearVelocity -= impulse * GetInverseMass(mass_b) / collider_b->perFrame.uniqueCollisions;
      rb_b->angularVelocity -= angularImpulse_b / collider_b->perFrame.uniqueCollisions;
    }
    //////////////////////////////////////////////////////////////////////
  }      
}
static void SolveCollisions(PhysicsWorld* world, f32 dt){
  //Gather every unique collision pair
  for(u32 i = 0; i < world->collidersToSolve.count; i++){
    Collider* collider_a = *world->collidersToSolve[i];
    world->quadTree.GetUniqueElementsInOccupiedPartitions(collider_a->aabb.min, collider_a->aabb.max,
                                                          &world->collidersInPartition);
    
    Collisions* collisionsForA = world->collisionPairs[i];
    if(!collisionsForA){ //Array needs to be expanded  
      collisionsForA = world->collisionPairs.Add(nullptr);
      //Initialize every array, this is just stupid, but don't have a solution for this yet
      for(u32 i = world->collisionPairs.count - 1; i < world->collisionPairs.size; i++){
        auto it = world->collisionPairs[i];          
        it->collisions.Init(world->Allocate, world->Free);
      }
    }else{
      collisionsForA = world->collisionPairs.Add(nullptr);
    }
    collisionsForA->collisions.count = 0;
    collisionsForA->a_id = collider_a->id;
    collisionsForA->a = collider_a;
    
    for(u32 ii = 0; ii < world->collidersInPartition.count; ii++){
      //@TODO
      //We could add a frame counter to partition for when collider was moved in the tree
      //so we don't have find b to know if it was moved
      u64 b_id = *world->collidersInPartition[ii]; 
      
      //Don't collide with self
      if(collider_a->id == b_id)
        continue;
        
      Collider* collider_b = GetColliderWithID(world, b_id); //@NOTE this might be expensive to do
      
      //Both colliders have moved
      //so if b has a larger id, then it is already been added by a
      //If b hasn't moved, then a has to add it even if it has a larger id
      if(collider_b->perFrame.hasMoved && collider_b->id > collider_a->id){
        //printf("!!! %d should be handled by %d\n !!!", collider_b->id, collider_a->id);
        continue;
      }
      
      //Attached to same body
      if(collider_a->rigidBody_id == collider_b->rigidBody_id)
        continue;

      //Broad collision check
      if(!CheckCollision_AABB(collider_a->aabb.min, collider_a->aabb.max,
                              collider_b->aabb.min, collider_b->aabb.max)){
        continue;
      }
      
      //Collision check
      CollisionManifold collision = GetCollisionManifold(collider_a, collider_b);
      if(collision.depth == 0){
        continue;
      }
      collider_a->perFrame.uniqueCollisions ++;
      collider_b->perFrame.uniqueCollisions ++;
      CollisionPair* collisionPair = collisionsForA->collisions.Add(nullptr);
      collisionPair->b_id = b_id;
      collisionPair->b = collider_b;
      collisionPair->manifold = collision;
    }
  }
  for(u32 i = 0; i < world->collisionPairs.count; i++){
    SolveCollision(world, dt, world->collisionPairs[i]);
  }

}
static void SwapEventBuffers(PhysicsWorld* world){
  {
    auto temp = world->collisionEvents;
    world->collisionEvents = world->collisionEvents_prev;
    world->collisionEvents_prev = temp;
    world->collisionEvents.count = 0;
  }
  {
    auto temp = world->triggerEvents;
    world->triggerEvents = world->triggerEvents_prev;
    world->triggerEvents_prev = temp;
    world->triggerEvents.count = 0;
  }
}
static bool CorrectEventIfMatch(PhysicsWorldEvent* eventToCorrect, PhysicsWorldEvent* lastFrameEvent){
  if(eventToCorrect->rigidBody_id_b != lastFrameEvent->rigidBody_id_b) return false;
  
  eventToCorrect->type = PHYSICS_WORLD_EVENT_STAY;
  return true;  
}
static void CorrectEventTypes(Array<PhysicsWorldEvent> &events,
                              Array<PhysicsWorldEvent> &events_prev){
  
  events.Sort_Radix256(GetEventRigidBodyA);
  for(u32 i = 0; i < events.count; i++){
    auto eventToCheck = events[i];

    auto lastFrameEvent = events_prev.Search_Binary(*eventToCheck, GetEventRigidBodyA);
    u32 startIndex = events_prev.GetIndexOf(lastFrameEvent);
    bool found = false;
    for(u32 ii = startIndex; ii < events_prev.count; ii ++){
      lastFrameEvent = events_prev[ii];
      if(CorrectEventIfMatch(eventToCheck, lastFrameEvent)){
        found = true;
        break;
      }
    }
    if(found) continue;
    for(i32 ii = (i32)startIndex - 1; ii >= 0 && ii < events_prev.count; ii --){
      lastFrameEvent = events_prev[ii];
      if(CorrectEventIfMatch(eventToCheck, lastFrameEvent)){
        found = true;
        break;
      }
    }
    if(found) continue;
  }

  u32 oldCount = events.count;
  for(u32 i = 0; i < events_prev.count; i++){
    auto eventToCheck = events_prev[i];

    auto lastFrameEvent = events.Search_Binary(*eventToCheck, GetEventRigidBodyA, 0, oldCount);
    u32 startIndex = events.GetIndexOf(lastFrameEvent); 
    bool found = false;
    for(u32 ii = startIndex; ii < oldCount; ii ++){
      lastFrameEvent = events[ii];
      if(eventToCheck->rigidBody_id_b == lastFrameEvent->rigidBody_id_b){
        found = true;
        break;
      }
    }
    if(found) continue;
    for(i32 ii = (i32)startIndex - 1; ii >= 0  && ii < events.count; ii --){
      lastFrameEvent = events[ii];
      if(eventToCheck->rigidBody_id_b == lastFrameEvent->rigidBody_id_b){
        found = true;
        break;
      }
    }
    
    if(found) continue;
    if(eventToCheck->type == PHYSICS_WORLD_EVENT_LEAVE) continue;
    //Not found in the currenc event list, so add leave event
    auto newEvent = *eventToCheck;
    newEvent.type = PHYSICS_WORLD_EVENT_LEAVE;
    events.Add(newEvent);                
  }
}
void PhysicsUpdate(PhysicsWorld* world, f32 dt){
  //////////////////////////////////////////////////////////////////////
  for(u32 i = 0; i < world->rigidBodies.count; i++){
    auto rb = world->rigidBodies[i];
    
    ApplyForceGenerators(world, rb);
    PhysicsUpdate_RigidBody(rb, dt);
    rb->system.centerOfMass = rb->position;
    
    if(IsMoved(rb)){
      SetRigidBodyToBeSynced(world, rb->id);
    }
    ResetForces(rb);
  }
  //////////////////////////////////////////////////////////////////////
  
  //////////////////////////////////////////////////////////////////////
  //Solve collisions
  SyncColliders(world); //Sync where colliders should be after rb update

  SwapEventBuffers(world);
  SolveCollisions(world, dt);

  CorrectEventTypes(world->collisionEvents, world->collisionEvents_prev);
  CorrectEventTypes(world->triggerEvents, world->triggerEvents_prev);
  
  SyncColliders(world); //Sync colliders to impulse position
  //////////////////////////////////////////////////////////////////////
}
  
void ResetPerFrame(PhysicsWorld* world){
  world->movedRigidBodies.count = 0;  
  world->collidersToSolve.count = 0;
  world->collisionPairs.count = 0; 
  world->removedRigidBodies.count = 0;
}
