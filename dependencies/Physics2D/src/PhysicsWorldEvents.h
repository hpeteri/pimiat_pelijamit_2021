#ifndef PHYSICSWORLDEVENTS_H
#define PHYSICSWORLDEVENTS_H


enum PHYSICS_WORLD_EVENT_TYPE: u8{
  PHYSICS_WORLD_EVENT_ENTER = 0,
  PHYSICS_WORLD_EVENT_STAY = 1,
  PHYSICS_WORLD_EVENT_LEAVE = 2,
};
struct PhysicsWorldEvent{
  PHYSICS_WORLD_EVENT_TYPE type;
  u64 rigidBody_id_a;
  u64 rigidBody_id_b;
};
typedef PhysicsWorldEvent TriggerEvent;
typedef PhysicsWorldEvent CollisionEvent;

inline u64 GetEventRigidBodyA(PhysicsWorldEvent e){return e.rigidBody_id_a;}
inline u64 GetEventRigidBodyB(PhysicsWorldEvent e){return e.rigidBody_id_b;}
#endif
