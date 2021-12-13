//Included in PhysicsWorld.cpp
//////////////////////////////////////////////////////////////////////////////////////////
void CreateForceGenerator(PhysicsWorld* world, f32 dir_x, f32 dir_y, f32 force){
  world->forceGenerators.Add(ForceGenerator{dir_x, dir_y, force});
}
//////////////////////////////////////////////////////////////////////////////////////////
u64 CreateRigidBody(PhysicsWorld* world, f32 mass, f32 rotation, f32 x, f32 y){
  RigidBody rb = {0};

  rb.id = ++world->runningRigidBody_id;

  rb.mass = MATH::Clamp(mass, 0, MATH::Inf);
  rb.system.mass = rb.mass;
  rb.position.x = x;
  rb.position.y = y;
  rb.rotation = rotation;
  SetInertia_Rectangle(&rb, 50, 50);
  
  auto it = world->rigidBodies.Add(rb);
  
  world->flags = world->flags & ~IS_RIGID_BODIES_SORTED;
  
  SetOldTransform(it);

  //Colliders attached need this to be able to find this rigidBody
  SetRigidBodyToBeSynced(world, it->id); 
  return rb.id;
}
//////////////////////////////////////////////////////////////////////////////////////////
u64 CreateCollider(PhysicsWorld* world, u64 rigidBody_id, Circle shape, COLLIDER_FLAGS flags){
  CircleCollider collider;
  collider.id = ++world->runningCollider_id;
  collider.rigidBody_id = rigidBody_id;
  collider.collider = shape;
  collider.type = CIRCLE_COLLIDER;
  collider.flags = flags;
  
  auto it = world->circleColliders.Add(collider);

  world->flags = world->flags & ~IS_CIRCLE_COLLIDERS_SORTED;
  
  //Attached colliders get synced when their attachent point moves,
  //so if they are not attached, we have to do it here
  if(!rigidBody_id){
    SyncCollider(world, it, nullptr);
  }
  return it->id;
}
u64 CreateCollider(PhysicsWorld* world, u64 rigidBody_id, RectangleShape shape, COLLIDER_FLAGS flags){
  BoxCollider collider;
  collider.id = ++world->runningCollider_id;
  collider.rigidBody_id = rigidBody_id;
  collider.collider = AsOriented(shape, 0);
  collider.collider.r = MATH::Length(collider.collider.size.x, collider.collider.size.y) / 2;
  collider.type = BOX_COLLIDER;
  collider.flags = flags;
  
  auto it = world->boxColliders.Add(collider);
  
  world->flags = world->flags & ~IS_BOX_COLLIDERS_SORTED; 
  
  //Attached colliders get synced when their attachent point moves
  //so if they are not attached, we have to do it here
  if(!rigidBody_id){
    SyncCollider(world, it, nullptr);
  }
  return it->id;
}
u64 CreateCollider(PhysicsWorld* world, u64 rigidBody_id, OrientedRectangle shape, COLLIDER_FLAGS flags){
  BoxCollider collider;
  collider.id = ++world->runningCollider_id;
  collider.rigidBody_id = rigidBody_id;
  collider.collider = shape;
  collider.collider.r = MATH::Length(collider.collider.size.x, collider.collider.size.y) / 2;
  collider.type = BOX_COLLIDER;
  collider.flags = flags;
  
  auto it = world->boxColliders.Add(collider);
  
  world->flags = world->flags & ~IS_BOX_COLLIDERS_SORTED;

  //Attached colliders get synced when their attachent point moves
  //so if they are not attached, we have to do it here
  if(!rigidBody_id){
    SyncCollider(world, it, nullptr);
  }
  return it->id;
}
//////////////////////////////////////////////////////////////////////////////////////////
RigidBody* GetRigidBodyWithID(PhysicsWorld* world, u64 id){
  if(!id) return nullptr;
  SortRigidBodies(world);
  RigidBody rb;
  rb.id = id;
  
  auto it = world->rigidBodies.Search_Binary(rb, GetRigidBodyID);
  return it;
}
void SetInertiaForRigidBody_Circle(PhysicsWorld* world, u64 id, f32 r){
  SetInertia_Circle(GetRigidBodyWithID(world, id), r);
}
void SetInertiaForRigidBody_Rectangle(PhysicsWorld* world, u64 id, f32 width, f32 height){
  SetInertia_Rectangle(GetRigidBodyWithID(world, id), width, height);
}

CircleCollider* GetCircleColliderWithID(PhysicsWorld* world, u64 id){
  if(!id) return nullptr;
  SortCircleColliders(world);

  CircleCollider collider;
  collider.id = id;
  return world->circleColliders.Search_Binary(collider, GetColliderID);
}
BoxCollider* GetBoxColliderWithID(PhysicsWorld* world, u64 id){
  if(!id) return nullptr;
  SortBoxColliders(world);
  
  BoxCollider collider;
  collider.id = id;
  return world->boxColliders.Search_Binary(collider, GetColliderID);
}
void RemoveCollider(PhysicsWorld* world, u64 id){
  auto boxCollider = GetBoxColliderWithID(world, id); 
  if(boxCollider){
    world->quadTree.Remove(boxCollider->id, boxCollider->aabb.min, boxCollider->aabb.max);
    world->boxColliders.Remove(boxCollider);
    world->flags = world->flags & ~IS_BOX_COLLIDERS_SORTED;
    return;
  }
  
  auto circleCollider = GetCircleColliderWithID(world, id); 
  if(circleCollider){
    world->quadTree.Remove(circleCollider->id, circleCollider->aabb.min, circleCollider->aabb.max);
    world->circleColliders.Remove(circleCollider);
    world->flags = world->flags & ~IS_CIRCLE_COLLIDERS_SORTED;
    return;
  }
}
bool RemoveRigidBody(PhysicsWorld* world, u64 id){
  auto rb = GetRigidBodyWithID(world, id);
  if(rb){
    world->rigidBodies.Remove(rb);
    world->flags = world->flags & ~IS_RIGID_BODIES_SORTED;
    world->removedRigidBodies.Add(id);
  }
  return rb;
}

//////////////////////////////////////////////////////////////////////////////////////////
