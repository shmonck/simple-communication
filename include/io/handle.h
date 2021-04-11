#pragma once

#include <io/system.h>

#if defined(IO_OS_WINDOWS)
    #include <basetsd.h>
#endif

namespace io
{
#if defined(IO_OS_LINUX)
    using Handle = int;
#elif defined(IO_OS_WINDOWS)
    using Handle = HANDLE;
#endif
}  // namespace io