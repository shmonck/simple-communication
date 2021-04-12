#pragma once

#include <xmodem/endpoint.h>

#include <iostream>

namespace XModem
{
    class Transmitter : public Endpoint
    {
        virtual bool initialize_transmission() const final;

    public:
        Transmitter(IO::Serial& serial, ErrorDetection& checksum);

        bool send(std::istream& istream);
    };
}  // namespace XModem