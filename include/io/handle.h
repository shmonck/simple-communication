#pragma once

#include <common/system.h>

#if defined(IO_OS_WINDOWS)
    #include <windows.h>
#endif

namespace io
{
#if defined(IO_OS_LINUX)
    using Handle = int;

    constexpr inline Handle IO_NO_HANDLE = 0;
#elif defined(IO_OS_WINDOWS)
    using Handle = HANDLE;

    constexpr inline Handle IO_NO_HANDLE = NULL;
#endif
}  // namespace io