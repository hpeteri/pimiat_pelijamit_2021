#include <windows.h>
#include "PLATFORM_Types.h"
namespace PLATFORM{
  
  u32 Atomic_Increment(volatile u32* dst){
    return InterlockedIncrement(dst);
  }
  u32 Atomic_CompareExchange_32(volatile u32* dst, u32 exchange, u32 compare){
    return InterlockedCompareExchange(dst, exchange, compare);
  }
  void* Atomic_CompareExchange_Pointer(volatile void** dst, void* exchange, void* compare){
    return InterlockedCompareExchangePointer((volatile PVOID*)dst, exchange, compare);
  }
};
