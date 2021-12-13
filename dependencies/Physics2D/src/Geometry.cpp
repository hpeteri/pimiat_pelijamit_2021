#include "Geometry.h"
#include <iostream>
const f32 epsilonf32 = 0.000001f;
//////////////////////////////////////////////////////////////////////
static void GetLocalMinMaxPoints(Rectangle r, MATH::Vector2<f32>* points){
  points[0].x = r.size.x * -0.5f;
  points[1].x = r.size.x * 0.5f;
  points[0].y = r.size.y * -0.5f;
  points[1].y = r.size.y * 0.5f;
}
static void GetGlobalMinMaxPoints(Rectangle r, MATH::Vector2<f32>* points){
  points[0].x = r.p0.x - r.size.x * 0.5f;
  points[1].x = r.p0.x + r.size.x * 0.5f;
  points[0].y = r.p0.y - r.size.y * 0.5f;
  points[1].y = r.p0.y + r.size.y * 0.5f;
}

OrientedRectangle AsOriented(Rectangle rect, f32 rotation){
  OrientedRectangle result;
  result.p0 = rect.p0;
  result.size = rect.size;
  result.rotation = rotation;
  return result;
}

//////////////////////////////////////////////////////////////////////
void GetGlobalMinMaxPoints(Circle c, MATH::Vector2<f32>* points, MATH::Vector2<f32> offset, f32 offset_rotation){
  points[0] = points[1] = offset + MATH::RotateZ(c.p0, offset_rotation);
  points[0].x -= c.r;
  points[0].y -= c.r;
  points[1].x += c.r;
  points[1].y += c.r;

}
void GetGlobalMinMaxPoints(OrientedRectangle r, MATH::Vector2<f32>* points, MATH::Vector2<f32> offset, f32 offset_rotation){
  MATH::Vector2<f32> globalPoints[4];
  GetGlobalPoints(r, globalPoints, offset, offset_rotation);

  //Use min max to set global points
  points[0] = points[1] = globalPoints[0];
  for(auto p : globalPoints){
    points[0].x = MIN(points[0].x, p.x);
    points[0].y = MIN(points[0].y, p.y);
    
    points[1].x = MAX(points[1].x, p.x);
    points[1].y = MAX(points[1].y, p.y);
  }
}
//////////////////////////////////////////////////////////////////////
void GetGlobalPoints(Rectangle r, MATH::Vector2<f32>* points){
  points[0].x = r.size.x * -0.5f;
  points[0].y = r.size.y * -0.5f;
  
  points[2].x = r.size.x * 0.5f;
  points[2].y = r.size.y * 0.5f;

  points[1].x = points[0].x;
  points[1].y = points[2].y;
  
  points[3].x = points[2].x;
  points[3].y = points[0].y;

  for(u32 i = 0; i < 4; i++)
    points[i] += r.p0;
}
void GetGlobalPoints(Rectangle r, MATH::Vector2<f32>* points, MATH::Vector2<f32> offset){
  points[0].x = r.size.x * -0.5f;
  points[0].y = r.size.y * -0.5f;
  
  points[2].x = r.size.x * 0.5f;
  points[2].y = r.size.y * 0.5f;

  points[1].x = points[0].x;
  points[1].y = points[2].y;
  
  points[3].x = points[2].x;
  points[3].y = points[0].y;

  for(u32 i = 0; i < 4; i++)
    points[i] += r.p0 + offset;
}
void GetGlobalPoints(OrientedRectangle r, MATH::Vector2<f32>* points){
  if(r.rotation == 0.0f) return GetGlobalPoints((Rectangle)r, points);
  
  points[0].x = r.size.x * -0.5f;
  points[0].y = r.size.y * -0.5f;
  
  points[2].x = r.size.x * 0.5f;
  points[2].y = r.size.y * 0.5f;

  points[1].x = points[0].x;
  points[1].y = points[2].y;
  
  points[3].x = points[2].x;
  points[3].y = points[0].y;
  
  f32 c = MATH::Cos(r.rotation);
  f32 s = MATH::Sin(r.rotation);
  for(u32 i = 0; i < 4; i++){
    auto it = points[i];
    points[i].x = it.x * c + it.y * -s;
    points[i].y = it.x * s + it.y * c;
    points[i] += r.p0;
  } 
}
void GetGlobalPoints(OrientedRectangle r, MATH::Vector2<f32>* points, MATH::Vector2<f32> offset, f32 offset_rotation){
    
  points[0].x = r.size.x * -0.5f;
  points[0].y = r.size.y * -0.5f;
  
  points[2].x = r.size.x * 0.5f;
  points[2].y = r.size.y * 0.5f;

  points[1].x = points[0].x;
  points[1].y = points[2].y;
  
  points[3].x = points[2].x;
  points[3].y = points[0].y;
  
  f32 c0 = MATH::Cos(r.rotation);
  f32 s0 = MATH::Sin(r.rotation);

  f32 c1 = MATH::Cos(offset_rotation);
  f32 s1 = MATH::Sin(offset_rotation);
  
  for(u32 i = 0; i < 4; i++){
    auto it = points[i];
    points[i].x = it.x * c0 + it.y * -s0;
    points[i].y = it.x * s0 + it.y * c0;
  }
  for(u32 i = 0; i < 4; i++){
    auto it = points[i] + r.p0;
    
    points[i].x = it.x * c1 + it.y * -s1;
    points[i].y = it.x * s1 + it.y * c1;
    
    points[i] += offset;
  }
  

}
//////////////////////////////////////////////////////////////////////
bool IsPointInside(MATH::Vector2<f32> p0, Line l){
  f32 min_x = MIN(l.p0.x, l.p1.x);
  f32 max_x = MAX(l.p0.x, l.p1.x);
  f32 min_y = MIN(l.p0.y, l.p1.y);
  f32 max_y = MAX(l.p0.y, l.p1.y);

  if(!(p0.x >= min_x && p0.x <= max_x)) return false;
  if(!(p0.y >= min_y && p0.y <= max_y)) return false;

  auto a = l.p1 - l.p0;
  auto b = p0 - l.p0;  
  auto dot = a.x * b.y - a.y * b.x;
  
  return ABS(dot) <= epsilonf32;
}
bool IsPointInside(MATH::Vector2<f32> p0, Circle c){
  f32 distance = MATH::Length(p0 - c.p0);
  return distance <= c.r;
}
bool IsPointInside(MATH::Vector2<f32> p0, Rectangle r){
  
  f32 min_x = r.p0.x - r.size.x * 0.5f;
  f32 max_x = r.p0.x + r.size.x * 0.5f;
  f32 min_y = r.p0.y - r.size.y * 0.5f;
  f32 max_y = r.p0.y + r.size.y * 0.5f;

  if(!(p0.x >= min_x && p0.x <= max_x)) return false;
  if(!(p0.y >= min_y && p0.y <= max_y)) return false;
  return true;
}
bool IsPointInside(MATH::Vector2<f32> p0, OrientedRectangle r){
  if(r.rotation == 0.0f) return IsPointInside(p0, Rectangle(r));
  //translate point to rect local space
  p0 = p0 - r.p0;
  
  f32 min_x = r.size.x * -0.5f;
  f32 max_x = min_x + r.size.x;
  f32 min_y = r.size.y * -0.5f;
  f32 max_y = min_y + r.size.y;;
  
  f32 c = MATH::Cos(-r.rotation);
  f32 s = MATH::Sin(-r.rotation);

  f32 x = p0.x * c + p0.y * -s;
  f32 y = p0.x * s + p0.y * c;
  
  if(!(x >= min_x && x <= max_x)) return false;
  if(!(y >= min_y && y <= max_y)) return false;

  return true;
}
//////////////////////////////////////////////////////////////////////
MATH::Vector2<f32> GetLineSegmentIntersection(Line a, Line b){
  auto r = a.p1 - a.p0;
  auto s = b.p1 - b.p0;

  f32 rs = MATH::CrossProduct2D(r, s);
  if(rs == 0.0f){ 
    
  }
  f32 sr = -rs;

  f32 t = MATH::CrossProduct2D(b.p0 - a.p0, s) / rs;
  if(t > 1.0f || t < 0.0f) return {MATH::Inf, MATH::Inf};

  f32 u = MATH::CrossProduct2D(a.p0 - b.p0, r) / sr;
  if(u > 1.0f || u < 0.0f) return {MATH::Inf, MATH::Inf};
  
  return a.p0 + r * t;
}
MATH::Vector2<f32> GetLineIntersection(Line a, Line b){
  auto A1 = a.p1.y - a.p0.y;
  auto B1 = a.p0.x - a.p1.x;
  auto C1 = A1 * a.p0.x + B1 * a.p0.y;

  auto A2 = b.p1.y - b.p0.y;
  auto B2 = b.p0.x - b.p1.x;
  auto C2 = A2 * b.p0.x + B2 * b.p0.y;

  auto denominator = A1 * B2 - A2 * B1;

  f32 x = (B2 * C1 - B1 * C2) / denominator;
  f32 y = (A1 * C2 - A2 * C1) / denominator;
  
  return {x, y};  
}

