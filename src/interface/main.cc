#include <io/serial.h>

#include <iostream>
#include <string>

#include <fmt/color.h>
#include <fmt/core.h>

int main()
{
    std::string port;
    fmt::print("Enter port: ");
    std::getline(std::cin, port);

    auto serial = io::Serial();
    if ( !serial.open(port).is_ok() )
    {
        fmt::print(fmt::fg(fmt::color::red), "Could not open specified port\n");
        return 1;
    }

    fmt::print(fmt::fg(fmt::color::lime_green), "Port opened, you can send messages now\n");

    while ( true )
    {
        std::string str;
        std::getline(std::cin, str);
        serial.write(str.data(), str.length());
    }
}