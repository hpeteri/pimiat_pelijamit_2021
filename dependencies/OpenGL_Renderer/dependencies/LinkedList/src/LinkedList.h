#ifndef LINKEDLIST_H
#define LINKEDLIST_H

template <typename T>
struct ListNode{
  T data;
  ListNode<T>* prev = nullptr;
  ListNode<T>* next = nullptr;
};

template <typename T>
struct LinkedList{
  void* (*Allocate)(size_t);
  void (*Free)(void*);

  unsigned int count = 0;
  ListNode<T>* start = nullptr;
  ListNode<T>* end = nullptr;
  
  
  void Init(void* (*Allocate)(size_t), void (*Free)(void*));
  void Add(T value);
  void RemoveLink(ListNode<T>* node);
  void AddLink(ListNode<T>* node);
};
#include "LinkedList.cpp"

#endif
