#include "PLATFORM_Types.h"
namespace PLATFORM{
  
  u32 Atomic_Increment(volatile u32* dst){
    return __sync_add_and_fetch(dst, 1);
  }
  
  u32 Atomic_CompareExchange_32(volatile u32* dst, u32 exchange, u32 compare){
    return __sync_val_compare_and_swap(dst, compare, exchange);
  }
  void* Atomic_CompareExchange_Pointer(volatile void** dst, void* exchange, void* compare){
    return __sync_val_compare_and_swap((void**)dst, compare, exchange);
  }
};
