#ifndef COLLIDER_H
#define COLLIDER_H

#include "Geometry.h"
#include "Math_Vector.h"
enum COLLIDER_TYPE : u8{
  BOX_COLLIDER,
  CIRCLE_COLLIDER,
};
enum COLLIDER_FLAGS : u8{
  IS_TRIGGER = 1,
  IS_COLLISION_CALLBACK = 2,
};
struct Collider{
  u64 id;
  u64 rigidBody_id;
  
  COLLIDER_TYPE type;
  COLLIDER_FLAGS flags;
  
  union{
    struct{
      MATH::Vector2<f32> min; 
      MATH::Vector2<f32> max;
    };
    MATH::Vector2<f32> minMax[2]; 
  }aabb;
  
  MATH::Vector2<f32> position = {0,0}; 
  f32 rotation = 0;                
  
  struct{
    bool hasMoved;
    u32 uniqueCollisions;
  }perFrame;
    
};

struct BoxCollider : public Collider{
  OrientedRectangle collider;
};
struct CircleCollider : public Collider{
  Circle collider;
};
inline u64 GetColliderID(BoxCollider collider){return collider.id;}
inline u64 GetColliderID(CircleCollider collider){return collider.id;}

bool CheckCollision_AABB(MATH::Vector2<f32> a_min, MATH::Vector2<f32> a_max,
                         MATH::Vector2<f32> b_min, MATH::Vector2<f32> b_max);

struct CollisionManifold{
  MATH::Vector2<f32> pointOfContact;
  MATH::Vector2<f32> normal;
  f32 depth;
};
CollisionManifold GetCollisionManifold(Collider* a, Collider* b);
#endif