//////////////////////////////////////////////////////////////////////
//Circle
bool IsColliding_Circle_Circle(Circle a, Circle b){
  f32 d = MATH::Length(b.p0 - a.p0);
  return d < a.r + b.r;
}
bool IsColliding_Circle_Rectangle(Circle a, Rectangle b){
  if(IsPointInside(a.p0, b)) return true;

  MATH::Vector2<f32> points[2];
  GetGlobalMinMaxPoints(b, points);
  
  MATH::Vector2<f32> closestPoint = a.p0;
  for(u32 i = 0; i < 2; i++){
    if(closestPoint[i] < points[0][i])
      closestPoint[i] = points[0][i];
    else if(closestPoint[i] > points[1][i])
      closestPoint[i] = points[1][i];
  }
  
  return MATH::Length(closestPoint - a.p0) < a.r;
};
bool IsColliding_Circle_OrientedRectangle(Circle a, OrientedRectangle b){
  if(IsPointInside(a.p0, b)) return true;
  if(b.rotation == 0.0f) return IsColliding_Circle_Rectangle(a, b);
  
  MATH::Vector2<f32> points[2];
  GetLocalMinMaxPoints(b, points);

  f32 c = MATH::Cos(-b.rotation);
  f32 s = MATH::Sin(-b.rotation);
  
  MATH::Vector2<f32> p0 = a.p0 - b.p0;
  f32 x = p0.x * c + p0.y * -s;
  f32 y = p0.x * s + p0.y * c;
  
  p0 = {x, y};

  MATH::Vector2<f32> closestPoint = p0;
  for(u32 i = 0; i < 2; i++){
    if(closestPoint[i] < points[0][i])
      closestPoint[i] = points[0][i];
    else if(closestPoint[i] > points[1][i])
      closestPoint[i] = points[1][i];
  }
  
  return MATH::Length(closestPoint - p0) < a.r;
}
//////////////////////////////////////////////////////////////////////
bool IsColliding_Rectangle_Rectangle(Rectangle a, Rectangle b){
  MATH::Vector2<f32> points0[2];
  MATH::Vector2<f32> points1[2];
  
  GetGlobalMinMaxPoints(a, points0);
  GetGlobalMinMaxPoints(b, points1);

  return (points0[0].x < points1[1].x && points0[1].x > points1[0].x &&
          points0[0].y < points1[1].y && points0[1].y > points1[0].y);
  
}
MATH::Vector2<f32> GetProjectionMinMax(MATH::Vector2<f32>* points, u32 pointCount, MATH::Vector2<f32>normal){
  f32 min0, max0;
  min0 = max0 = MATH::DotProduct(points[0], normal);  
  for(u32 i = 1; i < pointCount; i++){
    auto dot = MATH::DotProduct(points[i], normal);  
    max0 = MAX(max0, dot);
    min0 = MIN(min0, dot);
  }
  return {min0, max0};
}
/*
*/
bool IsColliding_Rectangle_OrientedRectangle(Rectangle a, OrientedRectangle b){
  if(b.rotation == 0.0f) return IsColliding_Rectangle_Rectangle(a, b);
  
  MATH::Vector2<f32> normals[4];
  normals[0] = {0, 1};
  normals[1] = {1, 0};
  normals[2] = {MATH::Cos(b.rotation), MATH::Sin(b.rotation)};
  normals[3] = {MATH::Cos(b.rotation + MATH::PI / 2), MATH::Sin(b.rotation + MATH::PI / 2)};

  MATH::Vector2<f32> aPoints[4];
  MATH::Vector2<f32> bPoints[4];
  GetGlobalPoints(a, aPoints);
  GetGlobalPoints(b, bPoints);

  for(auto point : aPoints)
    printf("a %f %f\n", point.x, point.y);

  for(auto point : bPoints)
    printf("b %f %f\n", point.x, point.y);
  
  //f32 shortestDistance = MATH::Inf;
  //i32 shortestDistanceIndex = 0;
  //f32 inverseMul = 1.0f;
  
  for(u32 i = 0; i < 4; i++){
    auto normal = normals[i];
        
    auto minMax0 = GetProjectionMinMax(aPoints, 4, normal);
    auto minMax1 = GetProjectionMinMax(bPoints, 4, normal);

    //No overlap
    if(minMax0.y < minMax1.x || minMax1.y < minMax0.x){
      printf("From normal %f %f\n", normal.x, normal.y);
      printf("%f - %f || %f - %f\n", minMax0.x, minMax0.y, minMax1.x, minMax1.y);
    
      return false;
    }
    printf("collision From normal %f %f\n", normal.x, normal.y);
    printf("%f - %f || %f - %f\n", minMax0.x, minMax0.y, minMax1.x, minMax1.y);
     
    /*
    
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
*/
  }
  return true;
  #if 0
  f32 x = normals[shortestDistanceIndex].x;
  f32 y = normals[shortestDistanceIndex].y;
  
  if(x < 0 || (x == 0.0f && y < 0.0f)){
    x *= -1;
    y *= -1;
  }else
    shortestDistance *= -1;

  auto it = MATH::Vector2<f32>{x * shortestDistance * inverseMul, y * shortestDistance * inverseMul};
  printf("%f %f\n", it.x, it.y);
  return true;
  #endif
}

bool IsColliding_Rectangle_Circle(Rectangle a, Circle b){
  return IsColliding_Circle_Rectangle(b, a);
}

