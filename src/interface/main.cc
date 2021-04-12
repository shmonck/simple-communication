#include <io/serial.h>
#include <xmodem/endpoint.h>
#include <xmodem/receiver.h>
#include <xmodem/transmitter.h>
#include <xmodem/checksum.h>
#include <xmodem/crc.h>

#include <iostream>
#include <string>
#include <string_view>

#include <fmt/color.h>
#include <fmt/core.h>

std::string_view menu(const std::string_view title, const std::initializer_list<std::string_view> items)
{
    while ( true )
    {
        fmt::print(fmt::fg(fmt::color::light_blue), "{}\n", title);

        for (std::size_t i = 1; const auto item : items)
        {
            fmt::print("{}. {}\n", i, item);
            i++;
        }

        fmt::print("#: ");

        std::string user_input;
        std::getline(std::cin, user_input);

        try
        {
            int choice = std::stoi(user_input);

            if ( choice >= 1 and choice <= std::distance(items.begin(), items.end()) )
            {
                return *(items.begin() + choice - 1);
            }
            else
            {
                fmt::print(fmt::fg(fmt::color::orange), "Please select a valid option.\n");
            }
        }
        catch(...)
        {
            fmt::print(fmt::fg(fmt::color::orange), "Please check your input.\n");
        }
    }
}

int main()
{
    // This is just for testing

    std::string port;
    fmt::print("Enter port: ");
    std::getline(std::cin, port);

    auto serial = IO::Serial();
    if ( !serial.open(port).is_ok() )
    {
        fmt::print(fmt::fg(fmt::color::red), "Could not open specified port\n");
        return 1;
    }

    serial.set_baud_rate(IO::Serial::BaudRate::BAUDRATE_9600);
    serial.set_read_timeout(3000);

    const auto endpoint_option = menu("Endpoint", {"Receiver", "Transmitter"});
    const auto error_detection_option = menu("Error detection", {"Checksum", "CRC"});
    
    fmt::print("File path: ");

    std::string file_path;
    std::getline(std::cin, file_path);

    std::unique_ptr<XModem::ErrorDetection> error_detection;
    if ( error_detection_option == "Checksum" )
    {
        error_detection = std::make_unique<XModem::Checksum>();
    }
    else if ( error_detection_option == "CRC" )
    {
        error_detection = std::make_unique<XModem::CRC>();
    }

    // TODO: Consider removing inheritance and move send/receive to Endpoint

    if ( endpoint_option == "Receiver" )
    {
        std::ofstream ofstream(file_path, std::ios::out | std::ios::trunc | std::ios::binary);
        if ( !ofstream.is_open() )
        {
            fmt::print(fmt::fg(fmt::color::red), "Could not open specified file");
            return 1;
        }

        XModem::Receiver receiver(serial, *error_detection);
        receiver.receive(ofstream);
    }
    else if ( endpoint_option == "Transmitter" )
    {
        std::ifstream ifstream(file_path, std::ios::in | std::ios::binary);
        if ( !ifstream.is_open() )
        {
            fmt::print(fmt::fg(fmt::color::red), "Could not open specified file");
            return 1;
        }

        XModem::Transmitter transmitter(serial, *error_detection);
        transmitter.send(ifstream);
    }
}