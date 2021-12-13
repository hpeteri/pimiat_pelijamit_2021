#ifndef ARRAY_H
#define ARRAY_H

#include <iostream>


template <typename T>
struct Array{
  void* (*Allocate)(size_t);
  void (*Free)(void*);

  unsigned int count = 0;
  unsigned int size = 0;
  T* data = nullptr;
    
  inline T* operator[] (unsigned int i){return (i <  this->size) ? &data[i] : nullptr; }
  inline T* End(){return (this->count) ? &data[this->count - 1] : nullptr;} 

  void Init(void* (*Allocate)(size_t), void (*Free)(void*));
  void Destroy();
  void Resize(unsigned int size);
  void Expand();

  T* Add(T* element);
  T* Add(T element);

  T* Insert(int index, T* element);
  T* Insert(int index, T element);
  
  void Remove(int index);
  void Remove(T* element);
    
  T* InsertAndKeepOrder(int index, T element);
  T* InsertAndKeepOrder(int index, T* element);
      
  void RemoveAndKeepOrder(int index);
    
  int GetIndexOf(T* element);
  int GetIndexToInsertAt(T element, u64 (*GetComparator)(T));

  T Pop();

  void Sort_Radix256(u64 (*GetComparator)(T));
  T*   Search_Binary(T element, u64 (*GetComparator)(T));
  T*   Search_Binary(T element, u64 (*GetComparator)(T), i32 start, i32 end);

  T*   Search_Linear(T element, u64 (*GetComparator)(T));
};
#include "Array.cpp"

#endif
