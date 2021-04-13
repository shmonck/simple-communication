#pragma once

#include <xmodem/endpoint.h>

#include <iostream>

namespace XModem
{
    class Receiver : public Endpoint
    {
        virtual bool initialize_transmission() final;
        virtual bool finalize_transmission() const final;

        [[nodiscard]] bool is_packet_valid(const Packet& packet, const std::uint8_t packet_n) const;

        const ErrorDetection* const m_error_detection;

    public:
        Receiver(IO::Serial& serial, const ErrorDetection& error_detection);

        bool receive(std::ostream& ostream);
    };
}  // namespace XModem