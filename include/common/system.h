#pragma once

#if defined(_WIN32) || defined(_WIN64)
    #define IO_OS_WINDOWS
#elif defined(__linux__)
    #define IO_OS_LINUX
#else
    #error "Sorry, your system is not supported"
#endif