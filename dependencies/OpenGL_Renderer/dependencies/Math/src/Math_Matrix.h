#ifndef MATH_MATRIX_H
#define MATH_MATRIX_H

#include "Math_Vector.h"

namespace MATH{
  
  struct Matrix4x4{
    union{
      struct{
        Vector4<f32> col0;
        Vector4<f32> col1;
        Vector4<f32> col2;
        Vector4<f32> col3;
      };
      Vector4<f32> A[4];
      f32 A2[4 * 4];
    };
    inline Vector4<f32>& operator[](i32 i){return A[i];}
    Matrix4x4 operator* (Matrix4x4 mat);
    Vector4<f32> operator*(Vector4<f32> B);
    bool operator==(Matrix4x4 mat);
    bool operator!=(Matrix4x4 mat);
  };
    
  Matrix4x4 Transpose(Matrix4x4 A);
  Matrix4x4 Identity();
  Matrix4x4 Translate(f32 x, f32 y, f32 z);
  Matrix4x4 Scale(f32 x, f32 y, f32 z);
  Matrix4x4 RotateX(f32 angle);
  Matrix4x4 RotateY(f32 angle);
  Matrix4x4 RotateZ(f32 angle);
  Matrix4x4 Ortho(f32 left, f32 right, f32 top, f32 bottom, f32 ne, f32 fa);
  Matrix4x4 Perspective(f32 fov, f32 aspect,  f32 ne, f32 fa);
  Matrix4x4 LookAt(Vector3<f32> position, Vector3<f32> target, Vector3<f32> up);
  Matrix4x4 Inverse(Matrix4x4 m, bool* defined);
 
};

#endif
