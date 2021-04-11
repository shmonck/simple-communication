#pragma once

#include <common/system.h>

#if defined(PLATFORM_WINDOWS)
    #include <windows.h>
#endif

namespace IO
{
#if defined(PLATFORM_LINUX)
    using Handle = int;

    constexpr inline Handle IO_NO_HANDLE = 0;
#elif defined(PLATFORM_WINDOWS)
    using Handle = HANDLE;

    constexpr inline Handle IO_NO_HANDLE = NULL;
#endif
}  // namespace IO