#if defined(_WIN32)
#include "PLATFORM_Thread_win32.cpp"
#elif defined(__linux__)
#include "PLATFORM_Thread_linux.cpp"
#endif
