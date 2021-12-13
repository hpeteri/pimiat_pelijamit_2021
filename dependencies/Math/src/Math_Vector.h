#ifndef MATH_VECTOR_H
#define MATH_VECTOR_H

#include "Math_Types.h"
#include "Math_Basic.h"

namespace MATH{
  
  template <typename T>
  struct Vector2{
    union{
      struct {
        T x,y;
      };
      T A[2];
    };
    inline T& operator[](i32 i){return A[i];}
    inline Vector2<T> operator+ (Vector2<T> v){ return {x + v.x, y + v.y};}
    inline Vector2<T> operator- (Vector2<T> v){ return {x - v.x, y - v.y};}
    inline void operator+= (Vector2<T> v){ x += v.x; y += v.y;}
    inline void operator-= (Vector2<T> v){ x -= v.x; y -= v.y;}
    inline Vector2<T> operator* (T i){return {x * i, y * i};}
    inline Vector2<T> operator/ (T i){return {x / i, y / i};}
    inline bool operator== (Vector2<T> v){return x == v.x && y != v.y;}
    inline bool operator!= (Vector2<T> v){return x != v.x || y != v.y;}
  };

  Vector2<f32> RotateZ(Vector2<f32> vec, f32 rad);
  f32 DotProduct(Vector2<f32> a, Vector2<f32> b);
  
  template <typename T>
  struct Vector3{
    union{
      struct {
        T x,y,z;
      };
      T A[3];
    };
    inline T& operator[](i32 i){return A[i];}
    inline Vector3<T> operator+ (Vector3<T> v){ return {x + v.x, y + v.y, z + v.z};}
    inline Vector3<T> operator- (Vector3<T> v){ return {x - v.x, y - v.y, z - v.z};}
    inline void operator+= (Vector3<T> v){ x += v.x; y += v.y; z += v.z;}
    inline void operator-= (Vector3<T> v){ x -= v.x; y -= v.y; z -= v.z;}
    inline Vector3<T> operator* (T i){return {x * i, y * i, z * i};}
  };
  f32 DotProduct(Vector3<f32> a, Vector3<f32> b);
  
  template <typename T>
  struct Vector4{
    union{
      struct {
        T x,y,z,w;
      };
      T A[4];
    };
    inline T& operator[](i32 i){return A[i];}
  };
  #include "Math_Vector_Template.cpp"


  
};
#endif
