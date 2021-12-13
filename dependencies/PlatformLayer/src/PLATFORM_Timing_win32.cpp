#include "PLATFORM_Timing.h"

namespace PLATFORM{
  static long long performanceCounterFrequency;

  u64 GetCycleCounter(){
    return __rdtsc();
  }
  void Init_Time(){
    LARGE_INTEGER temp;
    QueryPerformanceFrequency(&temp);
    performanceCounterFrequency = temp.QuadPart;
  }
  u64 GetClockTime_MicroS(){
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return (MICRO_SECONDS_IN_A_SECOND * now.QuadPart / performanceCounterFrequency);
  }
};
