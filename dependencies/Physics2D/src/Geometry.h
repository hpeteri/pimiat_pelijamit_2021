#ifndef GEOMETRY_H
#define GEOMETRY_H
#include "Types.h"
#include "Math_Vector.h"

struct Line{
  MATH::Vector2<f32> p0;
  MATH::Vector2<f32> p1;
};
struct Circle{
  MATH::Vector2<f32> p0;
  f32 r;
};
struct RectangleShape{
  MATH::Vector2<f32> p0; //center 
  MATH::Vector2<f32> size;
  f32 r;
};
struct OrientedRectangle : RectangleShape{
  f32 rotation;
};
OrientedRectangle AsOriented(RectangleShape rect,
                             f32 rotation);
//////////////////////////////////////////////////////////////////////
void GetGlobalMinMaxPoints(Circle c, MATH::Vector2<f32>* points, MATH::Vector2<f32> offset, f32 offset_rotation);
void GetGlobalMinMaxPoints(OrientedRectangle r, MATH::Vector2<f32>* points, MATH::Vector2<f32> offset, f32 offset_rotation);
//////////////////////////////////////////////////////////////////////
void GetGlobalPoints(RectangleShape r, MATH::Vector2<f32>* points);
void GetGlobalPoints(OrientedRectangle r, MATH::Vector2<f32>* points);
void GetGlobalPoints(RectangleShape r, MATH::Vector2<f32>* points, MATH::Vector2<f32> offset);
void GetGlobalPoints(OrientedRectangle r, MATH::Vector2<f32>* points, MATH::Vector2<f32> offset, f32 offset_rotation);
//////////////////////////////////////////////////////////////////////
//SAT uses this
MATH::Vector2<f32> GetProjectionMinMax(MATH::Vector2<f32>* points, u32 pointCount, MATH::Vector2<f32>normal);
//////////////////////////////////////////////////////////////////////
bool IsPointInside(MATH::Vector2<f32> p0, Line l);
bool IsPointInside(MATH::Vector2<f32> p0, Circle c);
bool IsPointInside(MATH::Vector2<f32> p0, RectangleShape r);
bool IsPointInside(MATH::Vector2<f32> p0, OrientedRectangle r);
//////////////////////////////////////////////////////////////////////
MATH::Vector2<f32> GetLineSegmentIntersection(Line a, Line b);
MATH::Vector2<f32> GetLineIntersection(Line a, Line b);

//////////////////////////////////////////////////////////////////////
bool IsColliding_Circle_Circle(Circle a, Circle b);
bool IsColliding_Circle_Rectangle(Circle a, RectangleShape b);
bool IsColliding_Circle_OrientedRectangle(Circle a, OrientedRectangle b);

bool IsColliding_Rectangle_Rectangle(RectangleShape a, RectangleShape b);
bool IsColliding_Rectangle_OrientedRectangle(RectangleShape a, OrientedRectangle b);
bool IsColliding_Rectangle_Circle(RectangleShape a, Circle b);
//////////////////////////////////////////////////////////////////////
#endif
