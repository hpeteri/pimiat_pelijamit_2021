#ifndef STRINGHASH_JENKINS_H
#define STRINGHASH_JENKINS_H

#include "RENDERER_Types.h"
inline u64 StringHash(const char* string){
  //jenkins_one_at_a_time
  u64 hash = 0;
  for(u64 i = 0; ; i++){
    if(!string[i]) break;
    hash += string[i];
    hash += (hash << 10);
    hash ^= (hash >> 6);
  }
    
  hash += (hash << 3);
  hash ^= (hash >> 11);
  hash += (hash << 15);
  return hash;
}

#endif
