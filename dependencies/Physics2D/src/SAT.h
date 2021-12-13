#ifndef SAT_H
#define SAT_H
#include "Math_Vector.h"
#include "Geometry.h"

MATH::Vector2<f32> GetSeparatingAxis(OrientedRectangle a, OrientedRectangle b);
MATH::Vector2<f32> GetSeparatingAxis(OrientedRectangle a, Circle b);

#endif
