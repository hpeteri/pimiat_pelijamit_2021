#pragma once
#include "PLATFORM_Types.h"

namespace PLATFORM{
  const u64 MICRO_SECONDS_IN_A_SECOND = 1000000;
  u64 GetCycleCounter();
  
  void Init_Time();
  u64 GetClockTime_MicroS();
  
  inline f64 MS_TO_SEC(u64 ms){return ms / (f64)MICRO_SECONDS_IN_A_SECOND;}
};
