//////////////////////////////////////////////////////////////////////////////////////////////
//QuadTree
template <typename T>
QuadTreePartition<T>* QuadTree<T>::CreatePartition(f32 min_x, f32 min_y, f32 max_x, f32 max_y, u32 depth){
  auto partition = (typeof(QuadTreePartition<T>)*)this->Allocate(sizeof(QuadTreePartition<T>));

  
  memset(partition, 0, sizeof(*partition));
  partition->min = MATH::Vector2<f32>{min_x, min_y};
  partition->max = MATH::Vector2<f32>{max_x, max_y};;
  partition->isPartitioned = false;
  partition->parentTree = this;
  partition->depth = depth;

  partition->tl = nullptr;
  partition->tr = nullptr;
  partition->bl = nullptr;
  partition->br = nullptr;
  partition->id = ++this->idCounter;
  partition->container.Init(this->Allocate, this->Free);
  
  return partition;
}
template <typename T>
void QuadTree<T>::Init(MATH::Vector2<f32> min, MATH::Vector2<f32> max, u32 depth, void* (*Allocate)(size_t), void (*Free)(void*)){
  this->Allocate = Allocate;
  this->Free = Free;
  this->min = min;
  this->max = max;
  this->maxDepth = depth;
  this->fillRate = 2;

  this->overflow.Init(Allocate, Free);
  this->root = this->CreatePartition(min.x, min.y, max.x, max.y, 0);
}
template <typename T>
void QuadTree<T>::Insert(T element, MATH::Vector2<f32> min, MATH::Vector2<f32> max){
  if(min.x < this->min.x || min.y < this->min.y ||
     max.x > this->max.x || max.y > this->max.y){
    this->overflow.Add(element);

  }
  
  this->root->Insert(element, min, max);
}
template <typename T>
void QuadTree<T>::Remove(T element, MATH::Vector2<f32> min, MATH::Vector2<f32> max){
  if(min.x < this->min.x || min.y < this->min.y ||
     max.x > this->max.x || max.y > this->max.y){
    for(u32 i = 0; i < this->overflow.count; i++){
      auto it = this->overflow[i];
      if(*it == element){
        this->overflow.Remove(i);
        break;
      }
    }
  }
  
  this->root->Remove(element, min, max);
  
}
template <typename T>
void QuadTree<T>::GetUniqueElementsInOccupiedPartitions(MATH::Vector2<f32> min, MATH::Vector2<f32> max, Array<T>* dst){
  dst->count = 0;
  this->root->GetUniqueElementsInOccupiedPartitions(min, max, dst);
}
template <typename T>
const QuadTreePartition<T>* QuadTree<T>::GetPartitionForPoint(MATH::Vector2<f32> point){
  return this->root->GetPartitionForPoint(point);
}
//////////////////////////////////////////////////////////////////////////////////////////////
//Partition
template <typename T>
void QuadTreePartition<T>::Insert(T element, MATH::Vector2<f32> min, MATH::Vector2<f32> max){
  
  if(this->isPartitioned){
    QuadTreePartition<T>* partitions[4] = {this->tl, this->tr, this->bl, this->br};
    for(auto partition : partitions){
      if(min.x < partition->max.x && max.x > partition->min.x &&
         min.y < partition->max.y && max.y > partition->min.y){
      partition->Insert(element, min, max);
      }
    }
    return;
  }  

  auto parent = (QuadTree<T>*)this->parentTree;
  
  if(this->container.count >= parent->fillRate && this->depth < parent->maxDepth){
    if(!this->tl){
      auto halfSize = (this->max - this->min) / 2.0f;
      
      this->tl = parent->CreatePartition(this->min.x, this->min.y + halfSize.y,
                                         this->min.x + halfSize.x, this->max.y,
                                         this->depth + 1);
      this->tr = parent->CreatePartition(this->min.x + halfSize.x, this->min.y + halfSize.y,
                                         this->max.x, this->max.y,
                                         this->depth + 1);
      this->bl = parent->CreatePartition(this->min.x, this->min.y,
                                         this->min.x + halfSize.x, this->min.y + halfSize.y,
                                         this->depth + 1);
      this->br = parent->CreatePartition(this->min.x + halfSize.x, this->min.y,
                                         this->max.x, this->min.y + halfSize.y,
                                         this->depth + 1);
    }
    
    this->isPartitioned = true;
    for(u32 i = 0; i < this->container.count; i++){
      auto entry = this->container[i]; 
      this->Insert(entry->element, entry->min, entry->max);
    }
    this->Insert(element, min, max);
    this->container.count = 0;
  }

  if(!this->isPartitioned){
    if(min.x < this->max.x && max.x > this->min.x &&
       min.y < this->max.y && max.y > this->min.y){
      QuadTreeEntry<T> entry;
      entry.element = element;
      entry.min = min;
      entry.max = max;
      this->container.Add(entry);
    }
  }
}
template <typename T>
void QuadTreePartition<T>::Remove(T element, MATH::Vector2<f32> min, MATH::Vector2<f32> max){
  if(this->isPartitioned){
    QuadTreePartition<T>* partitions[4] = {this->tl, this->tr, this->bl, this->br};
    for(auto partition : partitions){
      if(min.x < partition->max.x && max.x > partition->min.x &&
         min.y < partition->max.y && max.y > partition->min.y){
        partition->Remove(element, min, max);
      }    
    }
    //check if partitions are empty
    for(auto partition : partitions){
      if(partition->isPartitioned) return;
      if(partition->container.count) return;    
    }
    //partitions are empty, so make this the new insertion partition
    this->isPartitioned = false;
  }else{
    for(u32 i = 0; i < this->container.count; i++){
      auto it = this->container[i];
      //Remove the first element found
      if(it->element == element){
        this->container.Remove(i);
        return;
      }
    }
  }    
}
template <typename T>
void QuadTreePartition<T>::GetUniqueElementsInOccupiedPartitions(MATH::Vector2<f32> min, MATH::Vector2<f32> max, Array<T>* dst){

  if(this->isPartitioned){
    QuadTreePartition<T>* partitions[4] = {this->tl, this->tr, this->bl, this->br};
    for(auto partition : partitions){
      if(min.x < partition->max.x && max.x > partition->min.x &&
         min.y < partition->max.y && max.y > partition->min.y){
        partition->GetUniqueElementsInOccupiedPartitions(min, max, dst);
      }
    }
    return;
  }  
  if(!this->isPartitioned){
    if(min.x < this->max.x && max.x > this->min.x &&
       min.y < this->max.y && max.y > this->min.y){

      
      for(u32 i = 0; i < this->container.count; i++){
        auto element = this->container[i]->element; 
        bool found = false;
        for(u32 y = 0; y < dst->count; y++){
          if(*(*dst)[y] == element){
            found = true;
            break;
          }
        }
        if(!found)
          dst->Add(element);
      }
    }
  }
  
}

template <typename T>
const QuadTreePartition<T>* QuadTreePartition<T>::GetPartitionForPoint(MATH::Vector2<f32> point){

  if(this->isPartitioned){
    QuadTreePartition<T>* partitions[4] = {this->tl, this->tr, this->bl, this->br};
    for(auto partition : partitions){
      if(point.x < partition->max.x && point.x > partition->min.x &&
         point.y < partition->max.y && point.y > partition->min.y){
        return partition->GetPartitionForPoint(point);
      }
    }
  }
  return this;
  
  // printf("At :: %d\n", this->id);
  // MATH::Vector4<f32> size;
  // size.x = this->min.x;
  // size.y = this->min.y;
  // size.z = this->max.x;
  // size.w = this->max.y;
  // return size;
}
