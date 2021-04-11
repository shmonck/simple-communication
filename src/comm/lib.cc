#include <comm/lib.h>

#include <spdlog/spdlog.h>

namespace lib
{
    int foo()
    {
        spdlog::info("lib::foo() called!");
        return 42;
    }
}  // namespace lib