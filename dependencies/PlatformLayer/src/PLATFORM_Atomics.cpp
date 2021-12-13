#if defined(_WIN32)
#include "PLATFORM_Atomics_win32.cpp"
#elif defined(__linux__)
#include "PLATFORM_Atomics_linux.cpp"
#endif
