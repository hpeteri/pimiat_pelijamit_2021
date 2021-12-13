template <typename T>
T* HashTable<T>::operator[](unsigned int i){
  if(i >= this->count)
    return nullptr;

  return &this->table[this->usedIndices[i]].element;  
}

template <typename T>
void HashTable<T>::Init(void* (*Allocate0)(size_t), void (*Free0)(void*), unsigned long long (*HashFunction0)(const char* string)){
  memset(this, 0, sizeof(*this));
  this->Allocate = Allocate0;
  this->Free = Free0;
  this->HashFunction = HashFunction0;
}

template <typename T>
T* HashTable<T>::Add(const char* string, T element){
  if(!string) return nullptr;
  
  if(this->count == maxCount){ //Expand
    this->size = (this->size) ? this->size << 1 : 2;
    this->maxCount = this->size >> 1;
    
    auto table_old = this->table;
    auto indices_old = this->usedIndices;
    auto count_old = this->count;
    
    this->table = (HashTableNode<T>*)this->Allocate(sizeof(*this->table) * this->size);
    memset(this->table, 0, sizeof(*this->table) * this->size);
    
    this->usedIndices = (unsigned int*)this->Allocate(sizeof(unsigned int) * this->maxCount);
    
    this->count = 0;
    for(unsigned int i = 0; i < count_old; i++){
      auto index = indices_old[i];
      auto it = table_old[index];
      this->Add(it.string, it.element);
      this->Free((void*)it.string);
    }
    this->Free(table_old);
    this->Free(indices_old);
  }
  auto len = strlen(string);
  auto hash = this->HashFunction(string);
  if(!hash) //Need to get a valid hash
    *(int*)nullptr = 0;
  
  int index = hash % this->size;
  
  for(unsigned int i = 0; i < this->size; i++){
    auto it = &this->table[(index + i) % this->size];

    if(it->hash == 0){
      it->hash = hash;
      it->len = len;
      it->string = (const char*)this->Allocate(len + 1);
      memcpy((void*)it->string, string, len + 1);
      this->usedIndices[this->count++] = (index + i) % this->size;
      it->element = element;
      return &it->element;
    }
  }
  *(int*)nullptr = 0; //Should not happen
  return nullptr;
}

template <typename T>
T* HashTable<T>::Find(const char* string){
  if(!string || !this->count)
    return nullptr;
  
  
  auto len = strlen(string);
  auto hash = this->HashFunction(string);
  if(!hash) //Need to get a valid hash
    *(int*)nullptr = 0;

  
  int index = hash % this->size;
  
  for(unsigned int i = 0; i < this->size; i++){
    auto it = &this->table[(index + i) % this->size];

    if(it->hash == 0)
      break;
    
    if(it->hash == hash &&
       it->len == len &&
       !memcmp(it->string, string, it->len)){
      return &it->element;      
    }
  }
  return nullptr;
}

