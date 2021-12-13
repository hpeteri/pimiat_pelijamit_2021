#ifndef ROOM_H
#define ROOM_H

#include "Types.h"

#include "PhysicsWorld.h"
#include "Array.h"
#include "Math_Vector.h"
#include "Entity.h"

struct Room{
  PhysicsWorld world;
  MATH::Vector2<f32> world_offset;
  Array<Entity> entities;
};

struct Level{
  Array<Room> rooms;  
};

#endif
