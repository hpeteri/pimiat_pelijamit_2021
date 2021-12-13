#ifndef MATH_RANDOM_H
#define MATH_RANDOM_H

#include "Math_Types.h"
namespace MATH{
  void SetSeed(i32 seed);
  f32  Random();
  i32  RandomInRange_i32(i32 min, i32 max);
  f32  RandomInRange_f32(f32 min, f32 max);     
}
#endif
