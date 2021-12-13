#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <iostream> //@REMOVE 
template <typename T>
struct HashTableNode{
  size_t len;
  const char* string;
  unsigned long long hash;
  T element;
};
  
template <typename T>
struct HashTable{
  unsigned int size;
  unsigned int count;
  unsigned int maxCount;

  unsigned int* usedIndices;
  HashTableNode<T>* table;

  void* (*Allocate)(size_t);
  void (*Free)(void*);
  unsigned long long (*HashFunction)(const char*);
  
  T* operator[](unsigned int i);
  
  void Init(void* (*Allocate)(size_t),
            void (*Free)(void*),
            unsigned long long(*HashFunction0)(const char* string));
  T* Add(const char* string, T element);
  T* Find(const char* string);
  const unsigned int* GetElementIndices();
  T* GetElementAtIndex(unsigned int);
  
};

#include "HashTable.cpp"

#endif
