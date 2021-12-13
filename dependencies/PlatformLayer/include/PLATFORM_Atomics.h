#pragma once
#include "PLATFORM_Types.h"
namespace PLATFORM{

  //Incremens value in dst and returns the incremented value
  u32 Atomic_Increment(volatile u32* dst);

  //Compares dst to compare.
  //If the values match, value in dst will change to exchange
  //returns the initial value in dst
  u32 Atomic_CompareExchange_32(volatile u32* dst, u32 exchange, u32 compare);
  void* Atomic_CompareExchange_Pointer(volatile void** dst, void* exhange, void* compare);
};
