#include "SAT.h"
#include <cstdio>
MATH::Vector2<f32> GetShortestSeparatingAxis(f32 min0, f32 max0, f32 min1, f32 max1, u32 i, f32 shortestDistance, u32 shortestDistanceIndex){
  auto delta = max1 - min0;
  auto other_delta = max0 - min1;
  
  if(other_delta < delta)
    delta = -other_delta;
  
  if(ABS(delta) < ABS(shortestDistance)){
    shortestDistance = delta;
    shortestDistanceIndex = i;
  }
  return {shortestDistance, (f32)shortestDistanceIndex};
}
MATH::Vector2<f32> GetNormal(MATH::Vector2<f32> p0 ,MATH::Vector2<f32>* points, u32 pointCount){
  MATH::Vector2<f32> closestPoint = points[0];
  f32 distance = MATH::Length(p0 - closestPoint);
  for(u32 i = 1; i < pointCount; i++){
    f32 d =  MATH::Length(p0 - points[i]);
    if(d < distance){
      closestPoint = points[i];
      distance = d;
    }
  }
  return (closestPoint - p0) / distance;
}
MATH::Vector2<f32> GetSeparatingAxis(OrientedRectangle a, Circle b){
  // if(a.rotation == 0 && b.rotation == 0.0f) return IsColliding_Rectangle_Rectangle(a, b);
  MATH::Vector2<f32> normals[3];
  MATH::Vector2<f32> aPoints[4];
  MATH::Vector2<f32> bPoints[1] = {b.p0};
  GetGlobalPoints(a, aPoints);

  
  normals[0] = {MATH::Cos(a.rotation), MATH::Sin(a.rotation)};
  normals[1] = {MATH::Cos(a.rotation + MATH::PI / 2), MATH::Sin(a.rotation + MATH::PI / 2)};
  normals[2] = GetNormal(b.p0, aPoints, 4);
  
  f32 shortestDistance = MATH::Inf;
  i32 shortestDistanceIndex = 0;
  f32 inverseMul = 1.0f;
  
  for(u32 i = 0; i < 3; i++){
    auto normal = normals[i];
        
    auto minMax0 = GetProjectionMinMax(aPoints, 4, normal);
    auto minMax1 = GetProjectionMinMax(bPoints, 1, normal);
    minMax1.x -= b.r;
    minMax1.y += b.r;
    
    //No overlap
    if(minMax0.y < minMax1.x || minMax1.y < minMax0.x){
      return {0, 0};
    }
    
    if(minMax0.y > minMax1.x){
      auto result = GetShortestSeparatingAxis(minMax1.x, minMax1.y, minMax0.x,  minMax0.y, i, shortestDistance, shortestDistanceIndex);
      shortestDistance = result.x;
      shortestDistanceIndex = (i32)result.y;
      inverseMul = -1;
    }else if(minMax1.y > minMax0.x){
      auto result = GetShortestSeparatingAxis(minMax0.x, minMax0.y, minMax1.x, minMax1.y, i, shortestDistance, shortestDistanceIndex);
      shortestDistance = result.x;
      shortestDistanceIndex = (i32)result.y;
      inverseMul = 1;
    }
  }
  f32 x = normals[shortestDistanceIndex].x;
  f32 y = normals[shortestDistanceIndex].y;
  
  if(x < 0 || (x == 0.0f && y < 0.0f)){
    x *= -1;
    y *= -1;
  }else
    shortestDistance *= -1;

  return MATH::Vector2<f32>{x * shortestDistance * inverseMul, y * shortestDistance * inverseMul};
}
MATH::Vector2<f32> GetSeparatingAxis(OrientedRectangle a, OrientedRectangle b){
  // if(a.rotation == 0 && b.rotation == 0.0f) return IsColliding_Rectangle_Rectangle(a, b);
  MATH::Vector2<f32> normals[4];
  normals[0] = {MATH::Cos(a.rotation), MATH::Sin(a.rotation)};
  normals[1] = {MATH::Cos(a.rotation + MATH::PI / 2), MATH::Sin(a.rotation + MATH::PI / 2)};
  normals[2] = {MATH::Cos(b.rotation), MATH::Sin(b.rotation)};
  normals[3] = {MATH::Cos(b.rotation + MATH::PI / 2), MATH::Sin(b.rotation + MATH::PI / 2)};

  MATH::Vector2<f32> aPoints[4];
  MATH::Vector2<f32> bPoints[4];
  GetGlobalPoints(a, aPoints);
  GetGlobalPoints(b, bPoints);

  f32 shortestDistance = MATH::Inf;
  i32 shortestDistanceIndex = 0;
  f32 inverseMul = 1.0f;
  
  for(u32 i = 0; i < 4; i++){
    auto normal = normals[i];
        
    auto minMax0 = GetProjectionMinMax(aPoints, 4, normal);
    auto minMax1 = GetProjectionMinMax(bPoints, 4, normal);

    //No overlap
    if(minMax0.y < minMax1.x || minMax1.y < minMax0.x){
      return {0, 0};
    }    
    if(minMax0.y > minMax1.x){
      auto result = GetShortestSeparatingAxis(minMax1.x, minMax1.y, minMax0.x,  minMax0.y, i, shortestDistance, shortestDistanceIndex);
      shortestDistance = result.x;
      shortestDistanceIndex = (i32)result.y;
      inverseMul = -1;
    }else if(minMax1.y > minMax0.x){
      auto result = GetShortestSeparatingAxis(minMax0.x, minMax0.y, minMax1.x, minMax1.y, i, shortestDistance, shortestDistanceIndex);
      shortestDistance = result.x;
      shortestDistanceIndex = (i32)result.y;
      inverseMul = 1;
    }
  }
  f32 x = normals[shortestDistanceIndex].x;
  f32 y = normals[shortestDistanceIndex].y;
  
  if(x < 0 || (x == 0.0f && y < 0.0f)){
    x *= -1;
    y *= -1;
  }else
    shortestDistance *= -1;

  return MATH::Vector2<f32>{x * shortestDistance * inverseMul, y * shortestDistance * inverseMul};
}
