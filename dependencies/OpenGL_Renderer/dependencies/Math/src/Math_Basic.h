#ifndef MATH_H
#define MATH_H

#include "Math_Types.h"

namespace MATH{
  const f32 PI = 3.14159265f;
  const i32 infibit = 0b01111111100000000000000000000000;

  const f32 Inf =  *(f32*)&infibit;

#define ABS(x) ((x < 0)? -(x) : (x))
#define MIN(x,y) ((x < y) ? x : y)
#define MAX(x,y) ((x < y) ? y : x)
  
  i32 Floor(f32 value);
  i32 Round(f32 value);

  f32 Clamp(f32 value, f32 min, f32 max);
  
  f32 Sin(f32 rad);
  f32 Cos(f32 rad);
  f32 Tan(f32 rad);
  f32 Cotan(f32 rad);
  f32 Atan2(f32 y, f32 x);

  f32 Pow2(f32 x);
  f32 Sqrt(f32 x);

  f32 Fmod(f32 number, f32 denom);
  
  i32 Next2Pow(i32 x);

  f32 Length(f32 x, f32 y);
};
#endif
