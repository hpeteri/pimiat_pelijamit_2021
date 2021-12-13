#include "Math_Vector.h"
namespace MATH{
  Vector2<f32> RotateZ(Vector2<f32> vec, f32 rad){
    f32 s = MATH::Sin(rad);
    f32 c = MATH::Cos(rad);

    Vector2<f32> result;
    result.x = vec.x * c  + vec.y * -s;
    result.y = vec.x * s + vec.y * c;
    return result;    
  }  
  f32 DotProduct(Vector2<f32> a, Vector2<f32> b){
    return a.x * b.x + a.y * b.y;
  }
  f32 DotProduct(Vector3<f32> a, Vector3<f32> b){
    return a.x * b.x + a.y * b.y + a.z * b.z;
  }
}

