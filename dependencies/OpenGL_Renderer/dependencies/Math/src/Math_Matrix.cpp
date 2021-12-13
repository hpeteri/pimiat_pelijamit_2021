#include "Math_Basic.h"
#include "Math_Matrix.h"
#include "Math_Vector.h"

namespace MATH{
  Matrix4x4 Matrix4x4::operator* (Matrix4x4 mat){
    Matrix4x4 result = {0};

    for(u8 c = 0; c < 4; c++){
      for(u8 r = 0; r < 4; r++){
        for(u8 i = 0; i < 4; i++){
          result[c][r] += (*this)[i][r] * mat[c][i];
        }
      }
    }
    
    return result;
  }
  Vector4<f32> Matrix4x4::operator*(Vector4<f32> B){
    Vector4<f32> result = {A[0].x * B.x + A[1].x * B.y + A[2].x * B.z + A[3].x * B.w,
      A[0].y * B.x + A[1].y * B.y + A[2].y * B.z + A[3].y * B.w,
      A[0].z * B.x + A[1].z * B.y + A[2].z * B.z + A[3].z * B.w,
      A[0].w * B.x + A[1].w * B.y + A[2].w * B.z + A[3].w * B.w};
    
    return result;
  }                             
  bool Matrix4x4::operator==(Matrix4x4 mat){
    return !memcmp(this, mat.A, sizeof(mat));
  }
  bool Matrix4x4::operator!=(Matrix4x4 mat){
    return memcmp(this, mat.A, sizeof(mat));
  }
  
