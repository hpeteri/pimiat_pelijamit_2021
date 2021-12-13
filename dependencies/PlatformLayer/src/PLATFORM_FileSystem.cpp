#if defined(_WIN32)
#include "PLATFORM_FileSystem_win32.cpp"
#elif defined(__linux__)
#include "PLATFORM_FileSystem_linux.cpp"
#endif

namespace PLATFORM{

}
