template <typename T>
f32 Length(Vector3<T> vec3){return Sqrt(Pow2(vec3.x) + Pow2(vec3.y) + Pow2(vec3.z));}

template <typename T>
f32 Length(Vector2<T> vec2){return Sqrt(Pow2(vec2.x) + Pow2(vec2.y));}

template <typename T>
Vector3<T> Normalize(Vector3<T> vec3){
  f32 l = Length(vec3);
  return {vec3.x / l, vec3.y / l, vec3.z / l};
}  
template <typename T>
Vector2<T> Normalize(Vector2<T> vec2){
  f32 l = Length(vec2);
  return {vec2.x / l, vec2.y / l};
}  
template <typename T>
Vector2<T> Normal(Vector2<T> vec2){
  return {vec2.y, -vec2.x};
}
template <typename T>
T CrossProduct2D(Vector2<T> v, Vector2<T> w){
  return (v.x * w.y) - (w.x * v.y);
}
template <typename T>
Vector3<T> CrossProduct(Vector3<T> v, Vector3<T> w){
  return {(v.y * w.z) - (w.y * v.z) ,(v.z * w.x) - (w.z * v.x), (v.x * w.y) - (w.x * v.y)};
}

