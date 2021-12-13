#ifndef BUILDPART_H
#define BUILDPART_H

#include "Types.h"
#include "Math_Vector.h"
enum PART_TYPE : u8{
  PART_TYPE_ENGINE = 6,
    PART_TYPE_GUN,
    PART_TYPE_MASS,
    PART_TYPE_COUNT
    };
enum PART_BINDING : u8{
  PART_BINDING_W = 2,
    PART_BINDING_A,
    PART_BINDING_S,
    PART_BINDING_D,
    PART_BINDING_COUNT
};

struct BuildPartCard{
  

  PART_BINDING binding;
  PART_TYPE type;
  //card data
  MATH::Vector2<f32> position;
  f32 rotation;
  

  //Part data
  f32 mass;
  f32 force;

  f32 partRotation;
  
  bool isFlipped;
  bool isLocked;
};

BuildPartCard RollNewBuildPartCard(f32 x0, f32 y0, f32 x1, f32 y1);
#endif
