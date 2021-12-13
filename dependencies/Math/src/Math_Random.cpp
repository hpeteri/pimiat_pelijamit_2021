#include "Math_Random.h"
#include "Math_Basic.h"
namespace MATH{
#define _rmask 0x7fffffffU                                         
#define _m 1103515245U
#define _n 12345U

  static u32 _seed = 1;                                           

  void SetSeed(i32 seed){
    _seed = seed;
  }
  f32  Random(){
    _seed = (_seed * _m + _n) & _rmask;                                                     
    f32 f = _seed / (f32)_rmask;                                                     
    return f;
  }
  i32  RandomInRange_i32(i32 min, i32 max){                                        
    return MATH::Round(RandomInRange_f32(min, max));
  }
  f32  RandomInRange_f32(f32 min, f32 max){
    _seed = (_seed * _m + _n) & _rmask;                                                     
    f32 f = _seed / (f32)_rmask;                                                     
    
    f32 range = max - min;                                                             
    return (min + (range * f));    
  }
}
