#include <cstring>
template <typename T>
void Array<T>::Init(void* (*Allocate0)(size_t), void (*Free0)(void*)){
  this->Allocate = Allocate0;
  this->Free = Free0;

  this->size = 0;
  this->count = 0;
  this->data = nullptr;
}
template <typename T>
void Array<T>::Destroy(){
  this->Free(this->data);
}
template <typename T>
void Array<T>::Resize(unsigned int newSize){
  if(this->size >= newSize) return;
  
  T* d = nullptr;
  d = (T*)this->Allocate(sizeof(T) * newSize);
  
  memcpy(d, this->data, this->count * sizeof(T));
  this->Free(this->data);
  this->size = newSize;
  this->data = d;  
}

template <typename T>
void Array<T>::Expand(){ 
  unsigned int newSize = (this->size) ? this->size << 1 : 1;    
    
  T* d = nullptr;
  d = (T*)this->Allocate(sizeof(T) * newSize);
  if(this->data){
    memcpy(d, this->data, this->count * sizeof(T));
    this->Free(this->data);
  }
  this->size = newSize;
  this->data = d;  
}
template <typename T>
T* Array<T>::Add(T* element){  
  T* insert = nullptr;
  
  // if(this->count && !this->size){
  //   //This was the stupid serialization fix
  //   this->size = this->count;
  // }
  
  if(!(this->count < this->size))
    this->Expand();
  
  insert = &this->data[this->count];
  
  this->count ++;
  if(element){
    *insert = *element;
  }
  return insert;
}

template <typename T>
T* Array<T>::Add(T element){
  return this->Add(&element);
}
template <typename T>
T* Array<T>::Insert(int index, T* element){
  if(index > this->count) return nullptr;
  
  T* insert = nullptr;

  if(!(this->count < this->size))
    this->Expand();

  insert = &this->data[index];
  
  this-count ++;
  *this->End() = *insert;

  if(element)
    *insert = *element;
     
  return insert;
}
template <typename T>
T* Array<T>::Insert(int index, T element){
  return this->Insert(index, &element);
}
template <typename T>
T* Array<T>::InsertAndKeepOrder(int index, T* element){
  if(!(this->count < this->size))
    this->Expand();

  for(int i = this->count - 1; i >= index; i--){
    this->data[i + 1] = this->data[i];
  }

  this->count++;
  
  if(element){
    this->data[index] = *element;
  }
  return &this->data[index];
}
template <typename T>
T* Array<T>::InsertAndKeepOrder(int index, T element){
  return this->InsertAndKeepOrder(index, &element);
}
template <typename T>
void Array<T>::Remove(int index){
  if(index >= (i32)this->count) return;
  if(this->count > 1)
    this->data[index] = this->data[this->count - 1];
    
  this->count -= 1;
}
template <typename T>
void Array<T>::Remove(T* element){
  this->Remove(GetIndexOf(element));
}
template <typename T>
void Array<T>::RemoveAndKeepOrder(int index){
  if(index >= (i32)this->count) return;

  memcpy(&this->data[index], &this->data[index + 1], (this->count - index - 1) * sizeof(T));
  
  this->count--;
}
template <typename T>
int Array<T>::GetIndexOf(T* element){
  return (int)(element - this->data);
}

template <typename T>
int Array<T>::GetIndexToInsertAt(T element, u64 (*GetComparator)(T)){
  if(!this->count) return 0;
  int start = 0;
  int end = this->count - 1;
  int middle = 0;
  const auto comparator = GetComparator(element);
  while(end >= start){
    middle = start + (end - start) / 2;
    auto it = GetComparator(this->data[middle]);
    if(it == comparator)
      return middle;
                    
    if(it > comparator){
      end = middle - 1;
    }else{
      start = middle + 1;
    }    
  }
  
  if(GetComparator(this->data[middle]) < GetComparator(element))
    middle++;
  
  return middle;
}
template <typename T>
T Array<T>::Pop(){
  if(!this->count)
    *(int*)nullptr = 0;
  auto element = *this->End();
  this->count--;
  return element;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
void Array<T>::Sort_Radix256(u64 (*GetComparator)(T)){
  int counts[256];
  int prefixSum[256];

  T* swapArray = (T*)this->Allocate(sizeof(T) * this->count);
  const int bytes = (int)sizeof(GetComparator(*swapArray));
  const int elements = this->count;
  
  for(int b = 0; b < sizeof(u64); b++){
    memset(counts, 0, sizeof(counts));

    for(int i = 0; i < elements; i++){
      counts[(GetComparator(this->data[i]) >> (b * 8)) & 0xff] ++;
    }

    int sum = 0;
    for(int i = 0; i < 256; i++){
      prefixSum[i] = sum;                
      sum += counts[i];
    }
    
    memset(counts, 0, sizeof(counts));

    for(int i = 0; i < elements; i++){
      int index = (GetComparator(this->data[i]) >> (b * 8)) & 0xFF;
      swapArray[prefixSum[index] + counts[index]] = this->data[i];
      counts[index]++;
    }
    
    T* temp = this->data;
    this->data = swapArray;
    swapArray = temp;
    
  }
  this->Free(swapArray);
}
template <typename T>
T* Array<T>::Search_Binary(T element, u64 (*GetComparator)(T)){
  auto it =this->Search_Binary(element, GetComparator, 0, this->count);
  return it; 
}
template <typename T>
T* Array<T>::Search_Binary(T element, u64 (*GetComparator)(T), i32 start0, i32 end0){
  if(!this->count)
    return nullptr;
 
  int start = start0;
  int end = end0 - 1;
  const auto comparator = GetComparator(element);
  for(;;){
    int middle = start + (end - start) / 2;
    auto it = GetComparator(this->data[middle]);
    if(it == comparator)
      return &this->data[middle];
    
    
    if(start >= end)
      break;
                    
    if(it > comparator){
      end = middle - 1;
    }else{
      start = middle + 1;
    }    
  }
  return nullptr;
}
template <typename T>
T* Array<T>::Search_Linear(T element, u64 (*GetComparator)(T)){
  const int elements = this->count;
  const auto comparator = GetComparator(element);
  
  for(int i = 0; i < elements; i++){
    auto it = GetComparator(this->data[i]);
    if(it == comparator)
      return &this->data[i];
  }
  return nullptr;
}
