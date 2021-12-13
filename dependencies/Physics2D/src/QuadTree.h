#ifndef QUADTREE_H
#define QUADTREE_H

#include "Types.h"
#include "Array.h"
#include "Math_Vector.h"

template <typename T>
struct QuadTreeEntry{
  T element;
  MATH::Vector2<f32> min;
  MATH::Vector2<f32> max;
};
template <typename T>
struct QuadTreePartition{
  u32 id; //Temp
  void* parentTree;
  MATH::Vector2<f32> min;
  MATH::Vector2<f32> max;
  u32 depth;
  bool isPartitioned;
  Array<QuadTreeEntry<T>> container;
  //top left, top right, bottom left, bottom right;
  QuadTreePartition<T> *tl, *tr, *bl, *br; 

  void Insert(T element, MATH::Vector2<f32> min, MATH::Vector2<f32> max);  
  void Remove(T element, MATH::Vector2<f32> min, MATH::Vector2<f32> max);
  void GetUniqueElementsInOccupiedPartitions(MATH::Vector2<f32> min, MATH::Vector2<f32> max, Array<T>* dst);
  const QuadTreePartition<T>* GetPartitionForPoint(MATH::Vector2<f32> point);

};
template <typename T>
struct QuadTree{
  u32 idCounter;
  void* (*Allocate)(size_t);
  void (*Free)(void*);

  u32 maxDepth;
  u32 fillRate;
  
  MATH::Vector2<f32> min;
  MATH::Vector2<f32> max;
  
  QuadTreePartition<T>* root;
  Array<T> overflow; //Not inside the tree  

  void Init(MATH::Vector2<f32> min, MATH::Vector2<f32> max, u32 depth, void* (*Allocate)(size_t), void (*Free)(void*));
  QuadTreePartition<T>* CreatePartition(f32 min_x, f32 min_y, f32 max_x, f32 max_y, u32 depth);

  void Insert(T element, MATH::Vector2<f32> min, MATH::Vector2<f32> max);
  void Remove(T element, MATH::Vector2<f32> min, MATH::Vector2<f32> max);  

  void GetUniqueElementsInOccupiedPartitions(MATH::Vector2<f32> min, MATH::Vector2<f32> max, Array<T>* dst);

  
  const QuadTreePartition<T>* GetPartitionForPoint(MATH::Vector2<f32> point);
};

#include "QuadTree.cpp"

#endif