  Matrix4x4 Identity(){
    return Matrix4x4{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
  }
  
  Matrix4x4 Transpose(Matrix4x4 A){
    return Matrix4x4{A[0].x, A[1].x, A[2].x, A[3].x,
        A[0].y, A[1].y, A[2].y, A[3].y,
        A[0].z, A[1].z, A[2].z, A[3].z,
        A[0].w, A[1].w, A[2].w, A[3].w};
  }

  Matrix4x4 Translate(f32 x, f32 y, f32 z){
    return Matrix4x4{1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        x, y, z, 1.0f};
  }

  Matrix4x4 Scale(f32 x, f32 y, f32 z){
    return Matrix4x4{x,0.0f,0.0f, 0.0f,
        0.0f, y,0.0f, 0.0f,
        0.0f, 0.0f, z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f};
  }
  Matrix4x4 RotateX(f32 rad){
    return Matrix4x4{1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, Cos(rad), Sin(rad), 0.0f,
        0.0f, -1.0f *Sin(rad),Cos(rad), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f};
  }

  Matrix4x4 RotateY(f32 rad){
    return Matrix4x4{Cos(rad), 0.0f, -1.0f *Sin(rad), 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        -1.0f*Sin(rad), 0.0f,Cos(rad), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f};
  }

  Matrix4x4 RotateZ(f32 rad){
    return Matrix4x4{Cos(rad), Sin(rad), 0.0f, 0.0f,
        -1.0f * Sin(rad), Cos(rad), 0.0f, 0.0f,
        0.0f,  0.0f,1.0f, 0.0f,
        0.0f,  0.0f, 0.0f, 1.0f};
  }

  Matrix4x4 Ortho(f32 left, f32 right, f32 top, f32 bottom, f32 ne, f32 fa){
     return Matrix4x4{ 2.0f/(right - left), 0.0f, 0.0f, 0.0f,
                       0.0f, 2.0f/(top - bottom), 0.0f, 0.0f,
         0.0f, 0.0f, 2.0f / (fa - ne), 0.0f,
                       (-(right + left) / (right - left)), (-(top + bottom) / (top - bottom)), (-(fa + ne) / (fa - ne)), 1.0f};
    
  }
  Matrix4x4 Perspective(f32 fov, f32 aspect,  f32 ne, f32 fa){
    //aspect is x / y
    f32 f = Cotan(0.5f * (fov / 180.0f * PI));

    return {f / aspect, 0, 0, 0,
            0, f, 0, 0,
            0, 0, -(fa + ne) / (ne - fa), 1,
            0, 0, (2 * fa * ne) / (ne - fa), 0};
  };
  Matrix4x4 LookAt(Vector3<f32> position, Vector3<f32> target, Vector3<f32> up){
    Vector3<f32> d = Normalize(target - position);
    Vector3<f32> r = CrossProduct(up, d);
    Vector3<f32> u = CrossProduct(d, r);

    Matrix4x4 view =  {r.x, u.x, d.x, 0,
                       r.y, u.y, d.y, 0,
                       r.z, u.z, d.z, 0,
                       DotProduct(r, position * -1),DotProduct(u, position* -1),DotProduct(d, position * -1), 1};

    return view;
    return view * Matrix4x4{1,0,0, 0,
        0,1,0,0,
        0,0,1,0,
        -position.x, -position.y, -position.z, 1};
  }
  Matrix4x4 Inverse(Matrix4x4 m, bool* defined){
    Matrix4x4 inv;

    inv.A2[0] = m.A2[5]  * m.A2[10] * m.A2[15] - 
      m.A2[5]  * m.A2[11] * m.A2[14] - 
      m.A2[9]  * m.A2[6]  * m.A2[15] + 
      m.A2[9]  * m.A2[7]  * m.A2[14] +
      m.A2[13] * m.A2[6]  * m.A2[11] - 
      m.A2[13] * m.A2[7]  * m.A2[10];

    inv.A2[4] = -m.A2[4]  * m.A2[10] * m.A2[15] + 
      m.A2[4]  * m.A2[11] * m.A2[14] + 
      m.A2[8]  * m.A2[6]  * m.A2[15] - 
      m.A2[8]  * m.A2[7]  * m.A2[14] - 
      m.A2[12] * m.A2[6]  * m.A2[11] + 
      m.A2[12] * m.A2[7]  * m.A2[10];

    inv.A2[8] = m.A2[4]  * m.A2[9] * m.A2[15] - 
      m.A2[4]  * m.A2[11] * m.A2[13] - 
      m.A2[8]  * m.A2[5] * m.A2[15] + 
      m.A2[8]  * m.A2[7] * m.A2[13] + 
      m.A2[12] * m.A2[5] * m.A2[11] - 
      m.A2[12] * m.A2[7] * m.A2[9];

    inv.A2[12] = -m.A2[4]  * m.A2[9] * m.A2[14] + 
      m.A2[4]  * m.A2[10] * m.A2[13] +
      m.A2[8]  * m.A2[5] * m.A2[14] - 
      m.A2[8]  * m.A2[6] * m.A2[13] - 
      m.A2[12] * m.A2[5] * m.A2[10] + 
      m.A2[12] * m.A2[6] * m.A2[9];

    inv.A2[1] = -m.A2[1]  * m.A2[10] * m.A2[15] + 
      m.A2[1]  * m.A2[11] * m.A2[14] + 
      m.A2[9]  * m.A2[2] * m.A2[15] - 
      m.A2[9]  * m.A2[3] * m.A2[14] - 
      m.A2[13] * m.A2[2] * m.A2[11] + 
      m.A2[13] * m.A2[3] * m.A2[10];

    inv.A2[5] = m.A2[0]  * m.A2[10] * m.A2[15] - 
      m.A2[0]  * m.A2[11] * m.A2[14] - 
      m.A2[8]  * m.A2[2] * m.A2[15] + 
      m.A2[8]  * m.A2[3] * m.A2[14] + 
      m.A2[12] * m.A2[2] * m.A2[11] - 
      m.A2[12] * m.A2[3] * m.A2[10];

    inv.A2[9] = -m.A2[0]  * m.A2[9] * m.A2[15] + 
      m.A2[0]  * m.A2[11] * m.A2[13] + 
      m.A2[8]  * m.A2[1] * m.A2[15] - 
      m.A2[8]  * m.A2[3] * m.A2[13] - 
      m.A2[12] * m.A2[1] * m.A2[11] + 
      m.A2[12] * m.A2[3] * m.A2[9];

    inv.A2[13] = m.A2[0]  * m.A2[9] * m.A2[14] - 
      m.A2[0]  * m.A2[10] * m.A2[13] - 
      m.A2[8]  * m.A2[1] * m.A2[14] + 
      m.A2[8]  * m.A2[2] * m.A2[13] + 
      m.A2[12] * m.A2[1] * m.A2[10] - 
      m.A2[12] * m.A2[2] * m.A2[9];

    inv.A2[2] = m.A2[1]  * m.A2[6] * m.A2[15] - 
      m.A2[1]  * m.A2[7] * m.A2[14] - 
      m.A2[5]  * m.A2[2] * m.A2[15] + 
      m.A2[5]  * m.A2[3] * m.A2[14] + 
      m.A2[13] * m.A2[2] * m.A2[7] - 
      m.A2[13] * m.A2[3] * m.A2[6];

    inv.A2[6] = -m.A2[0]  * m.A2[6] * m.A2[15] + 
      m.A2[0]  * m.A2[7] * m.A2[14] + 
      m.A2[4]  * m.A2[2] * m.A2[15] - 
      m.A2[4]  * m.A2[3] * m.A2[14] - 
      m.A2[12] * m.A2[2] * m.A2[7] + 
      m.A2[12] * m.A2[3] * m.A2[6];

    inv.A2[10] = m.A2[0]  * m.A2[5] * m.A2[15] - 
      m.A2[0]  * m.A2[7] * m.A2[13] - 
      m.A2[4]  * m.A2[1] * m.A2[15] + 
      m.A2[4]  * m.A2[3] * m.A2[13] + 
      m.A2[12] * m.A2[1] * m.A2[7] - 
      m.A2[12] * m.A2[3] * m.A2[5];

    inv.A2[14] = -m.A2[0]  * m.A2[5] * m.A2[14] + 
      m.A2[0]  * m.A2[6] * m.A2[13] + 
      m.A2[4]  * m.A2[1] * m.A2[14] - 
      m.A2[4]  * m.A2[2] * m.A2[13] - 
      m.A2[12] * m.A2[1] * m.A2[6] + 
      m.A2[12] * m.A2[2] * m.A2[5];

    inv.A2[3] = -m.A2[1] * m.A2[6] * m.A2[11] + 
      m.A2[1] * m.A2[7] * m.A2[10] + 
      m.A2[5] * m.A2[2] * m.A2[11] - 
      m.A2[5] * m.A2[3] * m.A2[10] - 
      m.A2[9] * m.A2[2] * m.A2[7] + 
      m.A2[9] * m.A2[3] * m.A2[6];

    inv.A2[7] = m.A2[0] * m.A2[6] * m.A2[11] - 
      m.A2[0] * m.A2[7] * m.A2[10] - 
      m.A2[4] * m.A2[2] * m.A2[11] + 
      m.A2[4] * m.A2[3] * m.A2[10] + 
      m.A2[8] * m.A2[2] * m.A2[7] - 
      m.A2[8] * m.A2[3] * m.A2[6];

    inv.A2[11] = -m.A2[0] * m.A2[5] * m.A2[11] + 
      m.A2[0] * m.A2[7] * m.A2[9] + 
      m.A2[4] * m.A2[1] * m.A2[11] - 
      m.A2[4] * m.A2[3] * m.A2[9] - 
      m.A2[8] * m.A2[1] * m.A2[7] + 
      m.A2[8] * m.A2[3] * m.A2[5];

    inv.A2[15] = m.A2[0] * m.A2[5] * m.A2[10] - 
      m.A2[0] * m.A2[6] * m.A2[9] - 
      m.A2[4] * m.A2[1] * m.A2[10] + 
      m.A2[4] * m.A2[2] * m.A2[9] + 
      m.A2[8] * m.A2[1] * m.A2[6] - 
      m.A2[8] * m.A2[2] * m.A2[5];

    f32 det = m.A2[0] * inv.A2[0] + m.A2[1] * inv.A2[4] + m.A2[2] * inv.A2[8] + m.A2[3] * inv.A2[12];

    if(det == 0)
      *defined = false;
    
    det = 1.0f / det;

    for(u32 i = 0; i < 16; i++)
      inv.A2[i] *= det;

    *defined = true;
    return inv;
  }
}; 
