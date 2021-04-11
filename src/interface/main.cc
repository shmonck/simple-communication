#include <comm/lib.h>

#include <fmt/core.h>

int main()
{
    fmt::print("Hello, world!\nlib::foo() returned {}.\n", lib::foo());
}