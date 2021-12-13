#include "Collider.h"
#include "SAT.h"

#include <cstdio>
bool CheckCollision_AABB(MATH::Vector2<f32> a_min, MATH::Vector2<f32> a_max,
                    MATH::Vector2<f32> b_min, MATH::Vector2<f32> b_max){
  return (a_min.x < b_max.x && a_max.x > b_min.x &&
          a_min.y < b_max.y && a_max.y > b_min.y);
}

CollisionManifold GetCollisionManifold(Collider* a, Collider* b){
  CollisionManifold result = {0, 0};

  if(a->type == CIRCLE_COLLIDER){
    if(b->type == CIRCLE_COLLIDER){
      Circle aa = ((CircleCollider*)a)->collider;
      aa.p0 = RotateZ(aa.p0, a->rotation);
      aa.p0 += a->position;
      
      Circle bb = ((CircleCollider*)b)->collider;
      bb.p0 = RotateZ(bb.p0, b->rotation);
      bb.p0 += b->position;
     
      auto dir = aa.p0 - bb.p0;
      auto len = MATH::Length(dir);
      if(len < aa.r + bb.r){
        result.pointOfContact = aa.p0 + dir / len * -aa.r;
    
        result.normal = dir / len;
        result.depth = aa.r + bb.r - len;
      }
    }else if(b->type == BOX_COLLIDER){
      Circle aa = ((CircleCollider*)a)->collider;
      aa.p0 = RotateZ(aa.p0, a->rotation);
      aa.p0 += a->position;
            
      
      OrientedRectangle bb = ((BoxCollider*)b)->collider;
      bb.p0 = RotateZ(bb.p0, b->rotation);
      bb.p0 += b->position;
      bb.rotation += b->rotation;
      
      
      auto sep = GetSeparatingAxis(bb, aa);
      auto len = MATH::Length(sep);
      if(!len)
        return result;

      result.normal = sep / len;
      result.depth = len;
      result.pointOfContact = aa.p0 + result.normal * (-aa.r) - sep;
    }
  }
  else if(a->type == BOX_COLLIDER){
    if(b->type == CIRCLE_COLLIDER){
      Circle bb = ((CircleCollider*)b)->collider;
      bb.p0 = RotateZ(bb.p0, b->rotation);
      bb.p0 += b->position;
      
      OrientedRectangle aa = ((BoxCollider*)a)->collider;
      
      aa.p0 = RotateZ(aa.p0, a->rotation);
      aa.p0 += a->position;
      aa.rotation += a->rotation;

      
      auto sep = GetSeparatingAxis(aa, bb);
      auto len = MATH::Length(sep);
      if(!len) 
        return result;
      
      result.normal = sep / -len;
      result.depth = len;
      result.pointOfContact = bb.p0 + result.normal * (bb.r);// + sep;
      
    }else if(b->type == BOX_COLLIDER){
      OrientedRectangle aa = ((BoxCollider*)a)->collider;
      OrientedRectangle bb = ((BoxCollider*)b)->collider;

      aa.p0 = RotateZ(aa.p0, a->rotation);
      aa.p0 += a->position;
      aa.rotation += a->rotation;

      bb.p0 = RotateZ(bb.p0, b->rotation);
      bb.p0 += b->position;
      bb.rotation += b->rotation;
      
      auto sep = GetSeparatingAxis(aa, bb) * -1;
      auto len = MATH::Length(sep);
      if(!len)
        return result;
      
      result.normal = sep / len;
      result.depth = len;
      
      //Find collision points
      MATH::Vector2<f32> points_a[4];
      MATH::Vector2<f32> points_b[4];
      GetGlobalPoints(aa, points_a);
      GetGlobalPoints(bb, points_b);

      MATH::Vector2<f32> min = {MATH::Inf, MATH::Inf};
      MATH::Vector2<f32> max = min * -1;
      for(u32 i = 0; i < 4; i++){
        Line l0;
        l0.p0 = points_a[i];
        l0.p1 = points_a[(i + 1) % 4];
        
        for(u32 y = 0; y < 4; y++){
          Line l1;
          l1.p0 = points_b[y];
          l1.p1 = points_b[(y + 1) % 4];

          MATH::Vector2<f32> intersection = GetLineSegmentIntersection(l0, l1);
          if(intersection.x == MATH::Inf){
            continue;
          }
          if(intersection.x < min.x){
            min = intersection;            
          }
          if(intersection.x == min.x &&
             intersection.y < min.y)
            min = intersection;

          
          if(intersection.x > max.x){
            max = intersection;            
          }
          
          if(intersection.x == max.x &&
             intersection.y > max.y)
            max = intersection;
        }
      }
      result.pointOfContact = min + (max - min) / 2;
      
    }else{
      printf("B type is :: %d ? \n", a->type);
    }
  }else{
    printf("A type is :: %d ? \n", a->type);
  }
  
  return result;
}
