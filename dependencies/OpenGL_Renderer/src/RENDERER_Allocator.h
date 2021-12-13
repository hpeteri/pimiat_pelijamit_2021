#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stdlib.h>
struct Allocator{
  void* (*Allocate)(size_t) = malloc;
  void (*Free)(void*) = free;
};

#endif
