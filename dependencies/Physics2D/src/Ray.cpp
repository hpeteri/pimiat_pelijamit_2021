#include "Ray.h"

inline u64 GetItself(u64 id){return id;};

RayCastHit CastRay(PhysicsWorld* world, MATH::Vector2<f32> start, f32 rotation, u64 ignore_rigidBody_id, bool ignoreTriggers, Array<MATH::Vector2<f32>>* dst){
  RayCastHit hit = {0};

  
  f32 shortestDistance = MATH::Inf;
  Line ray;
  Line originalRay;
  MATH::Vector2<f32> direction = {MATH::Cos(rotation), MATH::Sin(rotation)};

  originalRay.p0 = start;
  originalRay.p1 = start + direction * 10000;
    
  ray.p0 = start;
  ray.p1.x = start.x;
  ray.p1.y = start.y;
  
  Array<u64> testedColliders;
  testedColliders.Init(world->Allocate, world->Free);
  
  bool isVertical = direction.x == 0;
  f32 slope_x = direction.y / direction.x;
  bool isHorizontal = direction.y == 0;
  f32 slope_y = direction.x / direction.y;
  for(;;){
    dst->Add(ray.p0);
        
    ray.p0 = ray.p1;
    auto partition = world->quadTree.GetPartitionForPoint(ray.p0 + direction);
    MATH::Vector4<f32> aabb;
    aabb.x = partition->min.x;
    aabb.y = partition->min.y;
    aabb.z = partition->max.x;
    aabb.w = partition->max.y;
    
    if(isVertical){
      if(direction.y > 0)
        ray.p1.y = aabb.w;
      else
        ray.p1.y = aabb.y;
    }else{
      MATH::Vector2<f32> intersection0 = ray.p1;
      if(direction.x > 0){
        intersection0.x = aabb.z;
      }else{
        intersection0.x = aabb.x;
      }
      intersection0.y += (intersection0.x - ray.p0.x) * slope_x;

      if(isHorizontal){
        ray.p1 = intersection0;
      }else{
        MATH::Vector2<f32> intersection1 = ray.p1;
        if(direction.y > 0){
          intersection1.y = aabb.w;
        }else{
          intersection1.y = aabb.y;
        }
        intersection1.x += (intersection1.y - ray.p0.y) * slope_y;
        
        if(MATH::Length(intersection0 - ray.p0) <
           MATH::Length(intersection1 - ray.p0)){
          ray.p1 = intersection0;
        }else{
          ray.p1 = intersection1;
        }
           
      }
    }    
    if(MATH::Length(ray.p1 - ray.p0) == 0.0f){
      dst->Add(ray.p1);
      return hit;
    }
    for(u32 i = 0; i < partition->container.count; i++){
      const u64 collider_id = partition->container.data[i].element;

      //Check if collider has been already checked
      if(testedColliders.Search_Binary(collider_id, GetItself))
        continue;
      
      testedColliders.InsertAndKeepOrder(testedColliders.GetIndexToInsertAt(collider_id, GetItself),
                                         collider_id);

     
      auto boxCollider = GetBoxColliderWithID(world, collider_id);

      if(boxCollider){
        if(ignoreTriggers && boxCollider->flags & IS_TRIGGER)
          continue;
      
        if(!ignore_rigidBody_id || boxCollider->rigidBody_id != ignore_rigidBody_id){
          { //Bounding radius check
            auto c = boxCollider->collider.p0 + boxCollider->position;
            f32 len_a = MATH::DotProduct(c - start, direction);
            MATH::Vector2<f32> projection = start + direction * len_a;
            
            if(MATH::Length(projection - c) >= boxCollider->collider.r){
              continue;
            }
          }

          //check intersection with every edge
          MATH::Vector2<f32> points[4];
          GetGlobalPoints(boxCollider->collider,
                          points,
                          boxCollider->position,
                          boxCollider->rotation);

          for(u32 ii = 0; ii < 4; ii++){
            Line line;
            line.p0 = points[ii];
            line.p1 = points[(ii + 1) % 4];
          
            auto intersection = GetLineSegmentIntersection(originalRay, line);
            auto distance = MATH::Length(intersection - start);
            if(distance < shortestDistance){
              shortestDistance = distance;
              hit.p0 = intersection;
              hit.rigidBody_id = boxCollider->rigidBody_id;
              hit.isHit = true;
            }
          }
        }
        continue;
      }
      auto circleCollider = GetCircleColliderWithID(world, collider_id);
      if(circleCollider){
        if(ignoreTriggers && circleCollider->flags & IS_TRIGGER)
          continue;
        if(!ignore_rigidBody_id || circleCollider->rigidBody_id != ignore_rigidBody_id){

          MATH::Vector2<f32> c = circleCollider->position + circleCollider->collider.p0;
          
          f32 len_a = MATH::DotProduct(c - start, direction);
          MATH::Vector2<f32> projection = start + direction * len_a;
          
          if(len_a >= 0 && MATH::Length(c - projection) <= circleCollider->collider.r){
            f32 len_e = MATH::Length(c - start);
            f32 b_pow2 = (len_e * len_e) - (len_a * len_a);
            
            f32 t = MATH::Sqrt(MATH::Pow2(circleCollider->collider.r) - b_pow2); 

            MATH::Vector2<f32> intersection = start + direction * (len_a - t);
            auto distance = MATH::Length(intersection);
            if(distance < shortestDistance){
              shortestDistance = distance;
              hit.isHit = true;
              hit.p0 = intersection;
              hit.rigidBody_id = circleCollider->rigidBody_id;
            }
          }
        }
        continue;
      }
    }// end of i < partition.container.count
    if(hit.isHit) break;
  }

  testedColliders.Destroy();
  return hit;
}
