#include "LinkedList.h"

template <typename T>
void LinkedList<T>::Init(void* (*Allocate0)(size_t), void (*Free0)(void*)){
  this->Allocate = Allocate0;
  this->Free = Free0;
  
  this->count = 0;
}

template <typename T>
void LinkedList<T>::Add(T value){
  ListNode<T>* node = (ListNode<T>*)this->Allocate(sizeof(ListNode<T>));
  node->next = nullptr;
  if(!this->end){
    this->end = node;
    node->prev = nullptr;
    this->start = node;
  }else{
    this->end->next = node;
    node->prev = this->end;
    this->end = node;
  }
  node->data = value;
  this->count++;
}

template <typename T>
void LinkedList<T>::RemoveLink(ListNode<T>* node){
  if(!node->prev){
    this->start = node->next;
  }else{ 
    node->prev->next = node->next;
  }

  if(!node->next){
    this->end = node->prev;
  }else{ 
    node->next->prev = node->prev;
  }
  this->count--;
}

template <typename T>
void LinkedList<T>::AddLink(ListNode<T>* node){
  node->next = nullptr;
  if(!this->end){
    this->end = node;
    node->prev = nullptr;
    this->start = node;
  }else{
    this->end->next = node;
    node->prev = this->end;
    this->end = node;
  }
  this->count++;
}
