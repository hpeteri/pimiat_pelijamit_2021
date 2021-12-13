#include "Math_Basic.h"
#include <math.h>
namespace MATH{

  i32 Floor(f32 value){return (i32)value;}  
  i32 Round(f32 value){return (i32)(value + .5f);}

  f32 Clamp(f32 value, f32 min, f32 max){
    if(value < min) return min;
    if(value > max) return max;
    return value;
  }
  //////////////////////////////////////////////////////////////////////
  f32 Sin(f32 rad){ return (f32)sin(rad); }
  f32 Cos(f32 rad){ return (f32)cos(rad); }
  f32 Tan(f32 rad){ return (f32)tan(rad); }
  f32 Cotan(f32 rad){ return 1.0f / Tan(rad);}
  f32 Atan2(f32 y, f32 x){ return atan2(y, x);}
  //////////////////////////////////////////////////////////////////////
  f32 Pow2(f32 x){ return x * x; }
  f32 Sqrt(f32 x){ return (f32)sqrt(x);}
  //////////////////////////////////////////////////////////////////////
  f32 Fmod(f32 number, f32 denom){return (f32)fmod(number,denom);}
  //////////////////////////////////////////////////////////////////////
  i32 Next2Pow(i32 x){
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x += 1;
    return x;
  }  
  //////////////////////////////////////////////////////////////////////
  f32 Length(f32 x, f32 y){
    return Sqrt(Pow2(x) + Pow2(y));
  }
}
