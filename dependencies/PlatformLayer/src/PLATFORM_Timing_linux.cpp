#include <sys/times.h>
#include <time.h>

#include "PLATFORM_Timing.h"
#include <x86intrin.h>

namespace PLATFORM{
  void Init_Time(){ }

  u64 GetCycleCounter(){
    return __rdtsc();
  }
  u64 GetClockTime_MicroS(){
    timespec t;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
    return (t.tv_sec * MICRO_SECONDS_IN_A_SECOND) + (t.tv_nsec / 1000);
  }
};
