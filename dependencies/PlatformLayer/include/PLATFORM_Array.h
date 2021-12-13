#pragma once
#include "PLATFORM_Types.h"
namespace PLATFORM{
  
  template <typename T>
    struct Array{
      u32 count = 0;
      u32 size = 0;
      T* data = nullptr;
      
      inline T* operator[] (u32 i){return (i <  this->count) ? &data[i] : nullptr; }
      inline T* End(){return (this->count) ? &data[this->count - 1] : nullptr;} 
      
      void ResizeAndFill(u32 size, void*(*Allocate)(size_t), void (*Free)(void*)); //Also sets count
      void Resize(u32 size, void*(*Allocate)(size_t), void (*Free)(void*));
      
      T* Add(T* element, void*(*Allocate)(size_t), void (*Free)(void*));
      T* Insert(u32 at, T* element, void*(*Allocate)(size_t), void (*Free)(void*));
      
      T* Add(T element, void*(*Allocate)(size_t), void (*Free)(void*));
      T* Insert(u32 at, T element, void* (*Alloacete)(size_t), void (*Free)(void*));
      
      void RemoveAndKeepOrder(u32 i);
      void Remove(u32 i);
    
      u32 GetIndexOfElement(T* element);
      T Pop();
    };
  template <typename T>
    void Array<T>::ResizeAndFill(u32 newSize, void*(*Allocate)(size_t), void (*Free)(void*)){
    if(this->size >= newSize) {
      this->count = newSize;
      return;
    }
    T* d = nullptr;
    d = (T*)Allocate(sizeof(T) * newSize);
  
    memcpy(d, this->data, this->count * sizeof(T));
    Free(this->data);
    this->count = newSize;
    this->size = newSize;
    this->data = d;  
  }
  template <typename T>
    void Array<T>::Resize(u32 newSize, void*(*Allocate)(size_t), void (*Free)(void*)){
    if(this->size >= newSize) return;
  
    T* d = nullptr;
    d = (T*)Allocate(sizeof(T) * newSize);
  
    memcpy(d, this->data, this->count * sizeof(T));
    Free(this->data);
    this->size = newSize;
    this->data = d;  
  }

  template <typename T>
    T* Array<T>::Add(T* element, void*(*Allocate)(size_t), void (*Free)(void*)){
    T* insert = nullptr;
    if(this->count && !this->size)
      this->size = this->count;
  
    if(this->count < this->size){ //Add to end
      insert = &this->data[this->count];
    }else if(!this->data){
      this->data = (T*)Allocate(sizeof(T));
      this->count = 0;
      this->size = 1;
      insert = this->data;
    }else{//Double this size
      T* d = nullptr;
      d = (T*)Allocate(sizeof(T) * this->size * 2);
      memcpy(d, this->data, this->size * sizeof(T));
    
      this->size *= 2;
      Free(this->data);
    
      this->data = d;
      insert = &this->data[this->count];
    }
  
    this->count ++;
    if(element)
      *insert = *element;
   
    return insert;
  }
  template <typename T>
    T* Array<T>::Insert(u32 at, T* element, void*(*Allocate)(size_t), void (*Free)(void*)){
    if(at > this->count) return nullptr;
  
    T* insert = nullptr;
  
    if(this->count < this->size){
      for(u32 i = this->count; i > at; i--)
        memcpy(&this->data[i], &this->data[i - 1], sizeof(T));
    
      insert = &this->data[at];
    }else if(!this->data){ //if no data, create array for 1 element
      this->data = (T*)Allocate(sizeof(T));
      this->count = 0;
      this->size = 1;
      insert = this->data;
    }else{//Double array size
      T* d = nullptr;
      d = (T*)Allocate(sizeof(T) * this->size * 2);

      memcpy(d, this->data, at * sizeof(T));

      memcpy((char*)d + ((at + 1) * sizeof(T)),
             (*this)[at],
             (this->count - at) * sizeof(T));
    
      this->size *= 2;
      Free(this->data);
    
      this->data = d;
      insert = &this->data[at];
    }
  
    this->count ++;

    if(element)
      *insert = *element;
   
    return insert;
  }
  template <typename T>
    T* Array<T>::Add(T element, void*(*Allocate)(size_t), void (*Free)(void*)){
    T* insert = nullptr;
  
    if(this->count && !this->size)
      this->size = this->count;
  
    if(this->count < this->size){ //Add to end
      insert = &this->data[this->count];
    }else if(!this->data){
      this->data = (T*)Allocate(sizeof(T));
      this->count = 0;
      this->size = 1;
      insert = this->data;
    }else{//Double this size
      T* d = nullptr;
      d = (T*)Allocate(sizeof(T) * this->size * 2);
      memcpy(d, this->data, this->size * sizeof(T));
    
      this->size *= 2;
      Free(this->data);
    
      this->data = d;
      insert = &this->data[this->count];
    }
  
    this->count ++;
    
    *insert = element;
   
    return insert;
  }
  template <typename T>
    T* Array<T>::Insert(u32 at, T element, void*(*Allocate)(size_t), void (*Free)(void*)){
    if(at > this->count) return nullptr;
  
    T* insert = nullptr;
    if(this->count < this->size){
      for(u32 i = this->count; i > at; i--)
        memcpy(&this->data[i], &this->data[i - 1], sizeof(T));
    
      insert = &this->data[at];
    }else if(!this->data){ //if no data, create array for 1 element
      this->data = (T*)Allocate(sizeof(T));
      this->count = 0;
      this->size = 1;
      insert = this->data;
    }else{//Double array size
      T* d = nullptr;
      d = (T*)Allocate(sizeof(T) * this->size * 2);

      memcpy(d, this->data,at * sizeof(T));

      memcpy((char*)d + ((at + 1) * sizeof(T)),
             (*this)[at],
             (this->count - at) * sizeof(T));
    
      this->size *= 2;
      Free(this->data);
    
      this->data = d;
      insert = &this->data[at];
    }
  
    this->count ++;
    
    *insert = element;
   
    return insert;
  }
  template <typename T>
    void Array<T>::Remove(u32 i){
    if(i >= this->count) return;
    if(this->count > 1)
      memcpy(&this->data[i], &this->data[this->count - 1], sizeof(T));
    this->count -= 1;
  }
  template <typename T>
    void Array<T>::RemoveAndKeepOrder(u32 i){
    if(i >= this->count) return;
    memcpy(&this->data[i], &this->data[i+1], (this->count - i - 1) * sizeof(T));
    this->count--;
  }
  template <typename T>
    u32 Array<T>::GetIndexOfElement(T* element){
    return (u32)(element - this->data);
  }
  template <typename T>
    T Array<T>::Pop(){
    if(!this->count) *(int*)nullptr = 0;
    auto element = *this->End();
    this->count--;
    return element;
  }
};
