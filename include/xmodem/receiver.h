#pragma once

#include <xmodem/endpoint.h>

#include <iostream>

namespace XModem
{
	class Receiver : public Endpoint
    {
        virtual bool initialize_transmission() const final;
        bool is_packet_valid(const Packet& packet, const std::uint16_t packet_n) const;

    public:
        Receiver(IO::Serial& serial, ErrorDetection& checksum);

        void receive(std::ostream& ostream);
    };
}